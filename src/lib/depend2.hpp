/*
  DEPEND.hpp  -  access point to singletons and dependencies

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



/** @file depend.hpp
 ** Singleton services and Dependency Injection.
 ** The <b>Singleton Pattern</b> provides a single access point to a class or
 ** service and exploits this ubiquitous access point to limit the number of objects
 ** of this type to a single shared instance. Within Lumiera, we mostly employ a
 ** factory template for this purpose; the intention is to use on-demand initialisation
 ** and a standardised lifecycle. In the default configuration, this \c Depend<TY> factory
 ** maintains a singleton instance of type TY. The possibility to install other factory
 ** functions allows for subclass creation and various other kinds of service management.
 ** 
 ** 
 ** # Why Singletons? Inversion-of-Control and Dependency Injection
 ** 
 ** Singletons are frequently over-used, and often they serve as disguised
 ** global variables to support a procedural programming style. As a remedy, typically
 ** the use of a »Dependency Injection Container« is promoted. And -- again typically --
 ** these DI containers tend to evolve into heavyweight universal tools and substitute
 ** the original problem by metadata hell.
 ** 
 ** Thus, for Lumiera, the choice to use Singletons was deliberate: we understand the
 ** Inversion-of-Control principle, yet we want to stay just below the level of building
 ** a central application manager core. At the usage site, we access a factory for some
 ** service *by name*, where the »name« is actually the type name of an interface or
 ** facade. Singleton is used as an _implementation_ of this factory, when the service
 ** is self-contained and can be brought up lazily.
 ** 
 ** ## Conventions, Lifecycle and Unit Testing
 ** 
 ** Usually we place an instance of the singleton factory (or some other kind of factory)
 ** as a static variable within the interface class describing the service or facade.
 ** As a rule, everything accessible as Singleton is sufficiently self-contained to come
 ** up any time -- even prior to `main()`. But at shutdown, any deregistration must be done
 ** explicitly using a lifecycle hook. Destructors aren't allowed to do _any significant work_
 ** beyond releasing references, and we acknowledge that singletons can be released
 ** in _arbitrary order_.
 ** 
 ** @todo WIP-WIP 3/18 rework of the singleton / dependency factory is underway
 ** 
 ** @see lib::Depend
 ** @see lib::DependencyFactory
 ** @see lib::test::Depend4Test
 ** @see singleton-test.cpp
 ** @see dependency-factory-test.cpp
 */


#ifndef WIP_LIB_DEPEND_H
#define WIP_LIB_DEPEND_H


#include "lib/sync-classlock.hpp"
#include "lib/dependency-factory2.hpp"


namespace lib {
  
  /**
   * Access point to singletons and other kinds of dependencies.
   * Actually this is a Factory object, which is typically placed into a
   * static field of the Singleton (target) class or some otherwise suitable interface.
   * @param SI the class of the Singleton instance
   * @note uses static fields internally, so all factory configuration is shared per type
   * @remark there is an ongoing discussion regarding the viability of the
   *   Double Checked Locking pattern, which requires either the context of a clearly defined
   *   language memory model (as in Java), or needs to be supplemented by memory barriers.
   *   In our case, this debate boils down to the question: does \c pthread_mutex_lock/unlock
   *   constitute a memory barrier, such as to force any memory writes happening \em within
   *   the singleton ctor to be flushed and visible to other threads when releasing the lock?
   *   To my understanding, the answer is yes. See
   *   [POSIX](http://www.opengroup.org/onlinepubs/000095399/basedefs/xbd_chap04.html#tag_04_10)
   * @remark we could consider to rely on a _Meyers Singleton_, where the compiler automatically
   *   generates the necessary code and guard variable to ensure single-threaded initialisation
   *   of the instance variable. But the downside of this approach is that we'd loose access
   *   to the singleton instance variable, which then resides within the scope of a single
   *   access function. Such would counterfeit the ability to exchange the instance to
   *   inject a mock for unit testing.
   * @todo WIP-WIP 3/18 rework of the singleton / dependency factory is underway   /////////////////////TICKET #1086
   * @param SI the class of the Singleton instance
   */
  template<class SI>
  class Depend2
    {
      typedef ClassLock<SI> SyncLock;
      
      static SI* volatile instance;
      static DependencyFactory2 factory;
      
      
    public:
      /** Interface to be used by clients for retrieving the service instance.
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
      
      
      
      typedef DependencyFactory2::InstanceConstructor Constructor;
      
      
      /** default configuration of the dependency factory
       *  is to build a singleton instance on demand */
      Depend2()
        {
          factory.ensureInitialisation (buildSingleton2<SI>());
        }
      
      /**
       * optionally, the instance creation process can be configured explicitly
       * \em once per type. By default, a singleton instance will be created.
       * Installing another factory function enables other kinds of dependency injection;
       * this configuration must be done \em prior to any use the dependency factory.
       * @param ctor a constructor function, which will be invoked on first usage.
       * @note basically a custom constructor function is responsible to manage any
       *         created service instances.
       * @remark typically the \c Depend<TY> factory will be placed into a static variable,
       *         embedded into another type or interface. In this case, actual storage for
       *         this static variable needs to be allocated within some translation unit.
       *         And this is the point where this ctor will be invoked, in the static
       *         initialisation phase of the respective translation unit (*.cpp)
       */
      Depend2 (Constructor ctor)
        {
          factory.installConstructorFunction (ctor);
        }
      
      // standard copy operations applicable
      
      
      
      /* === Management / Test support interface === */
      
      /** temporarily replace the service instance.
       *  The purpose of this operation is to support unit testing.
       * @param mock reference to an existing service instance (mock).
       * @return reference to the currently active service instance.
       * @warning this is a dangerous operation and not threadsafe.
       *       Concurrent accesses might still get the old reference;
       *       the only way to prevent this would be to synchronise
       *       \em any access (which is too expensive).
       *       This feature should only be used for unit tests thusly.
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
  
  
  
  
  // Storage for static per type instance management...
  template<class SI>
  SI* volatile Depend2<SI>::instance;
  
  template<class SI>
  DependencyFactory2 Depend2<SI>::factory;
  
  
  
} // namespace lib
#endif
