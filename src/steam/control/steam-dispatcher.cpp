/*
  SteamDispatcher  -  Steam-Layer command dispatch and execution

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file steam-dispatcher.cpp
 ** Implementation details of running commands and the builder.
 ** The SteamDispatcher is at the heart of the session subsystem and implements a
 ** (single) session thread to perform commands and trigger builder runs. New commands
 ** can be enqueued with a dedicated CommandQueue, while the details of operation control
 ** logic are encapsulated in a [processing logic component](\ref Looper).
 ** 
 ** # Operational Semantics
 ** We need to distinguish between the SteamDispatcher itself, which is a static (singleton) service,
 ** and the »Session Subsystem« plus the _Session proper._ The subsystem has an application-global lifecycle,
 ** while the Session itself is a data structure and can be closed, opened or re-loaded. There is a singular
 ** transactional access point to the Session datastructure, which can be switched to new session contents.
 ** But external manipulation of the session contents is performed by commands, which are _dispatched_ --
 ** and the management of this process is the concern served by the »Session Subsystem«.
 ** 
 ** Closing a session blocks further command processing, while the lifecycle of the _Session Subsystem_ is
 ** actually linked to _running the \ref DispatcherLoop_ -- a piece of implementation logic defined within this
 ** translation unit. The loop implementation is performed within a dedicated thread, _the Session Loop Thread._
 ** And this also entails opening the public SessionCommandService interface.
 ** 
 ** ## Loop operation control
 ** The loop starts with a blocking wait state, bound to the condition Looper::requireAction. The Looper
 ** is a helper to encapsulate the control logic, separated from the actual control flow. In the loop body,
 ** depending on the Looper's decision, either the next command is fetched from the CommandQueue and dispatched,
 ** or a builder run is triggered, rebuilding the »Low-Level-Model« to reflect the executed command's effects.
 ** After these working actions, a _"check point"_ is reached in Looper::markStateProcessed, which updates
 ** the logic and manages a _dirty state_ to control builder runs. After that, the looping control flow
 ** again enters the possibly blocking condition wait.
 ** - after a command has been dispatched, the builder is _dirty_ and needs to run
 ** - yet we continue to dispatch further commands, until the queue is emptied
 ** - and only after a further small latency wait, the builder run is triggered
 ** - but we _enforce a builder run_ after some extended timeout period, even
 **   when the command queue is not emptied yet
 ** - from the outside, it is possible to deactivate processing and place the
 **   loop into dormant state. This is used while closing or loading the Session
 ** - and of course we can request the Session Loop Thread to stop, for shutting
 **   down the »Session Subsystem« as a whole
 ** - in both cases the currently performed action (command or builder) is
 **   finished, without interruption.
 ** 
 ** ## Locking
 ** The SteamDispatcher uses an "inner and outer capsule" design, and both layers are locked independently.
 ** On the outer layer, locking ensures sanity of the control data structures, while locking on the inner
 ** layer guards the communication with the Session Loop Thread, and coordinates sleep wait and notification.
 ** As usual with Lumiera's Thread wrapper, the management of the thread's lifecycle itself, hand-over of
 ** parameters, and starting / joining of the thread operation is protected by means of synchronisation
 ** embedded into the underlying implementation (C++ standard library thread support).
 ** @note most of the time, the Session Loop Thread does not hold any lock, most notably while performing
 **       a command or running the builder. Likewise, evaluation of the control logic in the Looper helper
 **       is a private detail of the performing thread. The lock is acquired solely for checking or leaving
 **       the wait state and when fetching the next command from queue.
 ** 
 ** @see SteamDispatcher
 ** @see DispatcherLooper_test
 ** @see CommandQueue_test
 **
 */


#include "lib/error.hpp"
#include "include/logging.h"
#include "steam/control/steam-dispatcher.hpp"
#include "steam/control/command-dispatch.hpp"
#include "steam/control/command-queue.hpp"
#include "steam/control/looper.hpp"
#include "steam/control/session-command-service.hpp"
#include "steam/mobject/session.hpp"
#include "lib/depend-inject.hpp"
#include "lib/sync-barrier.hpp"
#include "lib/thread.hpp"
#include "lib/util.hpp"                ///////////////TODO for test command invocation

#include <utility>
#include <memory>
  
using lib::Sync;
using lib::SyncBarrier;
using lib::ThreadHookable;
using lib::RecursiveLock_Waitable;
using std::make_unique;
using std::move;

namespace steam {
namespace control {
  
  namespace error = lumiera::error;
  
  
  /********************************************************************//**
   * PImpl within SteamDispatcher to implement the _Session Loop Thread._
   * During the lifetime of this object...
   * - the SessionCommandService is offered to enqueue commands
   * - the Session Loop thread dispatches commands and triggers the Builder
   * @see DispatcherLooper_test
   */
  class DispatcherLoop
    : public CommandDispatch
    , public Sync<RecursiveLock_Waitable>
    {
      using ServiceHandle = lib::DependInject<SessionCommandService>::ServiceInstance<>;
      using Launch = lib::ThreadHookable::Launch;
      
      /** manage the primary public Session interface */
      ServiceHandle commandService_;

      SyncBarrier      init_;
      CommandQueue    queue_;
      string          error_;
      Looper         looper_;
      ThreadHookable thread_;
      
    public:
      /** start the session loop thread
       * @param notification callback to invoke on thread termination
       * @remark _in theory_ this ctor could block, since it waits for the thread
       *         actually to get operational and it waits for the SessionCommand interface
       *         to be opened. The latter _better should not_ run into any obstacles, because
       *         in case it does, the main application thread will be deadlocked on startup.
       *         Such might happen indirectly, when something depends on "the Session"
       */
      template<typename FUN>
      DispatcherLoop (FUN&& atExit)
        : commandService_{ServiceHandle::NOT_YET_STARTED}
        , queue_{}
        , error_{}
        , looper_([&]() -> bool
                    {
                      return not queue_.empty();
                    })
        , thread_{ //----the-Session-Thread---------------
                  Launch{&DispatcherLoop::runSessionThread, this}
                        .threadID("Session")
                        .atExit([this, signalEndOfThread = move(atExit)]
                                (lib::thread::ThreadWrapper& handle)
                                  {
                                    handle.detach_thread_from_wrapper();
                                    signalEndOfThread (&error_);
                                  })}
        {
          init_.sync();   // done with setup; loop may run now....
          INFO (session, "Steam-Dispatcher running...");
            {
              Lock sync{this}; // open public session interface:
              commandService_.createInstance(*this);
            }
        }
      
     ~DispatcherLoop()
        {
          try {
              commandService_.shutdown();  // redundant call, to ensure session interface is closed reliably
              INFO (session, "Steam-Dispatcher stopped.");
              if (thread_)
                ALERT (session, "Dispatcher destroyed while Session thread is running. The rest is silence.");
            }
          ERROR_LOG_AND_IGNORE(session, "Stopping the Steam-Dispatcher");
        }
      
      void
      activateCommandProecssing()
        {
          Lock sync{this};
          looper_.enableProcessing(true);
          INFO (command, "Session command processing activated.");
          sync.notify_all();
        }
      
      void
      deactivateCommandProecssing()
        {
          Lock sync{this};
          looper_.enableProcessing(false);
          INFO (command, "Session command interface closed.");
          sync.notify_all();
        }
      
      void
      requestStop()  noexcept
        {
          Lock sync{this};
          commandService_.shutdown(); // closes Session interface
          looper_.triggerShutdown();
          sync.notify_all();
        }
      
      void
      awaitStateProcessed()  const
        {
          Lock{this, [&]{ return isStateSynched(); }};
           //  wake-up typically by updateState()
        }
      
      size_t
      size()  const
        {
          Lock sync{this};
          return queue_.size();
        }
      
      
      /* === CommandDispatch interface === */
      
      void
      enqueue (Command&& cmd)  override
        {
          Lock sync{this};
          queue_.feed (move (cmd));
          sync.notify_all();
        }
      
      void
      clear()  override
        {
          Lock sync{this};
          queue_.clear();
          sync.notify_all();
        }
      
    private:
      /**
       * any operation running in the Session thread is
       * started from here. When this loop terminates,
       * the »session subsystem« shuts down.
       * @note the callback \a notifyEnd is typically bound
       *    to invoke SteamDispatcher::endRunningLoopState().
       */
      void
      runSessionThread ()
        {
          init_.sync();
          try
            {
              while (looper_.shallLoop())
                {
                  awaitAction();
                  if (looper_.isDying())
                    break;
                  if (looper_.runBuild())
                    startBuilder();
                  else
                    if (looper_.isWorking())
                      processCommands();
                  updateState();
                }
            }
          catch (std::exception& problem)
            {  // could also be lumiera::Error
              error_ = problem.what();
              lumiera_error();        // clear error flag
            }
          catch (...)
            {
              error_ = string{lumiera_error()};
            }
           // Session thread terminates...
        } //  atExit-λ will invoke ~DispatcherLoop()
      
      void
      awaitAction()   ///< at begin of loop body...
        {
          Lock{this}.wait_for (looper_.getTimeout()
                              ,[&]{ return looper_.requireAction(); });
        }
      
      void
      updateState()   ///< at end of loop body...
        {
          looper_.markStateProcessed();
          if (looper_.isDisabled())     // otherwise wake-up would not be safe
            getMonitor(this).notify_all();
        }
      
      bool
      isStateSynched()  const
        {
          if (thread_.invokedWithinThread())
            throw error::Fatal("Possible Deadlock. "
                               "Attempt to synchronise to a command processing check point "
                               "from within the (single) session thread."
                              , error::LUMIERA_ERROR_LIFECYCLE);
          return not looper_.hasPendingChanges();
        }
      
      void
      processCommands()
        {
          Command cmd;
            {
              Lock sync{this};
              if (not queue_.empty())
                cmd = queue_.pop();
            }
          if (cmd)
            {
              INFO (command, "+++ dispatch %s", cStr(cmd));          ////////////////////////////////////////TICKET #211 actually use a command logging and execution strategy here
              
              //////////////////////////////////////////////////////TODO : magic to invoke commands from unit tests
              if (util::startsWith (string(cmd.getID()), "test"))
                {
                  INFO (command, "+++ -------->>> bang!");
                  auto resultState = cmd();
                  resultState.maybeThrow();
                }
              //////////////////////////////////////////////////////TODO : magic to invoke commands from unit tests
            }
        }
      
      void
      startBuilder()
        {
          INFO (builder, "+++ start the Steam-Builder...");
        }
    };
  
  
  
  
  
  
  /** storage for Singleton access */
  lib::Depend<SteamDispatcher> SteamDispatcher::instance;
  
  
  /* ======== SteamDispatcher implementation ======== */
  
  SteamDispatcher::SteamDispatcher()  { }
  SteamDispatcher::~SteamDispatcher() { }
  
  /** starting the SteamDispatcher means to start the session subsystem.
   * @return `false` when _starting_ failed since it is already running...
   * @remark this function implements the start operation for the »session subsystem«.
   *         More specifically, this operation starts a new thread to perform the
   *         _session loop,_ which means to perform commands and trigger the builder.
   *         It might block temporarily for synchronisation with this new thread and
   *         while opening the SessionCommand facade.
   */
  bool
  SteamDispatcher::start (Subsys::SigTerm termNotification)
  {
    Lock sync{this};
    if (runningLoop_) return false;
    
    runningLoop_ =
         make_unique<DispatcherLoop>(
            [=](string* problemIndicator)
                { // when the Session thread ends....
                  SteamDispatcher::endRunningLoopState();
                  termNotification (problemIndicator);
                });
    
    if (active_)
      runningLoop_->activateCommandProecssing();
    return true;
  }
  
  /** @internal clean-up when leaving the session loop thread.
   *  This function is hooked up in to the termination callback,
   *  and is in fact the only one to delete the loop PImpl. We
   *  take the (outer) lock on SteamDispatcher to ensure no one
   *  commits anything to the DispatcherLoop object while being
   *  deleted. The call itself, while technically originating
   *  from within DispatcherLoop::runSessionThread(), relies
   *  solely on stack based context data and is a tail call.
   */
  void
  SteamDispatcher::endRunningLoopState()
  {
    Lock sync{this};
    if (runningLoop_)
      runningLoop_.reset();  // delete DispatcherLoop object
    else
      WARN (command, "clean-up of DispatcherLoop invoked, "
                     "while SteamDispatcher is not marked as 'running'. "
                     "Likely an error in lifecycle logic, as the only one "
                     "intended to delete this object is the loop thread itself.");
  }
  
  
  /** whether the »session subsystem« is operational.
   * @return `true` if the session loop thread has been fully
   *         started and is not (yet) completely terminated.
   */
  bool
  SteamDispatcher::isRunning()
  {
    Lock sync{this};
    return bool(runningLoop_);
  }
  
  
  /** signal to the loop thread that it needs to terminate.
   * @note the immediate consequence is to close SessionCommandService
   */
  void
  SteamDispatcher::requestStop()  noexcept
  {
    try {
      Lock sync{this};
      if (runningLoop_)
        runningLoop_->requestStop();
    }
    ERROR_LOG_AND_IGNORE (command, "Request for Session Loop Thread to terminate");
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
  SteamDispatcher::activate()
  {
    Lock sync{this};
    active_ = true;
    if (runningLoop_)
      runningLoop_->activateCommandProecssing();
  }
  
  
  /** halt further processing of session commands
   * @note the processing itself runs in a separate thread, thus any currently
   *  ongoing command or builder execution will be completed prior to this setting
   *  to take effect. If the intention is to halt processing because the session is
   *  about to dismantled, it is mandatory to awaitDeactivation()
   */
  void
  SteamDispatcher::deactivate()
  {
    Lock sync{this};
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
  SteamDispatcher::awaitDeactivation()
  {
    Lock sync{this};
    if (runningLoop_)
      runningLoop_->awaitStateProcessed();
  }
  
  
  /** discard any commands waiting in the dispatcher queue */
  void
  SteamDispatcher::clear()
  {
    Lock sync{this};
    if (not empty())
      {
        WARN (command, "DISCARDING pending Session commands.");
        REQUIRE (runningLoop_);
        runningLoop_->clear();
      }
  }
  
  
  bool
  SteamDispatcher::empty()  const
  {
    Lock sync{this};
    return not runningLoop_
        or 0 == runningLoop_->size();
  }
  
  
  
  
}} // namespace steam::control
