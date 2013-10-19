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
    
    
    
    class TemporaryShadowedInstanceFactory
      : boost::noncopyable
      {
        typedef DependencyFactory::InstanceConstructor Ctor;
        
        void* originalInstance_;
        Ctor  originalCtor_;
        
      public:
        TemporaryShadowedInstanceFactory(void* instance, Ctor ctor)
          : originalInstance_(instance)
          , originalCtor_(ctor)
          { }
        
        void*
        constructorInvocation()
          {
            REQUIRE (originalCtor_);
            
            if (!originalInstance_)
              return originalCtor_();
            
            void* existingInstance = originalInstance_;
            originalInstance_ = NULL;
            return existingInstance;
          }
      };
      
  }
  
  
  
  /** */
  
  
  /** */
  void
  DependencyFactory::deconfigure (void* existingInstance)
  {
    AutoDestructor::kill (existingInstance);
  }
  
  void
  DependencyFactory::scheduleDestruction (void* object, KillFun customDeleter)
  {
    AutoDestructor::schedule (object, customDeleter);
  }

  
}// lib
