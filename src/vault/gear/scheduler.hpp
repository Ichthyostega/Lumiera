/*
  SCHEDULER.hpp  -  coordination of render activities under timing and dependency constraints

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file scheduler.hpp
 ** Service for coordination and dispatch of render activities.
 ** The implementation of scheduling services is provided by an integration
 ** of two layers of functionality:
 ** - Layer-1 allows to enqueue and prioritise render activity records
 ** - Layer-2 connects and coordinates activities to conduct complex calculations
 ** Additionally, a [custom allocation scheme](\ref BlockFlow) is involved,
 ** a [notification service](\ref EngineObserver) and the execution environment
 ** for the low-level [»Activity Language](\ref ActivityLang). Some operational
 ** control and and load management is delegated to the \ref LoadController.
 ** The *purpose* of the »Scheduler Service« in the lumiera Render Engine
 ** is to coordinate the execution of [»Render Jobs«](\ref vault::gear::Job),
 ** which can be controlled by a timing scheme, but also triggered in response
 ** to some prerequisite event, most notably the completion of IO work.
 ** 
 ** # Thread coordination
 ** The typical situation found when rendering media is the demand to distribute
 ** rather scarce computation resources to various self-contained tasks sequenced
 ** in temporary and dependency order. In addition, some internal management work
 ** must be conducted to order these tasks, generate further tasks and coordinate
 ** the dependencies. Overall, any such internal work is by orders of magnitude
 ** less expensive than the actual media calculations, which reach up into the
 ** range of 1-10 milliseconds, possibly even way more (seconds for expensive
 ** computations). For this reason, the Scheduler in the Lumiera Render Engine
 ** uses a pool of workers, each representing one unit of computation resource
 ** (a »core«), and these workers will _pull work actively,_ rather then
 ** distributing, queuing and dispatching tasks to a passive set of workers.
 ** And notably the »management work« is performed also by the workers themselves,
 ** to the degree it is necessary to retrieve the next piece of computation.
 ** So there is no dedicated »queue manager« — scheduling is driven by the workers.
 ** 
 ** Assuming that this internal work is comparatively cheap to perform, a choice
 ** was made to handle any internal state changes of the Scheduler exclusively
 ** in single-threaded mode. This is achieved by an atomic lock, maintained in
 ** [Layer-2 of the Scheduler implementation](\ref SchedulerCommutator::groomingToken_).
 ** Any thread looking for more work will pull a pre-configured functor, which
 ** is implemented by the [work-function](\ref Scheduler::getWork()). The thread
 ** will attempt to acquire the lock, designated as »grooming-token« -- but only
 ** if this is necessary to perform internal changes. Since workers are calling
 ** in randomly, in many cases there might be no task to perform at the moment,
 ** and the worker can be instructed to go to a sleep cycle and call back later.
 ** On the other hand, when load is high, workers are instructed to call back
 ** immediately again to find the next piece of work. Based on assessment of
 ** the current [»head time«](\ref SchedulerInvocation::headTime), a quick
 ** decision will be made if the thread's capacity is useful right now,
 ** or if this capacity will be re-focussed into another zone of the
 ** scheduler's time axis, based on the distance to the next task.
 ** 
 ** If however a thread is put to work, it will start dequeuing an entry from
 ** the head of the [priority queue](\ref SchedulerInvocation::pullHead),
 ** and start interpreting this entry as a _chain of render activities_  with
 ** the help of the [»Activity Language«](\ref ActivityLang::dispatchChain).
 ** In the typical scenario, after some preparatory checks and notifications,
 ** the thread [transitions into work mode](\ref Scheduler::ExecutionCtx::work),
 ** which entails to [drop the grooming-token](\ref SchedulerCommutator::dropGroomingToken).
 ** Since the scheduler queue only stores references to render activities, which are
 ** allocated in a [special arrangement](\ref BlockFlow) exploiting the known deadline
 ** time of each task, further processing can commence concurrently.
 ** @note The grooming-token should always be dropped by a deliberate state transition.
 **       Notably _internal processing_ (e.g. planning of new jobs) will _not_ drop
 **       the token, since it must be able to change the schedule. Such internal
 **       tasks can be processed in row and will be confined to a single thread
 **       (there is a special treatment at the end of #getWork() to achieve that).
 **       As a safety net, the grooming-token will automatically be dropped after
 **       catching an exception, or when a thread is sent to sleep.
 ** 
 ** @see SchedulerService_test Component integration test
 ** @see SchedulerStress_test
 ** @see SchedulerUsage_test
 ** @see SchedulerInvocation Layer-1
 ** @see SchedulerCommutator Layer-2
 ** @see activity.hpp description of »Render Activities«
 ** 
 ** @todo WIP 11/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_SCHEDULER_H_
#define SRC_VAULT_GEAR_SCHEDULER_H_


#include "lib/error.hpp"
#include "vault/gear/block-flow.hpp"
#include "vault/gear/work-force.hpp"
#include "vault/gear/activity-lang.hpp"
#include "vault/gear/scheduler-commutator.hpp"
#include "vault/gear/scheduler-invocation.hpp"
#include "vault/gear/load-controller.hpp"
#include "vault/gear/engine-observer.hpp"
#include "vault/real-clock.hpp"
//#include "lib/symbol.hpp"
#include  "lib/nocopy.hpp"
//#include "lib/util.hpp"
#include "lib/format-cout.hpp"///////////////////////TODO

//#include <string>
#include <optional>
#include <utility>


namespace vault{
namespace gear {
  
//  using util::isnil;
//  using std::string;
  using std::move;
  using lib::time::Time;
  using lib::time::FSecs;
  using lib::time::Offset;
  using lib::time::Duration;
  
  namespace test {  // declared friend for test access
    class SchedulerService_test;
  }
  
  namespace { // Scheduler default config
    
    const auto   IDLE_WAIT = 20ms;            ///< sleep-recheck cycle for workers deemed _idle_
    const size_t DISMISS_CYCLES = 100;        ///< number of wait cycles before an idle worker terminates completely
    Offset POLL_WAIT_DELAY{FSecs(1,1000)};    ///< delay until re-evaluating a condition previously found unsatisfied
    Offset DUTY_CYCLE_PERIOD{FSecs(1,20)};    ///< period of the regular scheduler »tick« for state maintenance.
    Offset DUTY_CYCLE_TOLERANCE{FSecs(1,10)}; ///< maximum slip tolerated on duty-cycle start before triggering Scheduler-emergency
  }
  
  
  class Scheduler;
  
  class ScheduleSpec
    : util::MoveAssign
    {
      Job job_;
      TimeVar start_{Time::ANYTIME};
      TimeVar death_{Time::NEVER};
      ManifestationID manID_{};
      bool isCompulsory_{false};
      
      Scheduler* theScheduler_;
      std::optional<activity::Term> term_;
      
    public:
      ScheduleSpec (Scheduler& sched, Job job)
        : job_{job}
        , theScheduler_{&sched}
        , term_{std::nullopt}
        { }
      
      ScheduleSpec
      startOffset (microseconds afterNow)
        {
          start_ = RealClock::now() + _uTicks(afterNow);
          return move(*this);
        }
      
      ScheduleSpec
      startTime (Time fixedTime)
        {
          start_ = fixedTime;
          return move(*this);
        }
      
      ScheduleSpec
      lifeWindow (microseconds afterStart)
        {
          death_ = start_ + _uTicks(afterStart);
          return move(*this);
        }
      
      ScheduleSpec
      manifestation (ManifestationID manID)
        {
          manID_ = manID;
          return move(*this);
        }
      
      /** build Activity chain and hand-over to the Scheduler. */
      ScheduleSpec post();
      
      ScheduleSpec
      linkToSuccessor (ScheduleSpec& succSpec)
        {
          term_->appendNotificationTo (*succSpec.term_);
          return move(*this);
        }
      
      ScheduleSpec
      linkToPredecessor (ScheduleSpec& predSpec)
        {
          predSpec.term_->appendNotificationTo (*term_);
          return move(*this);
        }
    };
  
  
  
  /******************************************************//**
   * »Scheduler-Service« : coordinate render activities.
   * @todo WIP 11/2023
   * @see BlockFlow
   * @see SchedulerUsage_test
   */
  class Scheduler
    : util::NonCopyable
    {
      /** Binding of worker callbacks to the scheduler implementation */
      struct Setup : work::Config
        {
          Scheduler& scheduler;
          activity::Proc doWork() { return scheduler.getWork();          }
          void finalHook (bool _) { scheduler.handleWorkerTermination(_);}
        };
      
      
      SchedulerInvocation layer1_;
      SchedulerCommutator layer2_;
      WorkForce<Setup> workForce_;
      
      ActivityLang activityLang_;
      LoadController loadControl_;
      EngineObserver& engineObserver_;
      
      
    public:
      Scheduler (BlockFlowAlloc& activityAllocator
                ,EngineObserver& engineObserver)
        : layer1_{}
        , layer2_{}
        , workForce_{Setup{IDLE_WAIT, DISMISS_CYCLES, *this}}
        , activityLang_{activityAllocator}
        , loadControl_{connectMonitoring()}
        , engineObserver_{engineObserver}
        { }
      
      
      bool
      empty()  const
        {
          return layer1_.empty();
        }
      
      /**
       * Spark the engine self-regulation cycle and power up WorkForce.
       * @note set off automatically when [put to use](\ref #seedCalcStream);
       *       while active, the [duty-cycle](\ref #handleDutyCycle) retains
       *       itself, albeit bound to disengage when falling empty.
       */
      void
      ignite()
        {
          TRACE (engine, "Ignite Scheduler Dispatch.");
          handleDutyCycle (RealClock::now());
          if (not empty())
            workForce_.activate();
        }
      
      
      /**
       * Bring down processing destructively as fast as possible.
       * Dismiss worker threads as soon as possible, and clear the queues.
       * @warning Actually running Activities can not be aborted, but anything
       *    not yet scheduled will be discarded, irrespective of dependencies.
       * @remark should never need to call this in regular operation,
       *    since an empty scheduler disengages automatically.
       */
      void
      terminateProcessing()
        {
          TRACE (engine, "Forcibly terminate Scheduler Dispatch.");
          workForce_.awaitShutdown();
          layer1_.discardSchedule();
        }
      
      
      /**
       * 
       */
      double
      getLoadIndicator()
        {
          return loadControl_.effectiveLoad();
        }
      
      
      /**
       * Set the Scheduler to work on a new CalcStream.
       * @param planningJob a »meta-Job« to schedule a chunk of render-Jobs.
       * @param manID (optional) a manifestation-ID to be enabled for processing
       * @note the planningJob will be dispatched _immediately now,_ which typically
       *       will cause its dispatch in the current thread (but that is not guaranteed).
       *       The _deadline_ is also set automatically to a very large leeway (1/10 sec),
       *       and the new planning job is marked as _compulsory_ — implying that the
       *       Scheduler will [trigger emergency](\ref #triggerEmergency) if this deadline
       *       can not be met. Emergency will cause all PlayProcess to be paused.
       * @remark it is up to the planning instance to use this mechanism properly; the idea
       *       is to [place follow-up jobs](\ref #continueMetaJob) repeatedly, always to
       *       define the next chunk of work jobs. If a ManifestationID is given, then
       *       obviously the work jobs must use the same ID, since jobs with an ID not
       *       [explicitly enabled](\ref SchedulerInvocation::activate) will be silently
       *       discarded (unless the ID is zero, which is always implicitly enabled).
       *       Moreover, the recommendation is to start planning with at least 20ms
       *       of remaining headroom, to ensure smooth allocation of capacity.
       */
      void
      seedCalcStream (Job planningJob
                     ,ManifestationID manID = ManifestationID()
                     ,FrameRate expectedAdditionalLoad = FrameRate(25))
        {
          layer1_.activate (manID);
          activityLang_.announceLoad (expectedAdditionalLoad);
          continueMetaJob (RealClock::now(), planningJob, manID);
        }
      
      
      /**
       * Place a follow-up job-planning job into the timeline.
       */
      void
      continueMetaJob (Time nextStart
                      ,Job planningJob
                      ,ManifestationID manID = ManifestationID())
        {
          bool isCompulsory = true;
          Time deadline = nextStart + DUTY_CYCLE_TOLERANCE;
          // place the meta-Job into the timeline...
          postChain ({activityLang_.buildMetaJob(planningJob, nextStart, deadline)
                                   .post()
                     , nextStart
                     , deadline
                     , manID
                     , isCompulsory});
          ensureDroppedGroomingToken();
        }
      
      
      /**
       * Render Job builder: start definition of a schedule
       * to invoke the given Job. Use the functions on the returned builder
       * to fill in the details of the schedule; defining a start point and
       * a deadline is mandatory. Issue this schedule then by invoking `post()`
       * @warning the deadline is also used to manage the allocation.
       */
      ScheduleSpec
      defineSchedule (Job job)
        {
          return ScheduleSpec{*this, job};
        }
      
      
      
      /**
       * The worker-Functor: called by the active Workers from the
       * \ref WorkForce to pull / perform the actual render Activities.
       */
      activity::Proc getWork();
      
      
    private:
      void postChain (ActivationEvent);
      void handleDutyCycle (Time now);
      void handleWorkerTermination (bool isFailure);
      void maybeScaleWorkForce();
      
      void triggerEmergency();
      
      
      /** send this thread into a targeted short-time wait. */
      activity::Proc scatteredDelay (Time now, LoadController::Capacity);
      
      
      /**
       * monad-like step sequence: perform sequence of steps,
       * as long as the result remains activity::PASS
       */
      struct WorkerInstruction
        {
          activity::Proc lastResult = activity::PASS;
          
          /** exposes the latest verdict as overall result
           * @note returning activity::SKIP from the dispatch
           *   signals early exit, which is acquitted here. */
          operator activity::Proc()
            {
              return activity::SKIP == lastResult? activity::PASS
                                                 : lastResult;
            }
          
          template<class FUN>
          WorkerInstruction
          performStep (FUN step)
            {
              if (activity::PASS == lastResult)
                lastResult = step();
              return move(*this);
            }
        };
      
      
      /** @internal connect state signals for use by the LoadController */
      LoadController::Wiring
      connectMonitoring()
        {
          LoadController::Wiring setup;
          setup.maxCapacity = []{ return work::Config::COMPUTATION_CAPACITY; };
          setup.currWorkForceSize = [this]{ return workForce_.size(); };
          return setup;
        }
      
      void
      ensureDroppedGroomingToken()
        {
          if (layer2_.holdsGroomingToken (thisThread()))
            layer2_.dropGroomingToken();
        }
      
      /** access high-resolution-clock, rounded to µ-Ticks */
      Time
      getSchedTime()
        {
          return RealClock::now();
        }
      
      /** @internal expose a binding for Activity execution */
      class ExecutionCtx;
      friend class ExecutionCtx;
      
      /** the Job builder is allowed to allocate and dispatch */
      friend class ScheduleSpec;
      
      /** open private backdoor for tests */
      friend class test::SchedulerService_test;
    };
  
  
  
  
  /** work-timing event for performance observation */
  class WorkTiming
    : public EngineEvent
    {
      using Payload = EngineEvent::Payload<Time>;
      using EngineEvent::EngineEvent;
      
      static Symbol WORKSTART;
      static Symbol WORKSTOP;
      
    public:
      static WorkTiming start (Time now) { return WorkTiming{WORKSTART, Payload{now}}; }
      static WorkTiming stop  (Time now) { return WorkTiming{WORKSTOP,  Payload{now}}; }
    };
  
  /**
   * @remark when due, the scheduled Activities are performed within the
   *  [Activity-Language execution environment](\ref ActivityLang::dispatchChain());
   *  some aspects of Activity _activation_ however require external functionality,
   *  which — for the purpose of language definition — was abstracted as _Execution-context._
   *  The implementation of these binding functions fills in relevant external effects and
   *  is in fact supplied by the implementation internals of the scheduler itself.
   */
  class Scheduler::ExecutionCtx
    : util::NonCopyable
    {
      Scheduler& scheduler_;
    public:
      
      ActivationEvent rootEvent;
      
      ExecutionCtx(Scheduler& self, ActivationEvent toDispatch)
        : scheduler_{self}
        , rootEvent{toDispatch}
      { }
      
      
      /* ==== Implementation of the Concept ExecutionCtx ==== */
      
      /**
       * λ-post: enqueue for time-bound execution, possibly dispatch immediately.
       * @remark This function represents and _abstracted entrance to scheduling_
       *         for the ActivityLang and is relevant for recursive forwarding
       *         of activations and notifications. The concrete implementation
       *         needs some further contextual information, which is passed
       *         down here as a nested sub-context.
       */
      activity::Proc
      post (Time when, Activity* chain, ExecutionCtx& ctx)
        {
          REQUIRE (chain);
          ActivationEvent chainEvent = ctx.rootEvent;
          chainEvent.activity = chain;
          chainEvent.starting = _raw(chain->constrainedStart (when));
          chainEvent.deadline = _raw(chain->constrainedDeath (chainEvent.deathTime()));
          ExecutionCtx subCtx{scheduler_, chainEvent};
          return scheduler_.layer2_.postDispatch (chainEvent, subCtx, scheduler_.layer1_);
        }
      
      /**
       * λ-work : transition Managment-Mode -> Work-Mode
       * - drop the Grooming-Token (allow concurrent execution from now on)
       * - signal start time of actual processing
       * @warning current thread is expected to hold the Grooming-Token
       */
      void
      work (Time now, size_t qualifier)
        {
          scheduler_.layer2_.dropGroomingToken();
          scheduler_.engineObserver_.dispatchEvent(qualifier, WorkTiming::start(now));
        }
      
      /** λ-done : signal end time of actual processing */
      void
      done (Time now, size_t qualifier)
        {
          scheduler_.engineObserver_.dispatchEvent(qualifier, WorkTiming::stop(now));
        }
      
      /** λ-tick : scheduler management duty cycle */
      activity::Proc
      tick (Time now)
        {
          scheduler_.handleDutyCycle (now);
          return activity::PASS;
        }
      
      Offset
      getWaitDelay()
        {
          return POLL_WAIT_DELAY;
        }
      
      /** access high-resolution-clock, rounded to µ-Ticks */
      Time
      getSchedTime()
        {
          return scheduler_.getSchedTime();
        }
    };
  
  
  
  
  /** @note after invoking this terminal operation,
   *        the schedule is defined and will be triggered
   *        when start time arrives. However, before reaching
   *        this trigger point, the embedded activity::Term can
   *        still be augmented and dependencies can be established.
   *  @remark This ScheduleSpec builder is disposable (and can be moved),
   *        while the actual Activities are allocated into the BlockFlow,
   *        where they are guaranteed to live until reaching the deadline.
   */
  inline ScheduleSpec
  ScheduleSpec::post()
  {
    term_ = move(
      theScheduler_->activityLang_
          .buildCalculationJob (job_, start_,death_));
     //set up new schedule by retrieving the Activity-chain...
    theScheduler_->postChain ({term_->post(), start_
                                            , death_
                                            , manID_
                                            , isCompulsory_});
    theScheduler_->ensureDroppedGroomingToken();
    return move(*this);
  }
  
namespace{
  inline string
  relT (int64_t pling)
  {
    static int64_t nulli{0};
    if (nulli == 0) nulli = pling;
    return util::toString(pling - nulli);
  }
  inline string
  relT (Time plong)
  {
    return relT(_raw(plong));
  }
  
  inline string
  markThread()
  {
    return util::showHashLSB (std::hash<std::thread::id>{}(thisThread()));
  }
}
  
  /**
   * Enqueue for time-bound execution, possibly dispatch immediately.
   * This is the »main entrance« to get some Activity scheduled.
   * @param actEvent the Activity, start time and deadline
   *        and optionally further context information
   */
  inline void
  Scheduler::postChain (ActivationEvent actEvent)
  {
    maybeScaleWorkForce ();
cout<<"‖SCH‖ "+markThread()+": @"+relT(RealClock::now())+" ○ start="+relT(actEvent.starting)+" dead:"+util::toString(actEvent.deadline - actEvent.starting)<<endl;
    ExecutionCtx ctx{*this, actEvent};
    layer2_.postDispatch (actEvent, ctx, layer1_);
  }
  
  
  
  /**
   * @remarks this function is invoked from within the worker thread(s) and will
   *   - decide if and how the capacity of this worker shall be used right now
   *   - possibly go into a short targeted wait state to redirect capacity at a better time point
   *   - and most notably commence with dispatch of render Activities, to calculate media data.
   * @return an instruction for the work::Worker how to proceed next:
   *   - activity::PROC causes the worker to poll again immediately
   *   - activity::SLEEP induces a sleep state
   *   - activity::HALT terminates the worker
   * @note Under some circumstances, this function depends on acquiring the »grooming-token«,
   *     which is an atomic lock to ensure only one thread at a time can alter scheduler internals.
   *     In the regular processing sequence, this token is dropped after dequeuing and processing
   *     some Activities, yet prior to invoking the actual »Render Job«. Explicitly dropping the
   *     token at the end of this function is a safeguard against deadlocking the system.
   *     If some other thread happens to hold the token, SchedulerCommutator::findWork
   *     will bail out, leading to active spinning wait for the current thread.
   */
  inline activity::Proc
  Scheduler::getWork()
  {
    try {
        auto res = WorkerInstruction{}
                      .performStep([&]{
                                        layer2_.maybeFeed(layer1_);
                                        Time now = getSchedTime();
                                        Time head = layer1_.headTime();
                                        return scatteredDelay(now,
                                                  loadControl_.markIncomingCapacity (head,now));
                                      })
                      .performStep([&]{
                                        Time now = getSchedTime();
                                        auto toDispatch = layer2_.findWork (layer1_,now);
cout<<"   ·‖ "+markThread()+": @ "+relT(now)+" HT:"+relT(layer1_.headTime())+"  -> "+(toDispatch? "▶ "+relT(toDispatch.starting): string("∘"))<<endl;
                                        ExecutionCtx ctx{*this, toDispatch};
                                        return layer2_.postDispatch (toDispatch, ctx, layer1_);
                                      })
                      .performStep([&]{
                                        layer2_.maybeFeed(layer1_);
                                        Time now = getSchedTime();
                                        Time head = layer1_.headTime();
                                        return scatteredDelay(now,
                                                  loadControl_.markOutgoingCapacity (head,now));
                                      });
        
        // ensure lock clean-up
        if (res != activity::PASS)
          ensureDroppedGroomingToken();
        return res;
      }
    catch(...)
      {
        ensureDroppedGroomingToken();
        throw;
      }
  }
  
  
  /**
   * A worker [asking for work](\ref #getWork) constitutes free capacity,
   * which can be redirected into a focused zone of the scheduler time axis
   * where it is most likely to be useful, unless there is active work to
   * be carried out right away.
   * @param capacity classification of the capacity to employ this thread
   * @return how to proceed further with this worker
   *       - activity::PASS indicates to proceed or call back immediately
   *       - activity::SKIP causes to exit this round, yet call back again
   *       - activity::WAIT exits and places the worker into sleep mode
   * @note as part of the regular work processing, this function may
   *       place the current thread into a short-term targeted sleep.
   */
  inline activity::Proc
  Scheduler::scatteredDelay (Time now, LoadController::Capacity capacity)
  {
    auto doTargetedSleep = [&]
          { // ensure not to block the Scheduler after management work
            ensureDroppedGroomingToken();
            // relocate this thread(capacity) to a time where its more useful
            Offset targetedDelay = loadControl_.scatteredDelayTime (now, capacity);
            std::this_thread::sleep_for (std::chrono::microseconds (_raw(targetedDelay)));
          };
    auto doTendNextHead = [&]
          {
            Time head = layer1_.headTime();
            if (not loadControl_.tendedNext(head)
                and (layer2_.holdsGroomingToken(thisThread())
                    or layer2_.acquireGoomingToken()))
              loadControl_.tendNext(head);
          };
    
    switch (capacity) {
      case LoadController::DISPATCH:
        return activity::PASS;
      case LoadController::SPINTIME:
        std::this_thread::yield();
        return activity::SKIP;     //  prompts to abort chain but call again immediately
      case LoadController::IDLEWAIT:
        return activity::WAIT;     //  prompts to switch this thread into sleep mode
      case LoadController::TENDNEXT:
        doTendNextHead();
        doTargetedSleep();         //  let this thread wait until nest head time is due
        return activity::SKIP;
      default:
        doTargetedSleep();
        return activity::SKIP;     //  prompts to abort this processing-chain for good
      }
  }
  
  

  
  /**
   * »Tick-hook« : code to maintain sane running status.
   * This function will be invoked [regularly](\ref DUTY_CYCLE_PERIOD) while the scheduler
   * is actively processing; in fact this function determines when the scheduler falls empty
   * and can be shut down — and thus regular invocation is equivalent to running state.
   * Code for all kinds of status updates, low-level clean-up and maintenance work related
   * to the building blocks of the scheduler shall be added here. It will be invoked from
   * within some (random) worker thread, frequently enough for humans to seem like an
   * immediate response, but with sufficient large time period to amortise even slightly
   * more computational expensive work; IO and possibly blocking operations should be
   * avoided here though. Exceptions emanating from here will shut down the engine.
   */
  inline void
  Scheduler::handleDutyCycle (Time now)
  {
cout<<"‖▷▷▷‖ "+markThread()+": @ "+relT(now)+(empty()? string(" EMPTY"): " HT:"+relT(layer1_.headTime()))<<endl;
    // consolidate queue content
    layer1_.feedPrioritisation();
    // clean-up of outdated tasks here
    while (layer1_.isOutdated (now) and not layer1_.isOutOfTime(now))
      layer1_.pullHead();
    // protect against missing the deadline of a compulsory task
    if (layer1_.isOutOfTime (now))
      {
        triggerEmergency();
        return; // leave everything as-is
      }
    
    // clean-up of obsolete Activities
    activityLang_.discardBefore (now);
    
    loadControl_.updateState (now);
    
    if (not empty())
      {// prepare next duty cycle »tick«
        Time nextTick = now + DUTY_CYCLE_PERIOD;
        Time deadline = nextTick + DUTY_CYCLE_TOLERANCE;
        Activity& tickActivity = activityLang_.createTick (deadline);
        postChain (ActivationEvent{tickActivity, nextTick, deadline, ManifestationID(), true});
      }
  }
  
  /**
   * Callback invoked whenever a worker-thread is about to exit
   * @param isFailuere if the exit was caused by uncaught exception
   */
  inline void
  Scheduler::handleWorkerTermination (bool isFailure)
  {
    if (isFailure)
      triggerEmergency();
    else
      loadControl_.markWorkerExit();
Time now = getSchedTime();
cout<<"‖▽▼▽‖ "+markThread()+": @ "+relT(now)<<endl;
  }
  
  /**
   * Hook invoked whenever a new task is passed in.
   * Ensures that the Scheduler is in running state and
   * possibly steps up the WorkForce if not yet running at
   * full computation power.
   * @note the capacity scales down automatically when some
   *       workers fall idle for extended time (> 2sec).
   */
  inline void
  Scheduler::maybeScaleWorkForce()
  {
    if (empty())
{
      ignite();
cout<<"‖IGN‖     wof:"+util::toString(workForce_.size())<<endl;
}
    else
{
      workForce_.incScale();
cout<<"‖•△•‖     wof:"+util::toString(workForce_.size())+" HT:"+relT(layer1_.headTime())<<endl;
}
  }
  
  /**
   * Trip the emergency brake and unwind processing while retaining all state.
   */
  inline void
  Scheduler::triggerEmergency()
  {
    UNIMPLEMENTED ("scheduler overrun -- trigger Emergency");
  }
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_SCHEDULER_H_*/
