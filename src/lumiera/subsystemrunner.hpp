/*
  SUBSYSTEMRUNNER.hpp  -  helper for controlling execution of several dependant subsystems 
 
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
 
*/


#ifndef LUMIERA_SUBSYSTEMRUNNER_H
#define LUMIERA_SUBSYSTEMRUNNER_H

#include "include/error.hpp"
#include "lib/util.hpp"
#include "lumiera/subsys.hpp"
#include "lib/multithread.hpp"

#include <tr1/functional>
#include <vector>


namespace lumiera {

  using std::tr1::bind;
  using std::tr1::function;
  using std::tr1::placeholders::_1;
  using std::vector;
  using util::cStr;
  using util::isnil;
  using util::and_all;
  using util::for_each;
  using util::removeall;
  
  namespace {
    function<bool(Subsys*)>
    isRunning() {
      return bind (&Subsys::isRunning, _1);
    }
  }
  
  
  /*****************************************************************************
   * Implementation helper for managing execution of a collection of subsystems,
   * which may depend on one another and execute in parallel. Properties of the
   * subsystems are available through Subsys object refs, which act as handle.
   * In this context, "Subsystem" is an \em abstraction and doesn't necessarily
   * correspond to a single component, interface or plugin. It may well be a
   * complete layer of the application (e.g. the GUI).
   * 
   * \par Protocol of operation
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
   * will be triggered. The wait() function returns after shutdown of all subsystems,
   * signalling an emergency exit (caused by an exception) with its return value.
   * 
   * @todo implement an object monitor primitive based on a mutex and a condition. Inherit from this privately and create local instances of the embedded Lock class to activate the locking and wait/notify
   * @todo maybe use my refArray (see builder) to use Subsys& instead of Subsys* ??
   * 
   * @see lumiera::AppState
   * @see lumiera::Subsys
   * @see main.cpp   
   */
  class SubsystemRunner
    {
      Option& opts_;
      volatile bool emergency_;
      vector<Subsys*> running_;
      
      function<void(Subsys*)>  start_,
                               killIt_;
      
      
    public:
      
      SubsystemRunner (Option& opts)
        : opts_(opts)
        , emergency_(false)
        , start_(bind (&SubsystemRunner::triggerStartup, this,_1))
        , killIt_(bind (&Subsys::triggerShutdown, _1))
        { }
      
      void
      maybeRun (Subsys& susy)
        {
          //Lock guard (*this);
          
          if (!susy.isRunning() && susy.shouldStart (opts_))
            triggerStartup (&susy);
          if (susy.isRunning())
            running_.push_back (&susy);
        }
      
      void
      shutdownAll ()
        {
          //Lock guard (*this);
          for_each (running_, killIt_);
        }
      
      bool
      wait ()
        {
          //Lock(*this).wait (&SubsystemRunner::allDead);
          return isEmergencyExit();
        }
      
      bool isEmergencyExit ()           { return emergency_; }
      void triggerEmergency (bool cond) { emergency_ |= cond; }
      
      
      
    private:
      
      void
      triggerStartup (Subsys* susy)
        {
          ASSERT (susy);
          INFO (operate, "Starting subsystem \"%s\"", cStr(*susy));
          
          for_each (susy->getPrerequisites(), start_);
          bool started = susy->start (opts_, bind (&SubsystemRunner::sigTerm, this, susy, _1));
          
          if (started && !susy->isRunning())
            {
              throw error::Logic("Subsystem "+string(*susy)+" failed to start");
            }
          if (!and_all (susy->getPrerequisites(), isRunning() ))
            {
              susy->triggerShutdown();
              throw error::Logic("Unable to start all prerequisites of Subsystem "+string(*susy));
        }   }
      
      void
      sigTerm (Subsys* susy, Error* problem) ///< called from subsystem on termination
        {
          ASSERT (susy);
          //Lock guard (*this);
          triggerEmergency(problem);
          ERROR_IF (susy->isRunning(), lumiera, "Subsystem '%s' signals termination, "
                                                "without resetting running state", cStr(*susy));
          removeall (running_, susy);
          shutdownAll();
          //guard.notify();
        }
      
      bool
      allDead ()
        {
          if (isEmergencyExit())
            ; //Lock(*this).setTimeout(EMERGENCYTIMEOUT);
          
          return isnil (running_);  // end wait if no running subsystem left
        }
      
    };
  
  
  
} // namespace lumiera
#endif
