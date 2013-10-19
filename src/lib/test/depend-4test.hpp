/*
  DEPEND4TEST.hpp  -  inject test mock singletons and dependencies

  Copyright (C)         Lumiera.org
    2013,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

====================================================================
This code is heavily inspired by
 The Loki Library (loki-lib/trunk/include/loki/Singleton.h)
    Copyright (c) 2001 by Andrei Alexandrescu
    Loki code accompanies the book:
    Alexandrescu, Andrei. "Modern C++ Design: Generic Programming
        and Design Patterns Applied".
        Copyright (c) 2001. Addison-Wesley. ISBN 0201704315

*/



#ifndef LIB_TEST_DEPEND_4TEST_H
#define LIB_TEST_DEPEND_4TEST_H


#include "lib/sync-classlock.hpp"
#include "lib/depend.hpp"

#include "lib/meta/duck-detector.hpp"   ////TODO move in separate header

#include <boost/scoped_ptr.hpp>         ////TODO move in separate header
#include <boost/noncopyable.hpp>        ////TODO move in separate header
#include <boost/utility/enable_if.hpp>  ////TODO move in separate header


namespace lib {
  
  /**
   * Access point to singletons and other kinds of dependencies.
   * Actually this is a Factory object, which is typically placed into a static field
   * of the Singleton (target) class or some otherwise suitable interface.
   * @note uses static fields internally, so all factory instances share pInstance_
   * @remark there is an ongoing discussion regarding the viability of the
   *   Double Checked Locking pattern, which requires either the context of a clearly defined
   *   language memory model (as in Java), or needs to be supplemented by memory barriers.
   *   In our case, this debate boils down to the question: does \c pthread_mutex_lock/unlock
   *   constitute a memory barrier, such as to force any memory writes happening \em within
   *   the singleton ctor to be flushed and visible to other threads when releasing the lock?
   *   To my understanding, the answer is yes. See
   *   [POSIX](http://www.opengroup.org/onlinepubs/000095399/basedefs/xbd_chap04.html#tag_04_10)
   * @param SI the class of the Singleton instance
   * @param Create policy defining how to create/destroy the instance
   * @param Life policy defining how to manage Singleton Lifecycle
   */
  template<class SI>
  class Depend
    {
      typedef ClassLock<SI> SyncLock;
      
      static SI* volatile instance;
      static DependencyFactory factory;
      
      
    public:
      /** Interface to be used by clients to access the service instance.
       *  Manages the instance creation, lifecycle and access in multithreaded context.
       *  @return instance of class SI. When used in default configuration,
       *          this service instance is a singleton
       */
      SI&
      operator() ()
        {
          if (!instance)
            {
              SyncLock guard;
              
              if (!instance)
                instance = static_cast<SI*> (factory.buildInstance());
            }
          ENSURE (instance);
          return *instance;
        }
      
      
      
      typedef DependencyFactory::InstanceConstructor Constructor;
      
      
      /** default configuration of the dependency factory
       * is to build a singleton instance on demand */
      Depend()
        {
          factory.ensureInitialisation (buildSingleton<SI>());
        }
      
      /**
       * optionally, the instance creation process can be explicitly configured
       * \em once per type. By default, a singleton instance will be created.
       * Installing another factory function enables other kinds of dependency injection;
       * this configuration must be done prior to any use the dependency factory.
       * @param ctor a constructor function, which will be invoked on first usage.
       * @note basically a custom constructor function is responsible to manage any
       *         created service instances. Optionally it may install a deleter function
       *         via \c DependencyFactory::scheduleDestruction(void*,KillFun)
       * @remark typically the \c Depend<TY> factory will be placed into a static variable,
       *         embedded into another type or interface. In this case, actual storage for
       *         this static variable needs to be allocated within some translation unit.
       *         And this is the point where this ctor will be invoked, in the static
       *         initialisation phase of the respective translation unit (*.cpp)
       */
      Depend (Constructor ctor)
        {
          factory.installConstructorFunction (ctor);
        }
      
      // standard copy operations applicable
      
      
      
      /* === Management / Test support interface === */
      
      /** disable and destroy the actual service instance explicitly.
       *  Next access will re-invoke the factory to create a new instance.
       * @warning this is a very dangerous operation. Concurrent accesses
       *          might get NULL or even a reference to the old instance,
       *          which, worse still, resides typically in the same memory
       *          location as the new instance. The only way to prevent this
       *          would be to synchronise any \em access (which is expensive)
       *          Thus it is the client's duty to ensure there is no such
       *          concurrent access, i.e. all clients of the old instance
       *          should be disabled prior to invoking this function.
       */
      static void
      shutdown()
        {
          SyncLock guard;
          
          factory.deconfigure (instance);
          instance = NULL;
        }
      
      /** temporarily replace the service instance.
       *  The purpose of this operation is to support unit testing.
       * @param mock reference to an existing service instance (mock).
       * @return reference to the currently active service instance.
       * @warning not threadsafe. Same considerations as for \c shutdown() apply
       * @remark the replacement is not actively managed by the DependencyFactory,
       *       it remains in ownership of the calling client (unit test). Typically
       *       this test will keep the returned original service reference and
       *       care for restoring the original state when done.
       * @see Depend4Test scoped object for automated test mock injection
       */
      static SI*
      injectReplacement (SI* mock)
        {
          SyncLock guard;
          SI* currentInstance = instance; 
          instance = mock;
          return currentInstance;
        }
    };
  
  
  
  // Storage for SingletonFactory's static fields...
  template<class SI>
  SI* volatile Depend<SI>::instance;
  
  template<class SI>
  DependencyFactory Depend<SI>::factory;
  
  
  
  
  namespace { ///< details: inject a mock automatically in place of a singleton
    
    using boost::enable_if;
    using lib::meta::Yes_t;
    using lib::meta::No_t;
    
    /**
     * Metafunction: does the Type in question
     * give us a clue about what service interface to use?
     */
    template<class MOCK>
    class defines_ServiceInterface
      {
        META_DETECT_NESTED  (ServiceInterface);
        
      public:
        enum{ value = HasNested_ServiceInterface<MOCK>::value
            };
      };
    
    
    /**
     * Policy-Trait: determine the access point to install the mock.
     * @note either the mock service implementation needs to provide
     *       explicitly a typedef for the ServiceInterface, or we're
     *       just creating a separate new instance of the singleton service,
     *       while shadowing (but not destroying) the original instance.
     */
    template<class I, class YES =void>
    struct ServiceInterface
      {
        typedef I Type;
      };
    
    
    template<class MOCK>
    struct ServiceInterface<MOCK, typename enable_if< defines_ServiceInterface<MOCK> >::type>
      {
        typedef typename MOCK::ServiceInterface Type;
      };
    
  }//(End) mock injection details
  
  
  /**
   * Scoped object for installing/deinstalling a mocked service automatically.
   * Placing a suitably specialised instance of this template into a local scope
   * will inject the corresponding mock installation and remove it when the
   * control flow leaves this scope.
   * @param TYPE the concrete mock implementation type to inject. It needs to
   *        be default constructible. If TYPE is a subclass of the service interface,
   *        it needs to expose a typedef \c ServiceInterface
   */
  template<class TYPE>
  struct Depend4Test
    : boost::noncopyable
    {
      typedef typename ServiceInterface<TYPE>::Type Interface;
      
      boost::scoped_ptr<TYPE> mock_;
      Interface* shadowedOriginal_;
      
      Depend4Test()
        : mock_(new TYPE)
        , shadowedOriginal_(Depend<Interface>::injectReplacement (mock_.get()))
        { }
      
     ~Depend4Test()
        {
          ENSURE (mock_);
          Depend<Interface>::injectReplacement (shadowedOriginal_);
        }
    };
  
  
} // namespace lib
#endif
