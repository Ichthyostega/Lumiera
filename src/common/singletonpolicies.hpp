/*
  SINGLETONPOLICIES.hpp  -  how to manage creation, lifecycle and multithreading
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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

#include "common/error.hpp"



namespace cinelerra
  {
  namespace singleton
    {
      /* == several Policies usable in conjunction with cinelerra::Singleton == */
      
      /** 
       * Policy for creating the Singleton instance statically
       */
      template<class S>
      struct Static
        {
          static S* create ()
            {
#ifdef DEBUG
              static uint callCount (0);
              ASSERT ( 0 == callCount++ );
#endif              
              static S _theSingle_;
              return &_theSingle_;
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
      struct Heap
        {
          static S* create ()         { return new S; }
          static void destroy (S* pS) { delete pS;    }
        };
      
      
      /**
       * Policy relying on the compiler/runtime system for Singleton Lifecycle
       */
      template<class S>
      struct Automatic
        {
          typedef void (*DeleterFunc) (void);
          /** implements the Singleton removal by calling
           *  the provided deleter function at application shutdown,
           *  relying on the runtime system calling destructors of
           *  static objects
           */ 
          static void scheduleDelete (DeleterFunc kill_the_singleton) 
            {
                 struct DeleteTrigger
                        {
                         ~DeleteTrigger()              { *del_ (); }
                          DeleteTrigger(DeleterFunc d) : del_(d)  {}
                          DeleterFunc del_;
                        };
              static DeleteTrigger finally(kill_the_singleton);
              
              REQUIRE (kill_the_singleton);
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
          class Lock
            {
            public:
              Lock()  { UNIMPLEMENTED ("aquire Thread Lock"); }
              ~Lock() { UNIMPLEMENTED ("release Thread Lock"); }
            };
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
