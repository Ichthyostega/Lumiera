/*
  DEPENDENCY-FACTORY.hpp  -  managing the lifecycle of singletons and dependencies

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

*/




#ifndef LIB_DEPENDENCY_FACTORY_H
#define LIB_DEPENDENCY_FACTORY_H



#include "lib/nobug-init.hpp"
#include "lib/error.hpp"
//#include "lib/sync-classlock.hpp"


namespace lib {
  
  namespace error = lumiera::error;
  
  
  /**
   * Factory to generate and manage service objects classified by type.
   */
  class DependencyFactory
    {
    public:
      typedef void* (*InstanceConstructor)(void);
      typedef void  (*KillFun)            (void*);
      
      /** ensure initialisation by installing a default constructor function,
       *  but don't change an explicitly installed different constructor function.
       * @remark deliberately this DependencyFactory has no constructor to
       *         initialise the object field \c ctorFunction_  to zero.
       *         The reason is, in the intended usage scenario, the
       *         DependencyFactory lives within a static variable,
       *         which might be constructed in no defined order
       *         in relation to the Depend<TY> instance.
       */
      void
      ensureInitialisation (InstanceConstructor defaultCtor)
        {
          if (!ctorFunction_)
            this->ctorFunction_ = defaultCtor;
          ENSURE (ctorFunction_);
        }
      
      
      /** explicitly set up constructor function, unless already configured
       *  In the default configuration, the template \c Depend<TY> installs a
       *  builder function to create a singleton instance in static memory.
       *  But specific instances might install e.g. a factory to create a
       *  implementation defined subclass; this might also be the place
       *  to hook in some kind of centralised service manager in future.
       * @param ctor a function to be invoked to create a new service instance
       * @throw error::Fatal when attempting to change an existing configuration.
       */
      void
      installConstructorFunction (InstanceConstructor ctor)
        {
          if (ctorFunction_ && ctor != ctorFunction_)
            throw error::Fatal ("DependencyFactory: attempt to change the instance builder function "
                                "after-the-fact. Before this call, a different function was installed "
                                "and possibly also used already. Hint: visit all code locations, which "
                                "actually create an instance of the Depend<TY> template."
                               ,error::LUMIERA_ERROR_LIFECYCLE);
          this->ctorFunction_ = ctor;
        }
      
      
      
      /** invoke the installed ctor function */
      void*
      buildInstance()
        {
          REQUIRE (ctorFunction_);
          return ctorFunction_();
        }
      
      void deconfigure (void* existingInstance);
      
      
      template<class TAR>
      void takeOwnership (TAR*);
      
      
      template<class TAR>
      void shaddow (TAR* volatile & activeInstance, TAR* replacement);
      
      template<class TAR>
      void restore (TAR* volatile & activeInstance);
      
      
      
      
      /** hook to install a deleter function to clean up a service object.
       *  The standard constructor function uses this hook to schedule the
       *  destructor invocation on application shutdown; custom constructors
       *  are free to use this mechanism (or care for clean-up otherwise)
       * @see lib::DelStash
       */
      static void scheduleDestruction (void*, KillFun);
      
      
      
    private:
      /** pointer to the concrete function
       *  used for building new service instances */
      InstanceConstructor ctorFunction_;
      
      
      
      template<typename TAR>
      class InstanceHolder
        : boost::noncopyable
        {
          /** storage for the service instance */
          char buff_[sizeof(TAR)];
          
          
          /** deleter function to invoke the destructor
           *  of the embedded service object instance.
           *  A pointer to this deleter function will be
           *  enrolled for execution at application shutdown
           */
          static void
          destroy_in_place (void* pInstance)
            {
              if (!pInstance) return;
              static_cast<TAR*> (pInstance) -> ~TAR();
            }
          
          static void
          _kill_immediately (void* allocatedObject)
            {
              destroy_in_place (allocatedObject);
              const char* errID = lumiera_error();
              WARN (memory, "Failure in DependencyFactory. Error flag was: %s", errID);
            }
          
          
        public:
          TAR*
          buildInstance ()
            {
              // place new instance into embedded buffer
              TAR* newInstance = new(buff_) TAR;
              
              try
                {
                  scheduleDestruction (newInstance, &destroy_in_place);
                  return newInstance;
                }
              
              catch (std::exception& problem)
                {
                  _kill_immediately (newInstance);
                  throw error::State (problem, "Failed to install a deleter function "
                                               "for clean-up at application shutdown.");
                }
              catch (...)
                {
                  _kill_immediately (newInstance);
                  throw error::State ("Unknown error while installing a deleter function.");
                }
            }
        };
      
      
      
      template<class TAR>
      static void*
      createSingletonInstance()
        {
          static InstanceHolder<TAR> storage;
          return storage.buildInstance();
        }
      
      
      template<class TAR>
      friend InstanceConstructor
      buildSingleton()
      {
        return & createSingletonInstance<TAR>;
      }
      
    };
  
  
  namespace {
    /** helper: destroy heap allocated object.
     *  This deleter function is used to clean-up
     *  a heap allocated mock object, which was installed
     *  as a temporary replacement for some service,
     *  typically during an unit test
     */
    template<class X>
    inline void
    releaseOnHeap (void* o)
    {
      if (!o) return;
      X* instance = static_cast<X*> (o);
      delete instance;
    }
  }
  
  
  template<class TAR>
  void
  DependencyFactory::takeOwnership (TAR* newInstance)
  {
    scheduleDestruction (newInstance, &releaseOnHeap<TAR>);
  }
  
  /**
   * set up a temporary replacement, allowing to restore the original later
   * @param activeInstance
   * @param replacement
   */
  template<class TAR>
  void
  DependencyFactory::shaddow (TAR* volatile & activeInstance, TAR* replacement)
  {
    ctorFunction_ = 0;  /////TODO how to implement a functor without explicitly allocated storage??
    activeInstance = replacement;
  }
  
  /**
   * disable and destroy temporary shadowing instance and restore the dormant original instance
   * @param activeInstance
   */
  template<class TAR>
  void
  DependencyFactory::restore (TAR* volatile & activeInstance)
  {
    deconfigure (activeInstance);
    activeInstance = NULL;
  }
  
  
} // namespace lib
#endif
