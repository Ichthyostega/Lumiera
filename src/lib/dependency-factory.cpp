/*
  DependencyFactory  -  managing the lifecycle of singletons and dependencies

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

* *****************************************************/


/** @file dependency-factory.cpp
 ** This compilation unit holds the common backend of all singleton and dependency factories.
 ** Especially the code for tracking and clean-up of all service instance is located here.
 ** This clean-up is triggered by the invocation of \c ~AutoDestructor() -- at this point
 ** a special \c shutdownLock is set, which prevents any further singleton service creation.
 ** 
 */


#include "lib/dependency-factory.hpp"
#include "lib/del-stash.hpp"


namespace lib {
  namespace error = lumiera::error;
  
  
  namespace { // private implementation details...
  
    class AutoDestructor
      {
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
        schedule (void* object, DependencyFactory::KillFun customDeleter)
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
    
  }
  
  
  
  /** explicitly shut down and destroy a service instance.
   *  This can be used to re-start a service; by default, all
   *  services are created on-demand and stay alive until
   *  application shutdown. But a service deconfigured
   *  through this function is destroyed right away. 
   */
  void
  DependencyFactory::deconfigure (void* existingInstance)
  {
    AutoDestructor::kill (existingInstance);
  }
  
  
  /** hook to install a deleter function to clean up a service object.
   *  The standard constructor function uses this hook to schedule the
   *  destructor invocation on application shutdown; custom constructors
   *  are free to use this mechanism (or care for clean-up otherwise)
   * @see lib::DelStash
   */
  void
  DependencyFactory::scheduleDestruction (void* object, KillFun customDeleter)
  {
    AutoDestructor::schedule (object, customDeleter);
  }

  
}// lib
