/*
  DEPEND.hpp  -  access point to singletons and dependencies

   Copyright (C)
     2013,            Hermann Vosseler <Ichthyostega@web.de>
     2018,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file depend.hpp
 ** Singleton services and Dependency Injection.
 ** The <b>Singleton Pattern</b> provides a single access point to a class or
 ** service and exploits this ubiquitous access point to limit the number of objects
 ** of this type to a single shared instance. Within Lumiera, we mostly employ a
 ** factory template for this purpose; the intention is to use on-demand initialisation
 ** and a standardised lifecycle. In the default configuration, this `Depend<TY>` factory
 ** maintains a singleton instance of type `TY`. The possibility to install other factory
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
 ** explicitly using a lifecycle hook. In Lumiera, destructors aren't allowed to do
 ** _any significant work_ beyond releasing references, and we acknowledge that
 ** singletons can be released in _arbitrary order_.
 ** 
 ** Lifecycle and management of dependencies is beyond the scope of this access mechanism
 ** exposed here. However, the actual product to be created or exposed lazily can be
 ** configured behind the scenes, as long as this configuration is performed _prior_
 ** to the first access. This configuration is achieved with the help of the "sibling"
 ** template lib::DependInject, which is declared friend within `Depend<T>` for type `T`
 ** - a service with distinct lifecycle can be exposed through the `Depend<T>` front-end
 ** - it is possible to create a mock instance, which temporarily shadows what
 **   Depend<T> delivers on access.
 ** 
 ** ## Implementation and performance
 ** 
 ** Due to this option for flexible configuration, the implementation can not be built
 ** as Meyer's Singleton. Rather, Double Checked Locking of a Mutex is combined with
 ** an std::atomic to work around the known (rather theoretical) concurrency problems.
 ** Microbenchmarks indicate that this implementation technique ranges close to the
 ** speed of a direct access to an already existing object; in the fully optimised
 ** variant it was found to be roughly at ≈ 1ns and thus about 3 to 4 times slower
 ** than the comparable unprotected direct access without lazy initialisation.
 ** This is orders of magnitude better than any flavour of conventional locking.
 ** 
 ** ### Initialisation of the shared factory
 ** 
 ** We package the creation and destruction functors for the object to be managed
 ** into a factory, which is shared per type. Such a shared factory could live within
 ** a static member field `Depend<TY>::factory` -- however, the _definition_ of such
 ** a templated static member happens on first use of the enclosing template _instance_,
 ** and it seems the _initialisation order_ of such fields is not guaranteed, especially
 ** when used prior to main, from static initialisation code. For that reason, we manage
 ** the _factory_ as Meyer's singleton, so it can be accessed independently from the
 ** actual target object's lifecycle and the compiler will ensure initialisation
 ** prior to first use. To ensure the lifespan of this embedded factory object
 ** extends beyond the last instance of `lib::Depend<TY>`, we also need to
 ** access that factory from a ctor
 ** 
 ** @see depend-inject.hpp
 ** @see lib::DependInject
 ** @see Singleton_test
 ** @see DependencyConfiguration_test
 */


#ifndef LIB_DEPEND_H
#define LIB_DEPEND_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/nobug-init.hpp"
#include "lib/sync-classlock.hpp"
#include "lib/zombie-check.hpp"
#include "lib/meta/util.hpp"

#include <type_traits>
#include <functional>
#include <atomic>
#include <memory>


namespace lib {
  namespace error = lumiera::error;
  
  
  /**
   * Helper to abstract creation and lifecycle of a dependency.
   * Holds a configurable constructor function and optionally
   * an automatically invoked deleter function.
   * @note DependencyFactory can be declared friend to indicate
   *       the expected way to invoke an otherwise private ctor.
   *       This is a classical idiom for singletons.
   * @see lib::Depend
   * @see lib::DependInject
   */
  template<class OBJ>
  class DependencyFactory
    : util::NonCopyable
    {
      using Creator = std::function<OBJ*()>;
      using Deleter = std::function<void()>;
      
      Creator creator_;
      Deleter deleter_;
      
    public:
      ZombieCheck zombieCheck{util::typeStr<OBJ>()};
      
      DependencyFactory() = default;
     ~DependencyFactory()
        {
          if (deleter_)
            deleter_();
        }
      
      OBJ*
      buildTarget()
        {
          return creator_? creator_()
                         : buildAndManage();
        }
      
      template<typename FUN>
      void
      defineCreator (FUN&& ctor)
        {
          creator_ = std::forward<FUN> (ctor);
        }
      
      template<typename FUN>
      void
      defineCreatorAndManage (FUN&& ctor)
        {
          creator_ = [this,ctor]
                      {
                        OBJ* obj = ctor();
                        atDestruction ([obj]{ delete obj; });
                        return obj;
                      };
        }
      
      void
      disable()
        {
          creator_ = []() -> OBJ*
                      {
                        throw error::Fatal("Service not available at this point of the Application Lifecycle"
                                          ,LERR_(LIFECYCLE));
                      };
        }
      
      template<typename FUN>
      void
      atDestruction (FUN&& additionalAction)
        {
          if (deleter_)
            {
              Deleter oldDeleter{std::move (deleter_)};
              deleter_ = [oldDeleter, additionalAction]
                          {
                            oldDeleter();
                            additionalAction();
                          };
            }
          else
            deleter_ = std::forward<FUN> (additionalAction);
        }
      
      void
      transferDefinition (DependencyFactory&& source)
        {
          creator_ = std::move (source.creator_);
          deleter_ = std::move (source.deleter_);
          source.creator_ = Creator();
          source.deleter_ = Deleter(); // clear possible leftover deleter
        }
      
    private:
      OBJ*
      buildAndManage()
        {
          OBJ* obj = buildInstance<OBJ>();
          atDestruction ([obj]{ delete obj; });
          return obj;
        }

                                   // try to instantiate the default ctor
      template<class X, typename = decltype(X())>
      static std::true_type  __try_instantiate(int);
      template<class>
      static std::false_type __try_instantiate(...);
      
      /** metafunction: can we instantiate the desired object here?
       * @remark need to perform the check right here in this scope, because
       *  default ctor can be private with DependencyFactory declared friend
       */
      template<typename X>
      struct canDefaultConstruct
        : decltype(__try_instantiate<X>(0))
        { };
      
      
      template<class TAR>
      static     meta::enable_if<canDefaultConstruct<TAR>,
      TAR*                      >
      buildInstance()
        {
          return new TAR;
        }
      
      template<class ABS>
      static     meta::enable_if<std::is_abstract<ABS>,
      ABS*                      >
      buildInstance()
        {
          throw error::Fatal("Attempt to create a singleton instance of an abstract class. "
                             "Application architecture or lifecycle is seriously broken.");
        }
      template<class ABS>
      static     meta::disable_if<std::__or_<std::is_abstract<ABS>,canDefaultConstruct<ABS>>,
      ABS*                       >
      buildInstance()
        {
          throw error::Fatal("Desired singleton class is not default constructible. "
                             "Application architecture or lifecycle is seriously broken.");
        }
    };
  
  
  
  /**
   * @internal access point to reconfigure dependency injection on a per type base
   * @see depend-inject.hpp
   */
  template<class SRV>
  class DependInject;
  
  
  /**
   * Access point to singletons and other kinds of dependencies designated *by type*.
   * Actually this is a Factory object, which is typically placed into a static field
   * of the Singleton (target) class or some otherwise suitable interface.
   * @tparam SRV the class of the Service or Singleton instance
   * @note uses static fields internally, so all factory configuration is shared per type
   * @remarks
   *  - threadsafe lazy instantiation implemented by Double Checked Locking with std::atomic.
   *  - by default, without any explicit configuration, this template creates a singleton.
   *  - a per-type factory function can be configured with the help of lib::DependInject<SRV>
   *  - singletons will be destroyed when the embedded static InstanceHolder is destroyed.
   */
  template<class SRV>
  class Depend
    {
      using Instance = std::atomic<SRV*>;
      using Factory = DependencyFactory<SRV>;
      using Lock = ClassLock<SRV, NonrecursiveLock_NoWait>;
      
      /** shared per type */
      static Instance instance;
      
      static Factory&
      factory() ///< Meyer's Singleton to ensure initialisation on first use
        {
          static Factory sharedFactory;
          return sharedFactory;
        }
      
      friend class DependInject<SRV>;
      
      
    public:
      /**
       * Interface to be used by clients for retrieving the service instance.
       * Manages the instance creation, lifecycle and access in multithreaded context.
       * @return instance of class `SRV`. When used in default configuration,
       *         the returned service instance is a singleton.
       */
      SRV&
      operator() ()
        {
          SRV* object = instance.load (std::memory_order_acquire);
          if (!object)
            {
              factory().zombieCheck();
              Lock guard;
              
              object = instance.load (std::memory_order_relaxed);
              if (!object)
                {
                  object = factory().buildTarget();
                  factory().disable();
                  factory().atDestruction([]{ instance = nullptr; });
                }
              instance.store (object, std::memory_order_release);
            }
          ENSURE (object);
          return *object;
        }
      
      
      /**
       * allow to "peek" if a dependency is already available and exposed.
       * @remark relevant when C code relies on a service with lifecycle.
       */
      explicit
      operator bool()  const
        {
          return instance.load (std::memory_order_acquire);
        }
      
      /** @remark this ctor ensures the factory is created prior to first use,
       *          and stays alive during the whole lifespan of any `Depend<TY>`
       */
      Depend()
        {
          factory().zombieCheck();
        }
    };
  
  
  
  
  /* === allocate Storage for static per type instance management === */
  template<class SRV>
  std::atomic<SRV*> Depend<SRV>::instance;
  
  
  
} // namespace lib
#endif /*LIB_DEPEND_H*/
