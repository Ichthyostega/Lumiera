/*
  Lifecycle  -  registering and triggering lifecycle callbacks

   Copyright (C)
     2008,2017        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file lifecycle.cpp
 ** Implementation of installable callbacks for lifecycle events.
 */


#include "lib/error.hpp"
#include "include/lifecycle.hpp"
#include "lib/lifecycleregistry.hpp"
#include "lib/util.hpp"




namespace vessel {
  
  // ==== implementation Lifecycle Registry =======
  
  /** @remark since the LifecycleRegistry is used to implement
   * the most basic initialisation, we need to ensure it is fully initialised
   * as early as possible. Especially we can not use lib::Depend for that purpose,
   * since the latter needs the LifecycleRegistry to pull up the NoBug-Library.
   * More specifically, if we `#include "lib/depend.hpp"`, on static initialisation
   * the handle planted within that include would invoke `DependencyFactory<LifecycleRegistry>`,
   * which is defined later in the same header and thus not yet initialised.
   */
  LifecycleRegistry&
  LifecycleRegistry::instance()
  {
    static LifecycleRegistry theRegistry;
    return theRegistry;   // Meyer's singleton
  }
  
  
  
  // ==== implementation LifecycleHook class =======
  
  using Callback = LifecycleRegistry::Hook;
  
  
  LifecycleHook::LifecycleHook (Symbol eventLabel, Callback callbackFun)
  {
    add (eventLabel,callbackFun);
  }
  
  void
  LifecycleHook::add (Symbol eventLabel, Callback callbackFun)
  {
    bool isNew = LifecycleRegistry::instance().enrol (eventLabel,callbackFun);
    
    if (isNew and eventLabel == ON_BASIC_INIT)
      callbackFun();  // when this code executes,
                     //  then per definition we are already post "basic init"
  }
  
  
  void
  LifecycleHook::trigger (Symbol eventLabel)
  {
    LifecycleRegistry::instance().execute (eventLabel);
  }
  
  
  
  CStr ON_BASIC_INIT      ("ON_BASIC_INIT");
  CStr ON_GLOBAL_INIT     ("ON_GLOBAL_INIT");
  CStr ON_GLOBAL_SHUTDOWN ("ON_GLOBAL_SHUTDOWN");
  
  CStr ON_EMERGENCY       ("ON_EMERGENCY");
  
  
} // namespace vessel
