/*
  LIFECYCLE.h  -  interface for registering and triggering lifecycle callbacks 

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file lifecycle.h
 ** Installing and invoking of application lifecycle event callbacks.
 ** This service is a facade for and implemented by the lumiera::LifecycleRegistry.
 ** By placing a static LifecycleHook variable or by calling LifecycleHook::add,
 ** a callback can be registered to be executed on a specific application lifecycle
 ** event. Examples being #ON_BASIC_INIT, #ON_GLOBAL_INIT. Other subsystems may
 ** register additional events for more specific purpose.
 ** 
 ** @note the implementation resides in lib/lifecycle.cpp and gets linked into liblumiera.so
 ** @see lumiera::AppState
 ** @see main.cpp
 */


#ifndef LUMIERA_LIFECYCLE_H
#define LUMIERA_LIFECYCLE_H


#ifdef __cplusplus

#include "lib/symbol.hpp"
#include "lib/nocopy.hpp"



namespace lumiera {
  
  using lib::Symbol;
  
  //defined in liblumiera.so
  extern const char * ON_BASIC_INIT;      ///< automatic static init. treated specially to run as soon as possible
  extern const char * ON_GLOBAL_INIT;     ///< to be triggered in main()             @note no magic!
  extern const char * ON_GLOBAL_SHUTDOWN; ///< to be triggered at the end of main()  @note no magic!
  
  extern const char * ON_EMERGENCY;       ///< activated on shutdown after premature failure of a subsystem

  
  // client code is free to register and use additional lifecycle events
  
  
  
  /**
   *  define and register a callback for a specific lifecycle event.
   *  The purpose of this class is to be defined as a static variable in the implementation
   *  of some subsystem (i.e. in the cpp file), providing the ctor with the pointer to a 
   *  callback function. Thus the specified callback gets enrolled when the corresponding
   *  object file is loaded. The event ON_BASIC_INIT is handled specifically, firing off the
   *  referred callback function as soon as possible. All other labels are just arbitrary (string)
   *  constants and it is necessary that "someone" cares to fire off the lifecycle events
   *  at the right place. For example, lumiera-main (and the test runner) calls 
   *  \c LifecycleHook::trigger(ON_GLOBAL_INIT) (and..SHUTDOWN)
   *  @note duplicate or repeated calls with the same callback are NOP 
   */
  class LifecycleHook
    : util::NonCopyable
    {
    public:
      typedef void (*Hook)(void);
      
      LifecycleHook (Symbol eventLabel, Hook callbackFun);
      
      /** alternative, static interface for registering a callback */
      static void add (Symbol eventLabel, Hook callbackFun);
      
      /** trigger lifecycle callbacks registered under the given label */
      static void trigger (Symbol eventLabel);
    };
  
  
  
} // namespace lumiera



#else /* =========== C interface ====================== */


//defined in liblumiera.so
extern const char * lumiera_ON_BASIC_INIT;
extern const char * lumiera_ON_GLOBAL_INIT;
extern const char * lumiera_ON_GLOBAL_SHUTDOWN;


void lumiera_LifecycleHook_add (const char* eventLabel, void callbackFun(void));
void lumiera_Lifecycle_trigger (const char* eventLabel);
  
#endif
#endif
