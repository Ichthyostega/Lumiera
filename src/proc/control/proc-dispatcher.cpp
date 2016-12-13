/*
  ProcDispatcher  -  Proc-Layer command dispatch and execution

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
#include "include/logging.h"
#include "proc/control/proc-dispatcher.hpp"
#include "proc/mobject/session.hpp"
#include "backend/thread-wrapper.hpp"
//#include "proc/mobject/mobject-ref.hpp"
//#include "proc/mobject/mobject.hpp"
//#include "proc/mobject/placement.hpp"

//#include <boost/format.hpp>
//using boost::str;
  
using backend::ThreadJoinable;

namespace proc {
namespace control {
  
  class DispatcherLoop
    : ThreadJoinable
    {
    public:
      DispatcherLoop (Subsys::SigTerm notification)
        : ThreadJoinable("Lumiera Session"
                        , bind (&DispatcherLoop::run, this, notification))
        {
          INFO (session, "Proc-Dispatcher running...");
        }
      
     ~DispatcherLoop()
        {
          this->join();
          INFO (session, "Proc-Dispatcher stopped.");
        }
    
    private:
      void
      run (Subsys::SigTerm sigTerm)
        {
          string errorMsg;
          try
            {
              TODO ("actually do something in the loop");
              sleep(2);
            }
          catch (lumiera::Error& problem)
            {
              errorMsg = problem.what();
              lumiera_error();        // clear error flag
            }
          catch (...)
            {
              errorMsg = string{lumiera_error()};
            }
          sigTerm (&errorMsg);
        }
      
    };
  
  namespace {
  
    /** when session gets ready,
     *  open the command processing interface.
     */
    void
    activateCommandProcessing()
    {
      ProcDispatcher::instance().activate();
    }
    
    void
    deactivateCommandProcessing()
    {
      ProcDispatcher::instance().deactivate();
    }
    
    
    lumiera::LifecycleHook _schedule_opening (mobject::ON_SESSION_INIT, &activateCommandProcessing);
    lumiera::LifecycleHook _schedule_closing (mobject::ON_SESSION_END,  &deactivateCommandProcessing);
  
  }
  
  /** storage for Singleton access */
  lib::Depend<ProcDispatcher> ProcDispatcher::instance;
  
  
  
  
  
  /** */
  bool
  ProcDispatcher::start (Subsys::SigTerm termNotification)
  {
    if (runningLoop_) return false;
    
    runningLoop_.reset (
      new DispatcherLoop (
            [=] (string* problemMessage)
                {
                  runningLoop_.reset();
                  termNotification(problemMessage);
                }));

    return true;
  }
  
  
  /** */
  bool
  ProcDispatcher::isRunning()
  {
    return bool(runningLoop_);
  }
  
  
  /** */
  void
  ProcDispatcher::requestStop()
  {
    UNIMPLEMENTED ("trigger shutdown into the dispacher loop thread");
  }
  
  
  /** */
  void
  ProcDispatcher::activate()
  {
    INFO (command, "Session command processing activated.");
    TODO ("implement command processing queue");
  }
  
  
  void
  ProcDispatcher::deactivate()
  {
    INFO (command, "Session command interface closed.");
    TODO ("implement command processing queue");
  }

  
  void
  ProcDispatcher::clear()
  {
    if (!empty())
      WARN (command, "DISCARDING pending Session commands.");
    TODO ("implement command processing queue");
  }
  
  
  bool
  ProcDispatcher::empty()  const
  {
    TODO ("implement command processing queue");
    return true;
  }

  
  
  
  
  
}} // namespace proc::control
