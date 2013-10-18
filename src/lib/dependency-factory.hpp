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

#include "lib/del-stash.hpp"   ////////TODO

namespace lib {
  
  namespace error = lumiera::error;
  
  class AutoDestructor
    {
      typedef void KillFun(void*);
      
      DelStash destructionExecutor_;
      static bool shutdownLock;
      
      static AutoDestructor&
      instance()
        {
          static AutoDestructor _instance_;
          return _instance_;
        }
      
     ~AutoDestructor()
        {
          shutdownLock = true;
        }
       
      static void
      __lifecycleCheck()
        {
          if (shutdownLock)
            throw error::Fatal("Attempt to re-access a service, "
                               "while Application is already in shutdown"
                              ,error::LUMIERA_ERROR_LIFECYCLE);
        }
      
    public:
      static void
      schedule (void* object, KillFun* customDeleter)
        {
          __lifecycleCheck();
          instance().destructionExecutor_.manage (object, customDeleter);
        }
      
      static void
      kill (void* object)
        {
          __lifecycleCheck();
          instance().destructionExecutor_.kill (object);
        }
    };
  bool AutoDestructor::shutdownLock = false;
  
  
  template<typename TAR>
  struct InstanceHolder
    : boost::noncopyable
    {
      
      TAR*
      buildInstance ()
        {
#if NOBUG_MODE_ALPHA
          static uint callCount = 0;
          ASSERT ( 0 == callCount++ );
#endif
          
          // place new instance into embedded buffer
          TAR* newInstance = new(buff_) TAR;
          
          try
            {
              AutoDestructor::schedule (newInstance, &destroy_in_place);
              return newInstance;
            }
          
          catch (std::exception& problem)
            {
              _kill_immediately (newInstance);
              throw error::State (problem, "Failed to install a deleter function "
                                           "for clean-up at application shutdown. ");
            }
          catch (...)
            {
              _kill_immediately (newInstance);
              throw error::State ("Unknown error while installing a deleter function.");
            }
        }
      
    private:
      /** storage for the service instance */
      char buff_[sizeof(TAR)];
      
      
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
    };

  
  /** 
   * Factory to generate and manage service objects classified by type.
   */
  class DependencyFactory
    {
    public:
      /** invoke the installed ctor function */
      void*
      buildInstance()
        {
          REQUIRE (ctorFunction_);
          return ctorFunction_();
        }
      
      void
      deconfigure (void* existingInstance)
        {
          AutoDestructor::kill (existingInstance);
        }
      
      template<class TAR>
      void
      takeOwnership (TAR* newInstance)
        {
          UNIMPLEMENTED("enrol existing instance and prepare deleter function");
        }
      
      template<class TAR>
      void
      restore (TAR* volatile & activeInstance)
        {
          UNIMPLEMENTED("disable and destroy temporary shadowing instance and restore the dormant original instance");
        }
      
      template<class TAR>
      void
      shaddow (TAR* volatile & activeInstance)
        {
          UNIMPLEMENTED("set up a temporary replacement, allowing to restore the original later");
        }
      
      
      typedef void* (*InstanceConstructor)(void);
      
      void
      installConstructorFunction (InstanceConstructor ctor)
        {
          UNIMPLEMENTED("set up constructor function, unless already configured");
        }
      
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
      
    private:
      InstanceConstructor ctorFunction_;
    };
  
  
} // namespace lib
#endif
