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
using lib::Sync;
using lib::RecursiveLock_Waitable;

namespace proc {
namespace control {
  
  class DispatcherLoop
    : ThreadJoinable
    , public Sync<RecursiveLock_Waitable>
    {
      bool canDispatch_{false};
      bool blocked_    {false};
      
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
      
      void
      activateCommandProecssing()
        {
          Lock sync(this);
          canDispatch_ = true;
          INFO (command, "Session command processing activated.");
          TODO ("implement command processing queue");
        }
      
      void
      deactivateCommandProecssing()
        {
          Lock sync(this);
          canDispatch_ = false;
          INFO (command, "Session command interface closed.");
          TODO ("implement command processing queue");
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
  
  
  
  /** storage for Singleton access */
  lib::Depend<ProcDispatcher> ProcDispatcher::instance;
  
  
  
  
  
  /** */
  bool
  ProcDispatcher::start (Subsys::SigTerm termNotification)
  {
    Lock sync(this);
    if (runningLoop_) return false;
    
    runningLoop_.reset (
      new DispatcherLoop (
            [=] (string* problemMessage)
                {
                  runningLoop_.reset();
                  termNotification(problemMessage);
                }));
    
    if (active_)
      runningLoop_->activateCommandProecssing();
    return true;
  }
  
  
  /** */
  bool
  ProcDispatcher::isRunning()
  {
    Lock sync(this);
    return bool(runningLoop_);
  }
  
  
  /** */
  void
  ProcDispatcher::requestStop()
  {
    Lock sync(this);
    UNIMPLEMENTED ("trigger shutdown into the dispacher loop thread");
  }
  
  
  /** activate processing of enqueued session commands.
   * @remarks command processing serves as public external interface
   *  to the session. This call is used by the session lifecycle (SessManagerImpl)
   *  when the session is brought up; any other invocation runs danger to mess up
   *  the session lifecycle state and process commands on a deconfigured session.
   *  In case the dispatcher loop is not actually running, the activation state
   *  is stored and applied accordingly later, when the loop is fired up.
   */
  void
  ProcDispatcher::activate()
  {
    Lock sync(this);
    active_ = true;
    if (runningLoop_)
      runningLoop_->activateCommandProecssing();
  }
  
  
  void
  ProcDispatcher::deactivate()
  {
    Lock sync(this);
    active_ = false;
    if (runningLoop_)
      runningLoop_->deactivateCommandProecssing();
  }

  
  void
  ProcDispatcher::clear()
  {
    Lock sync(this);
    if (not empty())
      WARN (command, "DISCARDING pending Session commands.");
    TODO ("implement command processing queue");
  }
  
  
  bool
  ProcDispatcher::empty()  const
  {
    Lock sync(this);
    TODO ("implement command processing queue");
    return true;
  }

  
  
  
  
  
}} // namespace proc::control
