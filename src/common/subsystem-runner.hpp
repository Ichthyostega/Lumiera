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
#include "lib/format-string.hpp"
#include "common/subsys.hpp"
#include "lib/sync.hpp"

#include <vector>
#include <string>


namespace lumiera {

  using lib::Sync;
  using lib::RecursiveLock_Waitable;
  using std::chrono_literals::operator ""s;
  using std::vector;
  using std::string;
  using util::_Fmt;
  using util::isnil;
  using util::and_all;
  using util::for_each;
  using util::removeall;
  
  namespace {
    /** limited wait period for unwinding of remaining subsystems
     *  in case of an emergency shutdown, to avoid deadlock */
    const auto EMERGENCY_STOP = 5s;
  }
  
  
  
  /*************************************************************************//**
   * Implementation helper for managing execution of a collection of subsystems,
   * which may depend on one another and execute in parallel. Properties of the
   * subsystems are available through Subsys object refs, which act as handle.
   * In this context, »Subsystem« is an _abstraction_ and doesn't necessarily
   * correspond to a single component, interface or plugin. It may well be a
   * complete layer of the application (e.g. the GUI).
   * 
   * # Protocol of operation
   * The SubsystemRunner is to be configured with a lumiera::Option object first.
   * Then, primary subsystems are [provided](\ref SubsystemRunner::maybeRun) for
   * eventual startup, which may depend on conditions defined by the subsystem.
   * When it turns out (by investigating the options) that a Subsystem is actually
   * to be pulled up, all of its prerequisite subsystems  shall be started beforehand.
   * Problems while starting may result in throwing an exception, which is _not handled_
   * here and aborts the whole operation. On startup, a _callback signal slot_ is reserved
   * for each subsystem to notify the SubsystemRunner on termination. It is the liability
   * of the subsystems to ensure this callback functor is activated reliably, irrespective
   * of what actually causes the termination; failure to do so may deadlock the whole System.
   * 
   * Usually, the startup process is conducted from one (main) thread, which enters
   * the [blocking wait](\ref SubsystemRunner::wait) after starting the subsystems.
   * Awakened by some termination signal from one of the subsystems, termination of any
   * remaining subsystems will be triggered. The #wait() function returns after shutdown
   * of all subsystems, signalling an emergency situation with its return value. In this
   * context, _emergency_ is defined by encountering an top-level exception in any
   * Subsystem, reported by a non-empty error string in the #sigTerm handler.
   * An _emergency_ thus jeopardises the ability to wind-down the all parts
   * of the application reliably.
   * 
   * @todo 2018 this component works well but could be (re)written in a cleaner way  ////////////////////////TICKET #1177
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
      
      bool isEmergency()  { return emergency_; }
      bool allDead(){ return isnil (running_); }
      
      
    public:
      SubsystemRunner (Option& opts)
        : opts_{opts}
        , emergency_{false}
        { }
      
      void
      maybeRun (Subsys& susy)
        {
          Lock guard{this};
          
          if (!susy.isRunning() && susy.shouldStart (opts_))
            triggerStartup (&susy);
        }
      
      void
      shutdownAll()
        {
          Lock guard{this};
          for_each (running_, [](Subsys* susy){ susy->triggerShutdown(); });
        }
      
      void
      triggerEmergency (bool cond)
        {
          Lock guard{this};
          if (cond) emergency_= true;
        }
      
      bool
      wait()
        {
          Lock blocking{this, [&]{ return allDead() or isEmergency(); }};
          if (isEmergency())
            blocking.wait_for (EMERGENCY_STOP, [&]{ return allDead(); });
           // ...prevent deadlock on emergency by limiting shutdown wait
          return isEmergency();
        }
      
      
      
    private:
      void
      triggerStartup (Subsys* susy)
        {
          auto isRunning    = [](Subsys* susy){ return susy->isRunning(); };
          auto triggerStart = [this](Subsys* susy){ triggerStartup(susy); };
          auto termCallback = [this,susy]
                              (string* problem)
                                {
                                  this->sigTerm (susy, problem);
                                };
          REQUIRE (susy);
          if (isRunning(susy)) return;
          
          INFO (subsystem, "Triggering startup of subsystem \"%s\"", cStr(*susy));
          
          for_each (susy->getPrerequisites(), triggerStart );
          bool started = susy->start (opts_, termCallback);
          
          if (started)
            {
              if (isRunning(susy))
                running_.push_back (susy); // now responsible for managing the started subsystem
              else
                throw error::Logic(_Fmt{"Subsystem %s failed to start"} % *susy);
            }
          
          if (not and_all (susy->getPrerequisites(), isRunning ))
            {
              susy->triggerShutdown();
              throw error::State(_Fmt{"Unable to start all prerequisites of Subsystem %s"} % *susy);
        }   }
      
      void
      sigTerm (Subsys* susy, string* problem) ///< called from subsystem on termination
        {
          REQUIRE (susy);
          Lock sync{this};
          triggerEmergency(not isnil (problem));
          INFO (subsystem, "Subsystem '%s' terminated.", cStr(*susy));
          WARN_IF (not isnil(problem), subsystem, "Irregular shutdown caused by: %s", cStr(*problem));
          ERROR_IF (susy->isRunning(), subsystem, "Subsystem '%s' signals termination, "
                                                  "without resetting running state", cStr(*susy));
          removeall (running_, susy);
          shutdownAll();
          sync.notify_one();
        }
    };
  
  
  
} // namespace lumiera
#endif
