/*
  AppState  -  application initialisation and behaviour 

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


#include "lib/error.hpp"
#include "include/lifecycle.h"
#include "common/appstate.hpp"
#include "common/subsystem-runner.hpp"

extern "C" {
#include "common/config-interface.h"

#include "common/interface.h"
#include "common/interfaceregistry.h"
#include "common/plugin.h"
}

#include "lib/symbol.hpp"
#include "lib/util.hpp"


using util::cStr;
using lib::Literal;
using std::unique_ptr;



namespace lumiera {
  
  namespace { // implementation details
  
    inline void
    log_and_clear_unexpected_errorstate ()
    {
      if (lumiera_err errorstate = lumiera_error ())
        ALERT (common, "*** Unexpected error: %s\n     Triggering emergency exit.", errorstate);
    }
    
    void
    createAppStateInstance(){
      AppState::instance();
    }
    
    LifecycleHook schedule_ (ON_BASIC_INIT, &createAppStateInstance);         
    
  }
  
  
  
  
  /** perform initialisation triggered on first access. 
   *  Will execute BasicSetup sequence to determine the location
   *  of the executable and read in \c setup.ini  --
   *  Since above a LifecycleHook is installed ON_BASIC_INIT,
   *  this can be expected to happen on static initialisation
   *  of this compilation unit, if not earlier (if some other
   *  static initialisation code accesses the instance).
   * @note all further application startup is conducted by \c main.cpp
   */
  AppState::AppState()
    : setup_{LUMIERA_LOCATION_OF_BOOTSTRAP_INI}
    , subsystems_{}
    , emergency_{false}
    , core_up_{false}
  { }
  
  
  
  
  AppState& 
  AppState::instance()  // Meyer's singleton
  {
    static unique_ptr<AppState> theApp_;
    if (!theApp_) theApp_.reset (new AppState ());
    return *theApp_;
  }
  
  
  
  string
  AppState::fetchSetupValue (Literal key)
  {
    return setup_.get(key).as<string>();
  }

  
  
  
  
  // ===== Implementation startup and shutdown sequence for main() ========
  
  
#define _THROW_IF \
  maybeThrow<error::Fatal> ("internal failure while initialising the "\
                            "Lumiera application framework");
  
  
  
  void
  AppState::init (Option& options)
  {
    TRACE (common, "initialising application core...");
    
    lumiera_interfaceregistry_init ();
    _THROW_IF
    
    lumiera_plugin_discover (lumiera_plugin_load, lumiera_plugin_register);
    _THROW_IF
    
    lumiera_config_interface_init ();
    _THROW_IF
    
    core_up_= true;
    LifecycleHook::trigger (ON_GLOBAL_INIT);
    _THROW_IF
    
    
    subsystems_.reset (new SubsystemRunner (options));
    TRACE (common, "Lumiera core started successfully.");
  }
  
  
  
  void
  AppState::maybeStart (lumiera::Subsys& subsys)
  {
    TRACE (common, "maybe startup %s...?", cStr(subsys));
    REQUIRE (subsystems_);
    subsystems_->maybeRun (subsys);
  }
  
  
  
  typedef AppState::ExitCode ExitCode;
  
  
  /** @par
   *  This function is executed at the end of main(), after the necessary subsystems
   *  have been started, typically in separate threads. Thus, the main thread will
   *  enter a blocking wait, until all activated subsystems have signalled shutdown.
   *  After returning, we can proceed with the normal shutdown sequence.
   *  
   *  The SubsystemRunner ensures that in case of a premature failure of one subsystem,
   *  the termination of all other subsystems is initiated; when detecting this case,
   *  the emergency exit sequence is called. Any error which can't be handled within
   *  this scheme, should be thrown as exception, in which case the abort handler
   *  is activated.  
   */
  ExitCode
  AppState::maybeWait()
  {
    if (subsystems_)
      {
        emergency_ |= subsystems_->wait();
        subsystems_.reset(0);
      }
    
    NOTICE (common, "Shutting down Lumiera...");
    
    if (emergency_)
      {
        ALERT (common, "Triggering emergency exit...");
        LifecycleHook::trigger (ON_EMERGENCY);
        return CLEAN_EMERGENCY_EXIT;
      }
    else
      {
        LifecycleHook::trigger (ON_GLOBAL_SHUTDOWN);
        return NORMAL_EXIT;
      }
  }
  
  
  
  ExitCode
  AppState::abort (lumiera::Error& problem)
  {
    
    ERROR (common, "Aborting Lumiera after unhandled error: %s", cStr(problem.what()));
    
    log_and_clear_unexpected_errorstate();
    
    try
      {
        if (subsystems_)
          {
            subsystems_->triggerEmergency(true);
            subsystems_->shutdownAll();
          }
        return maybeWait ();
      }
    catch (...)
      {
        return abort();
      }
  }
  
  
  
  ExitCode
  AppState::abort ()  throw()
  {
    log_and_clear_unexpected_errorstate();
    
    if (emergency_)
      {
        LifecycleHook::trigger (ON_EMERGENCY);
        return FAILED_EMERGENCY_EXIT;
      }
    else
      {
        LifecycleHook::trigger (ON_GLOBAL_SHUTDOWN);
        return CLEAN_EXIT_AFTER_ERROR;
      }
  }
  
  
  
  
  /** anything which should be closed as late as possible and after
   *  the normal shutdown sequence can be placed into the AppState dtor.
   *  But note though, when the application is halted unconditionally,
   *  not dtors will be executed.
   */
  AppState::~AppState()
    {
      if (core_up_)
        try
        {
          TRACE (common, "shutting down basic application layer...");
          lumiera_config_interface_destroy ();
          lumiera_interfaceregistry_destroy ();
        }
    catch (...)
      {
        log_and_clear_unexpected_errorstate();
    } }
  
  
  
  
  
  
} // namespace lumiera
