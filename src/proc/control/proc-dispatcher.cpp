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


/** @file proc-dispatcher.cpp
 ** Implementation details of running commands and the builder.
 ** The ProcDispatcher is at the heart of the session subsystem and implements
 ** a (single) session thread to perform commands and trigger builder runs.
 ** New commands can be enqueued with a dedicated CommandQueue, while the details
 ** of operation control logic are encapsulated in a [logic component](\ref Looper).
 ** 
 ** @todo as of 12/2016, implementation has been drafted and is very much WIP
 ** @todo                         //////////////////////////////////////////////////////TODO ensure really every state change triggers a wakeup!!!!!!!
 ** 
 ** @see ProcDispatcher
 ** @see DispatcherLooper_test
 ** @see CommandQueue_test
 **
 */


#include "lib/error.hpp"
#include "include/logging.h"
#include "proc/control/proc-dispatcher.hpp"
#include "proc/control/command-dispatch.hpp"
#include "proc/control/command-queue.hpp"
#include "proc/control/looper.hpp"
#include "proc/control/session-command-service.hpp"
#include "proc/mobject/session.hpp"
#include "backend/thread-wrapper.hpp"
//#include "proc/mobject/mobject-ref.hpp"
//#include "proc/mobject/mobject.hpp"
//#include "proc/mobject/placement.hpp"

#include <memory>
//using boost::str;
  
using backend::ThreadJoinable;
using lib::Sync;
using lib::RecursiveLock_Waitable;
using std::unique_ptr;

namespace proc {
namespace control {
  
  namespace error = lumiera::error;
  
  class DispatcherLoop
    : ThreadJoinable
    , public CommandDispatch
    , public Sync<RecursiveLock_Waitable>
    {
      bool canDispatch_{false};
      bool blocked_    {false};
      bool mustHalt_   {false};  /////////////////TODO this flag shall be relocated into the Looper!
      
      unique_ptr<SessionCommandService> commandService_;
      
      CommandQueue queue_;
      Looper      looper_;
      
      
    public:
      DispatcherLoop (Subsys::SigTerm notification)
        : ThreadJoinable("Lumiera Session"
                        , bind (&DispatcherLoop::run, this, notification))
        , commandService_(new SessionCommandService(*this))
        , queue_()
        , looper_([&]() -> bool
                    {
                      return not queue_.empty();
                    })
        {
          INFO (session, "Proc-Dispatcher running...");
        }
      
     ~DispatcherLoop()
        {
          try {
              Lock sync(this);
              commandService_.reset();   // redundant call, ensure the session interface is reliably closed
              this->join();              // block until the loop thread terminates and is reaped
              INFO (session, "Proc-Dispatcher stopped.");
            }
          ERROR_LOG_AND_IGNORE(session, "Stopping the Proc-Dispatcher");
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
      
      
      /* === CommandDispatch interface === */
          //////////////////////////////////////////TODO notify!!!! on!! every!! state!! changing!! operation!!
      
      void
      clear()  override
        {
          Lock sync(this);
          UNIMPLEMENTED ("clear the queue");
          //////////////////////////////////////////TODO notify!!!!
        }
      
      size_t
      size()  const
        {
          TODO ("implement command processing queue");
          return 0;
        }
      
      void
      requestStop()  noexcept
        {
          Lock sync(this);
          commandService_.reset(); // closes Session interface
          mustHalt_ = true;
          UNIMPLEMENTED("*must* notify loop thread");  /////////////////TODO really?  YES!!!
          //////////////////////////////////////////TODO notify!!!!
        }
      
      void
      awaitCheckpoint()
        {
          Lock blockWaiting(this, &DispatcherLoop::stateIsSynched);
                                            //////////////////////////////////////////TODO find out who will notify us!!!!
        }
      
    private:
      /** the actual loop running in the Session thread */
      void
      run (Subsys::SigTerm sigTerm)
        {
          string errorMsg;
          try
            {
              while (looper_.shallLoop())
                {
                  awaitAction();
                  if (looper_.isDying()) break;
                  if (looper_.runBuild())
                    startBuilder();
                  else
                  if (looper_.isWorking())
                    processCommands();
                  looper_.markStateProcessed();
                }
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
          // now leave the Session thread...
          // send notification of subsystem shutdown
          sigTerm (&errorMsg);
        }
      
      void
      awaitAction()
        {
          Lock(this).wait(looper_, &Looper::requireAction,
                          looper_.getTimeout());
        }
      
      bool
      stateIsSynched()
        {
          if (looper_.hasPendingChanges() and calledFromWithinSessionThread())
            throw error::Fatal("Possible Deadlock. "
                               "Attempt to synchronise to a command processing check point "
                               "from within the (single) session thread."
                              , error::LUMIERA_ERROR_LIFECYCLE);
          return not looper_.hasPendingChanges();
        }
      
      void
      processCommands()
        {
          UNIMPLEMENTED ("pull commands from the queue and dispatch them");
        }
      
      void
      startBuilder()
        {
          UNIMPLEMENTED ("start the Proc-Builder to recalculate render nodes network");
        }
      
      bool
      calledFromWithinSessionThread()
        {
          UNIMPLEMENTED ("how to find out when the session thread attempts to catch its own tail...???");
          ////////////////////////////////////////////////////////////////TODO any idea how to achieve that? The lock does not help us, since it is recursive and
          //////////////////////////////////////////////////////////////// ... since command/builder execution itself is not performed in a locked section.
          //////////////////////////////////////////////////////////////// ... Possibly we'll just have to plant a ThreadLocal to mark this dangerous situation.
          
          ///////////////////////////////////////////////////////////////////////////////TICKET #1054 : can be done by relying on some internals of our thread handling framework
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
  
  
  /** signal to the loop thread that it needs to terminate.
   * @warning dangerous operation; must not block nor throw
   * 
   * @todo need to re-check the logic, once the loop is fully implemented; ensure there is nothing on this call path that can block or throw!!! 
   */
  void
  ProcDispatcher::requestStop()  noexcept
  {
    Lock sync(this);
    if (runningLoop_)
      runningLoop_->requestStop();
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
  
  
  /** block until the dispatcher has actually reached disabled state.
   * @warning beware of invoking this function from within the session thread,
   *        since the waiting relies on the very lock also used to coordinate
   *        command processing and builder runs within that thread.
   * @throw error::Fatal when a deadlock due to such a recursive call can be detected
   */
  void
  ProcDispatcher::awaitDeactivation()
  {
    Lock sync(this);
    if (runningLoop_)
      runningLoop_->awaitCheckpoint();
  }

  
  void
  ProcDispatcher::clear()
  {
    Lock sync(this);
    if (not empty())
      {
        WARN (command, "DISCARDING pending Session commands.");
        REQUIRE (runningLoop_);
        runningLoop_->clear();
      }
  }
  
  
  bool
  ProcDispatcher::empty()  const
  {
    Lock sync(this);
    return not runningLoop_
        or 0 == runningLoop_->size();
  }

  
  
  
  
  
}} // namespace proc::control
