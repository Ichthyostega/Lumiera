/*
  Appconfig  -  for global initialisation and configuration 
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
* *****************************************************/


#include "lumiera/appconfig.hpp"
#include "include/error.hpp"
#include "common/util.hpp"


using util::isnil;
using util::cStr;

namespace lumiera {
  


  Symbol ON_BASIC_INIT      ("ON_BASIC_INIT");
  Symbol ON_GLOBAL_INIT     ("ON_GLOBAL_INIT");
  Symbol ON_GLOBAL_SHUTDOWN ("ON_GLOBAL_SHUTDOWN");
  

  /** perform initialisation triggered on first access. 
   *  Will execute the ON_BASIC_INIT hook, but under typical
   *  circumstances this is a NOP, because when callbacks are
   *  added to this hook, the Appconfig singleton instance has
   *  already been created. For this reason, there is special
   *  treatment for the ON_BASIC_INIT in LifecycleHook::add,
   *  causing the provided callbacks to be fired immediately.
   *  (btw, this is nothing to be worried of, because from
   *  client codes POV it just behaves like intended). 
   */
  Appconfig::Appconfig()
    : lifecycleHooks_(new LifecycleRegistry)
  {
    lifecycleHooks_->execute (ON_BASIC_INIT);   // note in most cases a NOP
  }
  
  
  
  
  
  void
  Appconfig::lifecycle (Symbol event_label)
  {
    instance().lifecycleHooks_->execute(event_label);
  }



  
  // ==== implementation LifecycleHook class =======
  
  typedef LifecycleRegistry::Hook Callback;
  
  
  LifecycleHook::LifecycleHook (Symbol eventLabel, Callback callbackFun)
  {
    this->add (eventLabel,callbackFun);
  }
  
  LifecycleHook&
  LifecycleHook::add (Symbol eventLabel, Callback callbackFun)
  {
    bool isNew = Appconfig::instance().lifecycleHooks_->enroll (eventLabel,callbackFun);
    
    if (isNew && !strcmp(ON_BASIC_INIT, eventLabel))
      callbackFun();  // when this code executes,
                     //  then per definition we are already post "basic init"
                    //   (which happens in the Appconfig ctor); thus fire it immediately
    return *this;
  }
  
  
  
} // namespace lumiera

// ==== implementation C interface =======

void 
lumiera_LifecycleHook_add (const char* eventLabel, void callbackFun(void))
{
  lumiera::LifecycleHook (eventLabel, callbackFun);
}


void
lumiera_Lifecycle_execute (const char* eventLabel)
{
  lumiera::Appconfig::lifecycle (eventLabel);
}
