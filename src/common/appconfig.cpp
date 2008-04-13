/*
  Appconfig  -  for global initialization and configuration 
 
  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>
 
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


#include "common/appconfig.hpp"
#include "common/error.hpp"
#include "common/util.hpp"

#define NOBUG_INIT_DEFS_
#include "nobugcfg.h"
#undef NOBUG_INIT_DEFS_

#include <exception>


using util::isnil;
using util::cStr;

namespace lumiera
  {
  

#ifndef LUMIERA_VERSION
#define LUMIERA_VERSION 0++devel
#endif


  /** perform initialization triggered on first access. 
   *  Will execute the ON_BASIC_INIT hook, but under typical
   *  circumstances this is a NOP, because when callbacks are
   *  added to this hook, the Appconfig singleton instance has
   *  already been created. For this reason, there is special
   *  treatment for the ON_BASIC_INIT in LifecycleHook::add,
   *  causing the provided callbacks being fired immediately.
   *  (btw, this is nothing to be worried of, for the client
   *  code it just behaves like intended). 
   */
  Appconfig::Appconfig()
    : configParam_  (new Configmap),
      lifecycleHooks_(new LifecycleRegistry)
  {
    ////////// 
    NOBUG_INIT;
    //////////
    
    INFO(config, "Basic application configuration triggered.");
    lifecycleHooks_->execute (ON_BASIC_INIT);   // note in most cases a NOP
    
    // install our own handler for undeclared exceptions
    std::set_unexpected (lumiera::error::lumiera_unexpectedException);
    
    (*configParam_)["version"] = STRINGIFY (LUMIERA_VERSION);
  }
  
  
  
  
  
  const string &
  Appconfig::get (const string & key)
  {
    try
      {
        const string& val = (*instance().configParam_)[key];
        WARN_IF( isnil(val), config, "undefined config parameter \"%s\" requested.", key.c_str());
        return val;
      }
    catch (...)
      {
        ERROR(config, "error while accessing configuration parameter \"%s\".", key.c_str());
        static string NOTFOUND ("");
        return NOTFOUND;
  }   }

  
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
    Appconfig::instance().lifecycleHooks_->enroll (eventLabel,callbackFun);
    
    if (!strcmp(ON_BASIC_INIT, eventLabel))
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


const char*
lumiera_Appconfig_get (const char* key)
{
  return cStr (lumiera::Appconfig::get(key));
}
