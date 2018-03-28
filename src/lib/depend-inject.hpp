/*
  DEPEND-INJECT.hpp  -  managing the lifecycle of singletons and dependencies

  Copyright (C)         Lumiera.org
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


/** @file depend-inject.hpp
 ** Per type specific configuration of instances created as service dependencies.
 ** This is the _"Backstage Area"_ of lib::Depend, where the actual form and details
 ** of instance creation can be configured in various ways. Client code typically
 ** plants an instance of lib::Depend, templated to the actual type of the dependency.
 ** This is a way to express the dependency on some interface or service, while not
 ** expanding on any details as to when and how this dependency is created. Without
 ** and explicit configuration, lib::Depend will automatically create and manage a
 ** singleton instance of the type given as type parameter.
 ** 
 ** ## Architecture
 ** A _dependency_ is understood as something we need to do the task at hand, yet
 ** a dependency lies beyond that task and relates to concerns outside the scope
 ** and theme of this actual task. The usage site of the dependency is only bound
 ** and coupled to the _interface_ exposed as dependency, and the associated
 ** _contract_ of a service. Initially, such a dependency is _dormant_ and will
 ** be activated on first access. This simplifies the bootstrap of complex
 ** interwoven structures; it suffices to ensure that none of the participating
 ** entities actually starts to work before all of the setup and wiring is done.
 ** 
 ** For that reason, lib::DependInject<SRV> is meant to be used at the site providing
 ** the _actual_ service or implementation subtype -- not at the site consuming a
 ** dependency (through lib::Depend<SRV>). This choice also means that the actual
 ** configuration of services is not centralised, and can not be static; it need
 ** to happen prior to any service access (on violation error::Logic is raised)
 ** 
 ** The public configuration mechanisms offered by DependInject address various concerns:
 ** 
 ** \paragraph Wiring
 ** Even when relying on lazy on-demand initialisation, a concrete service implementation
 ** typically needs to connect to further services, and maybe even decide upon the actual
 ** subclass to be instantiated. By invoking the DependInject<SRV>::useSingleton(FUN)
 ** function, a functor or lambda can be installed into the static factory of lib::Depend.
 ** Especially, a lambda could be bound into the internal context of the service provider.
 ** This function is expected to deliver a heap allocated instance on invocation, which
 ** will be owned and managed by lib::Depend<SRV>::singleton (An InstanceHolder<SRV>).
 ** 
 ** \paragraph Service Lifecycle
 ** Whenever a module or subsystem can be started and stopped, several interconnected
 ** services become operational together, with dependent lifecycle. It is possible to
 ** expose such services through a lib::Depend<SRV> front-end; this way, the actual
 ** usage context remains agnostic with respect to details of the lifecycle. Any
 ** access while the service is _not_ available will just raise an error::Logic.
 ** This kind of configuration can be achieved by planting a smart-handle of type
 ** DependInject<SRV>::ServiceInstance<IMP>
 ** 
 ** \paragraph Unit Testing with Mock Services
 ** Dependencies tend to hamper unit testing, but global variables and actively
 ** linked and collaborating implementations are worse and often prevent test coverage
 ** altogether. Preferably dependencies are limited to an interface and a focused topic,
 ** and then it might be possible to inject a _mock implementation_ locally within the
 ** unit test. Such a mock instance temporarily shadows any previously existing state,
 ** instance or configuration for this dependency; the mock object can be rigged and
 ** instrumented by the test code to probe or observe the subject's behaviour. This
 ** concept can only work when the test subject does not cache any state and really
 ** pulls the dependency whenever necessary.
 ** 
 ** 
 ** ## Performance, concurrency and sanity
 ** 
 ** The lib::Depend<SRV> front-end is optimised for the access path. It uses an
 ** std::atomic to hold the instance pointer and a class-level Mutex to protect
 ** the initialisation phase. On the other hand, initialisation happens only once
 ** and will be expensive anyway. And, most importantly, for any non-standard
 ** configuration we assume, that -- by architecture -- there is _no contention_
 ** between usage and configuration. Services are to be started in a dedicated bootstrap
 ** phase, and unit tests operated within a controlled, single threaded environment.
 ** For this reason, any configuration grabs the lock, and publishes via the default
 ** memory order of std::atomic (which is std::memory_order_seq_cst). Any spotted
 ** collision or inconsistency raises an exception, which typically should not
 ** be absorbed, but rather trigger component, subsystem or application shutdown.
 ** @warning there is a known coherency breach in "emergency shutdown": When a
 **     subsystem collapses unexpectedly, its root handler signals the subsystem
 **     runner to initiate emergency shutdown. However, the collapsed subsystem
 **     is already defunct at that point, which breaks the the general contract
 **     of prerequisite subsystems to be available in operative mode. Lumiera
 **     is not built as a resilient service in that respect, but we also
 **     mandate for any parts not to cache essential work results in
 **     transient memory; actions are logged and performed for
 **     persistent history and UNDO.
 ** 
 ** @see DependencyConfiguration_test
 ** @see SingletonSubclass_test
 ** @see subsys.hpp
 */




#ifndef LIB_DEPEND_INJECT_H
#define LIB_DEPEND_INJECT_H



#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/depend2.hpp"
#include "lib/meta/trait.hpp"
#include "lib/meta/function.hpp"
#include "lib/sync-classlock.hpp"

#include <type_traits>
#include <utility>
#include <memory>


namespace lib {
  namespace error = lumiera::error;
  
  using std::forward;
  using std::move;
  
  
  
  
  /**
   * This framework allows to (re)configure the lib::Depend front-end for dependency-injection.
   * By default, `Depend<TY>` will create a singleton instance of `TY` lazily, on demand.
   * When instantiating one of the configuration handles provided here -- _prior_ to using
   * retrieving the instance through `Depend<TY>` -- this default (singleton) behaviour
   * can be reconfigured in various ways, without the client being aware of it
   * - instead of a singleton, a service instance with well defined lifecycle can be
   *   exposed through the `Depend<TY>` front-end. When the service is shut down,
   *   clients will receive an exception on access.
   * - instead of the interface type `TY` mentioned in `Depend<TY>`, an implementation subclass
   *   can be specified, optionally with a closure for the actual creation of this subclass
   *   singleton type, which still happens lazily, on demand
   * - the current state and configuration can be shadowed temporarily by a test mock instance,
   *   which is managed automatically and removed when leaving the scope of the test.
   */
  template<class SRV>
  class DependInject
    : util::NoInstance
    {
      using Factory = typename Depend<SRV>::Factory;
      using Lock    = typename Depend<SRV>::Lock;
      
    public:
      /** configure dependency-injection for type SRV to build a subclass singleton
       * @tparam SUB concrete subclass type to build on demand when invoking `Depend<SRV>`
       * @throws error::Logic (LUMIERA_ERROR_LIFECYCLE) when the default factory has already
       *         been invoked at the point when calling this (re)configuration function.
       */
      template<class SUB>
      static void
      useSingleton()
        {
          installFactory ([]{ return & Depend<SUB>{}(); });
        }
      
      /** configure dependency-injection for type SRV to manage a subclass singleton,
       *  which is created lazily on demand by invoking the given builder function
       * @param ctor functor to create a heap allocated instance of subclass
       * @throws error::Logic (LUMIERA_ERROR_LIFECYCLE) when the default factory has already
       *         been invoked at the point when calling this (re)configuration function.
       */
      template<class FUN>
      static void
      useSingleton(FUN&& ctor)
        {
          using Fun = typename SubclassFactory<FUN>::Fun;
          using Sub = typename SubclassFactory<FUN>::Sub;
          __assert_compatible<Sub>();
          
          installFactory (buildCustomSingleton<Sub,Fun> (forward<FUN> (ctor)));
        }
      
      
      /**
       * Configuration handle to expose a service implementation through the `Depend<SRV>` front-end.
       * This non-copyable (but movable) handle shall be planted within the context operating the service
       * to be exposed. It will immediately create (in RAII style) and manage a heap-allocated instance
       * of the subclass `IMP` and expose a baseclass pointer to this specific instance through `Depend<SRV>`.
       * Moreover, the implementation subclass can be accessed through this handle, which acts as smart-ptr.
       * When the handle goes out of scope, the implementation instance is destroyed and the access through
       * `Depend<SRV>` is closed and inhibited, to prevent on-demand creation of a baseclass `SRV` singleton.
       * @tparam IMP concrete service implementation subclass to build, manage and expose.
       * @throws error::Logic (LUMIERA_ERROR_LIFECYCLE) when the default factory has already
       *         been invoked at the point when calling this (re)configuration function.
       */
      template<class IMP>
      class ServiceInstance
        : util::MoveOnly
        {
          std::unique_ptr<IMP> instance_;
          
        public:
          template<typename...ARGS>
          ServiceInstance(ARGS&& ...ctorArgs)
            : instance_(new IMP(forward<ARGS> (ctorArgs)...))
            {
              __assert_compatible<IMP>();
              activateServiceAccess (*instance_);
            }
          
         ~ServiceInstance()
            {
              deactivateServiceAccess();
            }
          
          explicit
          operator bool()  const
            {
              return bool(instance_);
            }
          
          IMP&
          operator* ()  const
            {
              ENSURE (instance_);
              return *instance_;
            }
          
          IMP*
          operator-> ()  const
            {
              ENSURE (instance_);
              return instance_.get();
            }
        };
      
      
      /**
       * Configuration handle for temporarily shadowing a dependency by a test mock instance.
       * This non-copyable (but movable) handle shall be planted within the immediate test context.
       * It immediately stashes away the existing state and configuration from `Depend<SRV>`, but
       * waits for actual invocation of the `Depend<SRV>`-front-end to create a heap-allocated
       * instance of the `MOC` subclass, which it manages and exposes like a smart-ptr.
       * When the handle goes out of scope, the original state and configuration is restored
       */
      template<class MOC>
      class Local
        : util::MoveOnly
        {
          std::unique_ptr<MOC> mock_;
          
          SRV* origInstance_;
          Factory origFactory_;
          
        public:
          Local()
            : Local([]{ return new MOC{}; })
          { }
          
          template<class FUN>
          explicit
          Local (FUN&& buildInstance)
            {
              __assert_compatible<MOC>();
              __assert_compatible<typename SubclassFactory<FUN>::Sub>();
              
              temporarilyInstallAlternateFactory (origInstance_, origFactory_
                                                 ,[=]()
                                                     {
                                                        mock_.reset (buildInstance());
                                                        return mock_.get();
                                                     });
            }
         ~Local()
            {
              restoreOriginalFactory (origInstance_, origFactory_);
            }
          
          explicit
          operator bool()  const
            {
              return bool(mock_);
            }
          
          MOC&
          operator* ()  const
            {
              ENSURE (mock_);
              return *mock_;
            }
          
          MOC*
          operator-> ()  const
            {
              ENSURE (mock_);
              return mock_.get();
            }
        };
      
      
      
    protected: /* ======= internal access-API for those configurations to manipulate Depend<SRV> ======= */
      template<class IMP>
      friend class ServiceInstance;
      template<class MOC>
      friend class Local;
      
      
      template<class SUB>
      static void
      __assert_compatible()
        {
          static_assert (meta::is_Subclass<SUB,SRV>()
                        ,"Installed implementation class must be compatible to the interface.");
        }
      
      template<typename FUN>
      struct SubclassFactory
        {
          static_assert (meta::_Fun<FUN>(),
                         "Need a Lambda or Function object to create a heap allocated instance");
          
          using Fun = typename meta::_Fun<FUN>::Functor;   // suitable type to store for later invocation 
          using Res = typename meta::_Fun<FUN>::Ret;
          using Sub = typename meta::Strip<Res>::TypePlain;
          
          static_assert (std::is_pointer<Res>::value,
                         "Function must yield a pointer to a heap allocated instance");
        };
      
      
      static void
      installFactory (Factory&& otherFac)
        {
          Lock guard;
          if (Depend<SRV>::instance)
            throw error::Logic("Attempt to reconfigure dependency injection after the fact. "
                               "The previously installed factory (typically Singleton) was already used."
                              , error::LUMIERA_ERROR_LIFECYCLE);
          Depend<SRV>::factory = move (otherFac);
        }
      
      /** wrap custom factory function to plant a singleton instance
       * @remark call through this intermediary function because we need to capture a _copy_ of the functor,
       *  to invoke it later, on-demand. Especially we need the ability to change the type of this functor,
       *  since sometimes the argument is passed as function reference, which can not be instantiated,
       *  but needs to be wrapped into a std::function. */
      template<class SUB, class FUN>
      static Factory
      buildCustomSingleton (FUN&& ctor)
        {
          static std::unique_ptr<SUB> singleton;
          return ([ctor]()                    // copy of ctor in the closure
                       {
                         singleton.reset (ctor());
                         return singleton.get();
                       });
        }
      
      static void
      temporarilyInstallAlternateFactory (SRV*& stashInstance, Factory& stashFac, Factory&& newFac)
        {
          Lock guard;
          stashFac = move(Depend<SRV>::factory);
          stashInstance = Depend<SRV>::instance;
          Depend<SRV>::factory = move(newFac);
          Depend<SRV>::instance = nullptr;
        }
      
      static void
      restoreOriginalFactory (SRV*& stashInstance, Factory& stashFac)
        {
          Lock guard;
          Depend<SRV>::factory = move(stashFac);
          Depend<SRV>::instance = stashInstance;
        }
      
      static void
      activateServiceAccess (SRV& newInstance)
        {
          Lock guard;
          if (Depend<SRV>::instance)
            throw error::Logic("Attempt to activate an external service implementation, "
                               "but another instance has already been dependency-injected."
                              , error::LUMIERA_ERROR_LIFECYCLE);
          Depend<SRV>::instance = &newInstance;
          Depend<SRV>::factory = Depend<SRV>::disabledFactory;
        }
      
      static void
      deactivateServiceAccess()
        {
          Lock guard;
          Depend<SRV>::instance = nullptr;
          Depend<SRV>::factory = Depend<SRV>::disabledFactory;
        }
    };
  
  
  
} // namespace lib
#endif
