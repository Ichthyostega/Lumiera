/*
  DEPEND.hpp  -  access point to singletons and dependencies

  Copyright (C)         Lumiera.org
    2013,               Hermann Vosseler <Ichthyostega@web.de>
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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
 ** @see Singleton_test
 ** @see DependencyConfiguration_test
 */


#ifndef WIP_LIB_DEPEND_H
#define WIP_LIB_DEPEND_H


#include "lib/error.hpp"
#include "lib/nobug-init.hpp"
#include "lib/sync-classlock.hpp"
#include "lib/meta/util.hpp"

#include <boost/noncopyable.hpp>
#include <type_traits>
#include <functional>
#include <memory>


namespace lib {
  namespace error = lumiera::error;
  
  
  namespace { // Implementation helper...
    
    using lib::meta::enable_if;
    
    template<typename TAR, typename SEL =void>
    class InstanceHolder
      : boost::noncopyable
      {
        std::unique_ptr<TAR> instance_;
        
        
      public:
        TAR*
        buildInstance()
          {
            return buildInstance ([]{ return new TAR{}; });
          }
      
        template<class FUN>
        TAR*
        buildInstance(FUN&& ctor)
          {
            if (instance_)
              throw error::Fatal("Attempt to double-create a singleton service. "
                                 "Either the application logic, or the compiler "
                                 "or runtime system is seriously broken"
                                ,error::LUMIERA_ERROR_LIFECYCLE);
            
            instance_.reset (ctor());
            return instance_.get();
          }
      };
    
    template<typename ABS>
    class InstanceHolder<ABS,  enable_if<std::is_abstract<ABS>>>
      {
      public:
        ABS*
        buildInstance(...)
          {
            throw error::Fatal("Attempt to create a singleton instance of an abstract class. "
                               "Application architecture or lifecycle is seriously broken.");
          }
      };
  }//(End)Implementation helper
  
  
  
  /** 
   * @internal access point to reconfigure dependency injection on a per type base
   * @see depend-inject.hpp
   */
  template<class SRV>
  class DependInject;
  
  
  /**
   * Access point to singletons and other kinds of dependencies.
   * Actually this is a Factory object, which is typically placed into a
   * static field of the Singleton (target) class or some otherwise suitable interface.
   * @tparam SRV the class of the Service or Singleton instance
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
  template<class SRV>
  class Depend
    {
      using Factory = std::function<SRV*()>;
      
      static SRV* instance;
      static Factory factory;
      
      static InstanceHolder<SRV> singleton;
      
      friend class DependInject<SRV>;
      
      
    public:
      /** Interface to be used by clients for retrieving the service instance.
       *  Manages the instance creation, lifecycle and access in multithreaded context.
       *  @return instance of class `SRV`. When used in default configuration,
       *          this service instance is a singleton
       */
      SRV&
      operator() ()
        {
          if (!instance)
            retrieveInstance();
          ENSURE (instance);
          return *instance;
        }
      
    private:
      void
      retrieveInstance()
        {
          ClassLock<SRV> guard;
          
          if (!instance)
            {
              if (!factory)
                instance = singleton.buildInstance();
              else
                instance = factory();
              factory = disabledFactory;
            }
        }
      
      static SRV*
      disabledFactory()
        {
          throw error::Fatal("Service not available at this point of the Application Lifecycle"
                            ,error::LUMIERA_ERROR_LIFECYCLE);
        }
    };
  
  
  
  
  /* === allocate Storage for static per type instance management === */
  template<class SRV>
  SRV* Depend<SRV>::instance;
  
  template<class SRV>
  typename Depend<SRV>::Factory Depend<SRV>::factory;
  
  template<class SRV>
  InstanceHolder<SRV> Depend<SRV>::singleton;
  
  
  
} // namespace lib
#endif
