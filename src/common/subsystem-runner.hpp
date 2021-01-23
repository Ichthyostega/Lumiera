/*
  SUBSYSTEMRUNNER.hpp  -  helper for controlling execution of several dependent subsystems 

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

*/


/** @file subsystem-runner.hpp
 ** Manage execution of the independent [Subsystems](\ref subsys.hpp)
 ** of the Lumiera application. The SubsystemRunner is used by
 ** lumiera::AppState::init() for actually "performing" the various subsystems,
 ** as defined by their [Subsystem descriptors](\ref lumiera::Subsys). Together
 ** these parts define the lifecycle protocol.
 ** 
 ** @todo   as of 2018, a design rework seems indicated eventually                  /////////////////////////TICKET #1177
 ** @remark While this facility is still pretty much the first implementation draft
 **         from 2008, it fulfilled our needs to express simple dependencies and to
 **         conduct a controlled shutdown in case of fatal problems. The operations
 **         protocol implied by this implementation might be somewhat brittle and
 **         creates strong ties to implementation details, like the fine points
 **         of thread handling and locking. If we ever consider to build an
 **         improved subsystem runner, we should care to include the
 **         full set of lifecycle callbacks, similar to a
 **         two phase commit in databases.
 ** 
 ** @see SubsystemRunner_test
 ** 
 */


#ifndef LUMIERA_SUBSYSTEMRUNNER_H
#define LUMIERA_SUBSYSTEMRUNNER_H

#include "lib/error.hpp"
#include "lib/util.hpp"
#include "lib/util-foreach.hpp"
#include "common/subsys.hpp"
#include "lib/sync.hpp"

#include <functional>
#include <vector>
#include <string>


namespace lumiera {

  using std::bind;
  using std::function;
  using std::placeholders::_1;
  using std::vector;
  using std::string;
  using util::cStr;
  using util::isnil;
  using util::and_all;
  using util::for_each;
  using util::removeall;
  using lib::Sync;
  using lib::RecursiveLock_Waitable;
  
  namespace {
    
    /** limit waiting for subsystem shutdown in case of 
     *  an emergency shutdown to max 2 seconds */
    const uint EMERGENCYTIMEOUT = 2000; 
    
    
    function<bool(Subsys*)>
    isRunning() {
      return bind (&Subsys::isRunning, _1);
    }
  }
  
  
  /*************************************************************************//**
   * Implementation helper for managing execution of a collection of subsystems,
   * which may depend on one another and execute in parallel. Properties of the
   * subsystems are available through Subsys object refs, which act as handle.
   * In this context, "Subsystem" is an \em abstraction and doesn't necessarily
   * correspond to a single component, interface or plugin. It may well be a
   * complete layer of the application (e.g. the GUI).
   * 
   * # Protocol of operation
   * The SubsystemRunner is to be configured with a lumiera::Option object first.
   * Then, primary subsystems are \link #maybeRun provided \endlink for eventual
   * startup, which may depend on conditions defined by the subsystem. When
   * a component is actually to be pulled up, all of its prerequisite subsystems
   * shall be started in advance. Problems while starting may result in throwing
   * an exception, which is \em not handled here and aborts the whole operation.
   * On startup, a signal slot is reserved for each subsystem to notify the
   * SubsystemRunner on termination. It is the liability of the subsystems to
   * ensure this signal is activated regardless of what actually causes the 
   * termination; failure to do so may deadlock the SubsystemRunner.
   * 
   * Usually, the startup process is conducted from one (main) thread, which enters
   * a blocking wait() after starting the subsystems. Awakened by some termination
   * signal from one of the subsystems, termination of any remaining subsystems
   * will be triggered. The #wait() function returns after shutdown of all subsystems,
   * signalling an emergency exit (caused by an exception) with its return value.
   * 
   * @todo 2018 this component works well but could be (re)written in a cleaner way
   * 
   * @see lumiera::AppState
   * @see lumiera::Subsys
   * @see main.cpp   
   */
  class SubsystemRunner
    : public Sync<RecursiveLock_Waitable>  
    {
      Option& opts_;
      volatile bool emergency_;
      vector<Subsys*> running_;
      
      function<void(Subsys*)>  start_,
                               stopIt_;
      
      
    public:
      
      SubsystemRunner (Option& opts)
        : opts_(opts)
        , emergency_(false)
        , start_(bind (&SubsystemRunner::triggerStartup, this,_1))
        , stopIt_(bind (&Subsys::triggerShutdown, _1))
        { }
      
      void
      maybeRun (Subsys& susy)
        {
          Lock guard (this);
          
          if (!susy.isRunning() && susy.shouldStart (opts_))
            triggerStartup (&susy);
        }
      
      void
      shutdownAll ()
        {
          Lock guard (this);
          for_each (running_, stopIt_);
        }
      
      void
      triggerEmergency (bool cond)
        { 
          Lock guard (this);
          if (cond) emergency_= true;
        }
      
      bool
      wait ()
        {
          Lock wait_blocking(this, &SubsystemRunner::allDead);
          return isEmergencyExit();
        }
      
      
      
    private:
      bool isEmergencyExit () { return emergency_; }
      
      void
      triggerStartup (Subsys* susy)
        {
          REQUIRE (susy);
          if (susy->isRunning()) return;
          
          INFO (subsystem, "Triggering startup of subsystem \"%s\"", cStr(*susy));
          
          for_each (susy->getPrerequisites(), start_);
          bool started = susy->start (opts_, bind (&SubsystemRunner::sigTerm, this, susy, _1));
          
          if (started)
            {
              if (susy->isRunning())
                running_.push_back (susy); // now responsible for managing the started subsystem
              else
                throw error::Logic("Subsystem "+string(*susy)+" failed to start");
            }
          
          if (not and_all (susy->getPrerequisites(), isRunning() ))
            {
              susy->triggerShutdown();
              throw error::State("Unable to start all prerequisites of Subsystem "+string(*susy));
        }   }
      
      void
      sigTerm (Subsys* susy, string* problem) ///< called from subsystem on termination
        {
          REQUIRE (susy);
          Lock sync (this);
          triggerEmergency(!isnil (problem));
          INFO (subsystem, "Subsystem '%s' terminated.", cStr(*susy));
          WARN_IF (!isnil(problem), subsystem, "Irregular shutdown caused by: %s", cStr(*problem));
          ERROR_IF (susy->isRunning(), subsystem, "Subsystem '%s' signals termination, "
                                                  "without resetting running state", cStr(*susy));
          removeall (running_, susy);
          shutdownAll();
          sync.notify();
        }
      
      bool
      allDead ()
        {
          if (isEmergencyExit())
            {
              Lock sync(this);
              if (!sync.isTimedWait())
                sync.setTimeout(EMERGENCYTIMEOUT);
            }
          
          return isnil (running_);  // end wait if no running subsystem left
        }
      
    };
  
  
  
} // namespace lumiera
#endif
