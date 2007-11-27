/*
  SINGLETONPOLICIES.hpp  -  how to manage creation, lifecycle and multithreading
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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
    This Loki code accompanies the book:
    Alexandrescu, Andrei. "Modern C++ Design: Generic Programming
        and Design Patterns Applied". 
        Copyright (c) 2001. Addison-Wesley. ISBN 0201704315
 
*/



#ifndef CINELERRA_SINGLETONPOLICIES_H
#define CINELERRA_SINGLETONPOLICIES_H

#include "common/multithread.hpp"
#include "common/error.hpp"

#include <vector>


namespace cinelerra
  {
  namespace singleton
    {
      /* == several Policies usable in conjunction with cinelerra::Singleton == */
      
      /** 
       * Policy placing the Singleton instance into a statically allocated buffer
       */
      template<class S>
      struct StaticCreate
        {
          static S* create ()
            {
#ifdef DEBUG
              static uint callCount = 0;
              ASSERT ( 0 == callCount++ );
#endif              
              static char buff[sizeof(S)];
              return new(buff) S();
            }
          static void destroy (S* pSi)
            {
              pSi-> ~S();
            }
        };
      
      
      /**
       * Policy for creating the Singleton instance heap allocated
       */
      template<class S>
      struct HeapCreate
        {
          static S* create ()         { return new S; }
          static void destroy (S* pS) { delete pS;    }
        };
        
        
      

        
      typedef void (*DelFunc)(void);
      using std::vector;
        
      /**
       * Policy relying on the compiler/runtime system for Singleton Lifecycle
       */
      template<class S>
      struct AutoDestroy
        {
          /** implements the Singleton removal by calling
           *  the provided deleter function(s) at application shutdown,
           *  relying on the runtime system calling destructors of static
           *  objects. Because this Policy class can be shared between 
           *  several Singletons, we need to memoize all registered
           *  deleter functions for calling them at shutdown.
           */ 
          static void scheduleDelete (DelFunc kill_the_singleton) 
            {
                 class DeleteTrigger
                        {
                          vector<DelFunc> dels_;
                          
                        public:
                          void schedule (DelFunc del) 
                            { 
                              dels_.push_back(del); 
                            }
                         ~DeleteTrigger()        
                            { 
                              vector<DelFunc>::iterator i = dels_.begin();
                              for ( ; i != dels_.end(); ++i )
                                (*i)(); // invoke deleter func 
                            }
                        };
                        
              REQUIRE (kill_the_singleton);
              static DeleteTrigger finally;
              finally.schedule (kill_the_singleton);
            }
          
          static void onDeadReference ()
            {
              throw error::Logic ("Trying to access the a Singleton instance that has "
                                  "already been released or finished its lifecycle.");
            }
        };
      
      
      
      /**
       * Policy for handling multithreaded access to the singleton instance
       * @todo actually implement this policy using the cinelerra databackend.
       */
      template<class S>
      struct Multithreaded
        {
          typedef volatile S* VolatileType;
          typedef cinelerra::Thread::Lock<S> Lock;
        };
      
      
      /**
       * Policy just ignoring thread safety
       */
      template<class S>
      struct IgnoreThreadsafety
        {
          typedef S VolatileType;
          struct Lock {};
        };
      
    
  } // namespace singleton
  
} // namespace cinelerra
#endif
