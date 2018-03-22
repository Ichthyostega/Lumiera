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


/** @file dependency-factory.hpp
 ** Per type specific configuration of instances created as service dependencies.
 ** @todo WIP-WIP 3/18 rework of the singleton / dependency factory is underway
 ** @see DependencyConfiguration_test
 */




#ifndef LIB_DEPEND_INJECT_H
#define LIB_DEPEND_INJECT_H



#include "lib/error.hpp"
#include "lib/depend2.hpp"
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
  struct DependInject
    {
      using Factory = typename Depend<SRV>::Factory;
      
      
      /** configure dependency-injection for type SRV to build a subclass singleton
       * @tparam SUB concrete subclass type to build on demand when invoking `Depend<SRV>`
       * @throws error::Logic (LUMIERA_ERROR_LIFECYCLE) when the default factory has already
       *         been invoked at the point when calling this (re)configuration function.
       */
      template<class SUB>
      static void
      useSingleton()
        {
          useSingleton ([]{ return new SUB{}; });
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
          using lib::meta::_Fun;
          using lib::meta::Strip;
          
          static_assert (_Fun<FUN>::value, "Need a Lambda or Function object to create a heap allocated instance");
          
          using Ret = typename _Fun<FUN>::Ret;
          using Sub = typename Strip<Ret>::TypePlain;
          
          __assert_compatible<Sub>();
          static_assert (std::is_pointer<Ret>::value, "Function must yield a pointer to a heap allocated instance");
          
          static InstanceHolder<Sub> singleton;
          installFactory ([ctor]()
                              {
                                return singleton.buildInstance (ctor);
                              });
        }
      
      
      /**
       * Configuration handle to expose a service implementation through the `Depend<SRV>` front-end.
       * This noncopyable (but movable) handle shall be planted within the context operating the service
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
          
          ServiceInstance (ServiceInstance&&)            = default;
          ServiceInstance (ServiceInstance const&)       = delete;
          ServiceInstance& operator= (ServiceInstance&&) = delete;
          
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
       * This noncopyable (but movable) handle shall be planted within the immediate test context.
       * It immediately stashes away the existing state and configuration from `Depend<SRV>`, but
       * waits for actual invocation of the `Depend<SRV>`-front-end to create a heap-allocated
       * instance of the `MOC` subclass, which it manages and exposes like a smart-ptr.
       * When the handle goes out of scope, the original state and configuration is restored
       */
      template<class MOC>
      class Local
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
          
          Local (Local&&)            = default;
          Local (Local const&)       = delete;
          Local& operator= (Local&&) = delete;
          
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
          static_assert (std::is_base_of<SRV,SUB>::value,
                         "Installed implementation class must be compatible to the interface.");
        }
      
      static void
      installFactory (Factory&& otherFac)
        {
          ClassLock<SRV> guard;
          if (Depend<SRV>::instance)
            throw error::Logic("Attempt to reconfigure dependency injection after the fact. "
                               "The previously installed factory (typically Singleton) was already used."
                              , error::LUMIERA_ERROR_LIFECYCLE);
          Depend<SRV>::factory = move (otherFac);
        }
      
      static void
      temporarilyInstallAlternateFactory (SRV*& stashInstance, Factory& stashFac, Factory&& newFac)
        {
          ClassLock<SRV> guard;
          stashFac = move(Depend<SRV>::factory);
          stashInstance = Depend<SRV>::instance;
          Depend<SRV>::factory = move(newFac);
          Depend<SRV>::instance = nullptr;
        }
      
      static void
      restoreOriginalFactory (SRV*& stashInstance, Factory& stashFac)
        {
          ClassLock<SRV> guard;
          Depend<SRV>::factory = move(stashFac);
          Depend<SRV>::instance = stashInstance;
        }
      
      static void
      activateServiceAccess (SRV& newInstance)
        {
          ClassLock<SRV> guard;
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
          ClassLock<SRV> guard;
          Depend<SRV>::instance = nullptr;
          Depend<SRV>::factory = Depend<SRV>::disabledFactory;
        }
    };
  
  
  
  
} // namespace lib
#endif
