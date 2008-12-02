/*
  AppState  -  application initialisation and behaviour 
 
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


#include "include/error.hpp"
#include "include/lifecycle.h"
#include "lumiera/appstate.hpp"
#include "lib/lifecycleregistry.hpp"

extern "C" {
#include "lumiera/config_interface.h"

#include "lumiera/interface.h"
#include "lumiera/interfaceregistry.h"
#include "lumiera/plugin.h"
}

#include "common/util.hpp"
#include "include/configfacade.hpp" //////////TODO: temp hack to force configfacade.o to be linked in


using util::isnil;
using util::cStr;


namespace lumiera {
  
  
  
  Symbol ON_BASIC_INIT      ("ON_BASIC_INIT");
  Symbol ON_GLOBAL_INIT     ("ON_GLOBAL_INIT");
  Symbol ON_GLOBAL_SHUTDOWN ("ON_GLOBAL_SHUTDOWN");
  
  
  /** perform initialisation triggered on first access. 
   *  Will execute the ON_BASIC_INIT hook, but under typical
   *  circumstances this is a NOP, because when callbacks are
   *  added to this hook, the AppState singleton instance has
   *  already been created. For this reason, there is special
   *  treatment for the ON_BASIC_INIT in LifecycleHook::add,
   *  causing the provided callbacks to be fired immediately.
   *  (btw, this is nothing to be worried of, because from
   *  client codes POV it just behaves like intended). 
   */
  AppState::AppState()
    : lifecycleHooks_(new LifecycleRegistry),
      parts_(0)
  {
    lifecycleHooks_->execute (ON_BASIC_INIT);   // note in most cases a NOP
  }
  
  
  AppState::~AppState()  { }
  
  
  
  AppState& 
  AppState::instance()  // Meyer's singleton
  {
    static scoped_ptr<AppState> theApp_ (0);
    if (!theApp_) theApp_.reset (new AppState ());
    return *theApp_;
  }
  
  
  
  void
  AppState::lifecycle (Symbol event_label)
  {
    instance().lifecycleHooks_->execute(event_label);
  }
  
  
  
  
  
  // ==== implementation startup sequence for main() =======
  
  struct SubsystemRunner
    {
      Option& opts_;
      
      SubsystemRunner (Option& opts)
        : opts_(opts)
        { }
      
    };
  
  
#define _THROW_IF \
  if (lumiera_error_peek()) \
    throw error::Fatal (lumiera_error());   
  
  
  
  void
  AppState::init (Option& options)
  {
    TRACE (lumiera, "initialising application core...");
    
    lumiera_interfaceregistry_init ();
    _THROW_IF
    
    TODO ("use a plugindb instead of loading all plugins at once");
    lumiera_plugin_discover (lumiera_plugin_load, lumiera_plugin_register);
    _THROW_IF
    
    lumiera_config_interface_init ();
    _THROW_IF
    
    AppState::lifecycle (ON_GLOBAL_INIT);
    _THROW_IF
    
    
    parts_.reset (new SubsystemRunner (options));
    TRACE (lumiera, "Lumiera core started successfully.");
  }
  
  
  
  void
  AppState::maybeStart (lumiera::Subsys&)
  {
    UNIMPLEMENTED ("maybe register and activate the given subsystem");
  }
  
  
  
  typedef AppState::ExitCode ExitCode;
  
  
  ExitCode
  AppState::maybeWait()
  {
    UNIMPLEMENTED ("wait on termination and normal shutdown");
  }
  
  
  
  ExitCode
  AppState::abort (lumiera::Error& problem)
  {
          
    INFO (operate, "Address of Config Facade = %x", &lumiera::Config::instance());   //////////TODO: a temp hack to force configfacade.cpp to be linked into lumiera.exe
          
    UNIMPLEMENTED ("clean error shutdown");
  }
  
  
  
  ExitCode
  AppState::abort ()
  {
    UNIMPLEMENTED ("emergency shutdown");
  }
  
  
  
  
  
  
  
  
  // ==== implementation LifecycleHook class =======
  
  typedef LifecycleRegistry::Hook Callback;
  
  
  LifecycleHook::LifecycleHook (Symbol eventLabel, Callback callbackFun)
  {
    add (eventLabel,callbackFun);
  }
  
  void
  LifecycleHook::add (Symbol eventLabel, Callback callbackFun)
  {
    bool isNew = AppState::instance().lifecycleHooks_->enroll (eventLabel,callbackFun);
    
    if (isNew && !strcmp(ON_BASIC_INIT, eventLabel))
      callbackFun();  // when this code executes,
                     //  then per definition we are already post "basic init"
                    //   (which happens in the AppState ctor); thus fire it immediately
  }
  
  
  void
  trigger (Symbol eventLabel)
  {
    AppState::lifecycle (eventLabel);
  }
  
  
  
} // namespace lumiera


extern "C" { /* ==== implementation C interface for lifecycle hooks ======= */
  
  
  extern const char * lumiera_ON_BASIC_INIT       = lumiera::ON_BASIC_INIT;
  extern const char * lumiera_ON_GLOBAL_INIT      = lumiera::ON_GLOBAL_INIT;
  extern const char * lumiera_ON_GLOBAL_SHUTDOWN  = lumiera::ON_GLOBAL_SHUTDOWN;
  
  
  
  void 
  lumiera_LifecycleHook_add (const char* eventLabel, void callbackFun(void))
  {
    lumiera::LifecycleHook (eventLabel, callbackFun);
  }
  
  
  void
  lumiera_Lifecycle_trigger (const char* eventLabel)
  {
    lumiera::AppState::lifecycle (eventLabel);
  }
  
}

