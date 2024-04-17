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
 ** is implemented by the [work-function](\ref Scheduler::doWork()). The thread
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
 **       (there is a special treatment at the end of #doWork() to achieve that).
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
  
  namespace test {  // declared friend for test access
    class SchedulerService_test;
  }
  
  namespace { // Scheduler default config
    
    const auto   IDLE_WAIT = 20ms;            ///< sleep-recheck cycle for workers deemed _idle_
    const size_t DISMISS_CYCLES = 100;        ///< number of wait cycles before an idle worker terminates completely
    Offset DUTY_CYCLE_PERIOD{FSecs(1,20)};    ///< period of the regular scheduler »tick« for state maintenance.
    Offset DUTY_CYCLE_TOLERANCE{FSecs(2,10)}; ///< maximum slip tolerated on duty-cycle start before triggering Scheduler-emergency
    Offset FUTURE_PLANNING_LIMIT{FSecs{20}};  ///< limit timespan of deadline into the future (~360 MiB max)
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
      
      ScheduleSpec
      compulsory (bool indeed =true)
        {
          isCompulsory_ = indeed;
          return move(*this);
        }
      
      
      /** build Activity chain and hand-over to the Scheduler. */
      ScheduleSpec post();
      
      ScheduleSpec linkToSuccessor (ScheduleSpec&,  bool unlimitedTime =false);
      ScheduleSpec linkToPredecessor(ScheduleSpec&, bool unlimitedTime =false);
    private:
      void maybeBuildTerm();
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
          activity::Proc doWork() { return scheduler.doWork();           }
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
          bool force_continued_run{true};
          handleDutyCycle (RealClock::now(), force_continued_run);
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
       * @return a synthetic indicator fused from several observations
       *       - 1.0 defines full work capacity yet no significant congestion
       *       - values > 2.0 indicate overload
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
          auto guard = layer2_.requireGroomingTokenHere();  // allow mutation
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
      activity::Proc doWork();
      
      
    private:
      void postChain (ActivationEvent);
      void sanityCheck (ActivationEvent const&);
      void handleDutyCycle (Time now, bool =false);
      void handleWorkerTermination (bool isFailure);
      void maybeScaleWorkForce (Time startHorizon);
      
      void triggerEmergency();
      
      
      /** @internal connect state signals for use by the LoadController */
      LoadController::Wiring
      connectMonitoring()
        {
          LoadController::Wiring setup;
          setup.maxCapacity = []{ return work::Config::COMPUTATION_CAPACITY; };
          setup.currWorkForceSize = [this]{ return workForce_.size(); };
          setup.stepUpWorkForce   = [this](uint steps){ workForce_.incScale(steps); };
          return setup;
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
       * λ-post: enqueue for time-bound execution, within given ExecutionCtx.
       * @remark This function represents an _abstracted entrance to scheduling_
       *         for the ActivityLang and is relevant for recursive forwarding
       *         of activations and notifications. The concrete implementation
       *         needs some further contextual information, which is passed
       *         down here as a nested sub-context.
       */
      activity::Proc
      post (Time when, Time dead, Activity* chain, ExecutionCtx& ctx)
        {
          REQUIRE (chain);
          ActivationEvent chainEvent = ctx.rootEvent;
          chainEvent.refineTo (chain, when, dead);
          scheduler_.sanityCheck (chainEvent);
          return scheduler_.layer2_.postChain (chainEvent, scheduler_.layer1_);
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
      
      /** access high-resolution-clock, rounded to µ-Ticks */
      Time
      getSchedTime()
        {
          return scheduler_.getSchedTime();
        }
    };
  
  
  inline activity::Proc
  Scheduler::doWork()
  {
    return layer2_.dispatchCapacity (layer1_
                                    ,loadControl_
                                    ,[this](ActivationEvent toDispatch)
                                            {
                                              ExecutionCtx ctx{*this, toDispatch};
                                              return ActivityLang::dispatchChain (toDispatch, ctx);
                                            }
                                    ,[this]{ return getSchedTime(); }
                                    );
  }
  
  
  
  
  
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
  {                                  // protect allocation
//  auto guard = theScheduler_->layer2_.requireGroomingTokenHere();//////////////////////////////////////TODO  can we avoid that?
    maybeBuildTerm();
     //set up new schedule by retrieving the Activity-chain...
    theScheduler_->postChain ({term_->post(), start_
                                            , death_
                                            , manID_
                                            , isCompulsory_});
    return move(*this);
  }
  
  /**
   * @internal if not done yet, then construct the Activity-Language term
   *   describing the schedule according to the parameters set thus far.
   */
  inline void
  ScheduleSpec::maybeBuildTerm()
  {
    if (term_) return;
    term_ = move(
      theScheduler_->activityLang_
          .buildCalculationJob (job_, start_,death_));
  }
  
  inline ScheduleSpec
  ScheduleSpec::linkToSuccessor (ScheduleSpec& succSpec, bool unlimitedTime)
    {
      this->maybeBuildTerm();
      succSpec.maybeBuildTerm();
      term_->appendNotificationTo (*succSpec.term_, unlimitedTime);
      return move(*this);
    }
  
  inline ScheduleSpec
  ScheduleSpec::linkToPredecessor (ScheduleSpec& predSpec, bool unlimitedTime)
    {
      predSpec.maybeBuildTerm();
      this->maybeBuildTerm();
      predSpec.term_->appendNotificationTo (*term_, unlimitedTime);
      return move(*this);
    }
  
  
  
  inline void
  Scheduler::sanityCheck (ActivationEvent const& event)
    {
      if (not event)
        throw error::Logic ("Empty event passed into Scheduler entrance");
      if (event.startTime() == Time::ANYTIME)
        throw error::Fatal ("Attempt to schedule an Activity without valid start time");
      if (event.deathTime() == Time::NEVER)
        throw error::Fatal ("Attempt to schedule an Activity without valid deadline");
      Time now{getSchedTime()};
      Offset toDeadline{now, event.deathTime()};
      if (toDeadline > FUTURE_PLANNING_LIMIT)
        throw error::Fatal (util::_Fmt{"Attempt to schedule Activity %s "
                                       "with a deadline by %s into the future"}
                                      % *event.activity
                                      % toDeadline);
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
    sanityCheck (actEvent);
    maybeScaleWorkForce (actEvent.startTime());
    layer2_.postChain (actEvent, layer1_);
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
   * @param forceContinuation whether a follow-up DutyCycle _must_ happen,
   *        irrespective if the queue has still further entries. Used
   *        on first Tick-Cycle directly after ignition, which is
   *        then also shortened (to improve scheduling precision)
   */
  inline void
  Scheduler::handleDutyCycle (Time now, bool forceContinuation)
  {
    auto guard = layer2_.requireGroomingTokenHere();
    
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
    
    if (not empty() or forceContinuation)
      {// prepare next duty cycle »tick«
        Time nextTick = now + (forceContinuation? WORK_HORIZON : DUTY_CYCLE_PERIOD);
        Time deadline = nextTick + DUTY_CYCLE_TOLERANCE;
        Activity& tickActivity = activityLang_.createTick (deadline);
        ActivationEvent tickEvent{tickActivity, nextTick, deadline, ManifestationID(), true};
        layer2_.postChain (tickEvent, layer1_);
      } // *deliberately* use low-level entrance
  }    //  to avoid ignite() cycles and derailed load-regulation
  
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
  Scheduler::maybeScaleWorkForce (Time startHorizon)
  {
    if (empty())
      ignite();
    else
      loadControl_.ensureCapacity (startHorizon);
  }
  
  /**
   * Trip the emergency brake and unwind processing while retaining all state.
   * @todo as of 4/2024 it is not clear what Scheduler-Emergency actually entails;
   *       thus it can not be determined at which level of the component the handler
   *       must reside, and how it can be reached. Notably there exists a situation
   *       in `SchedulerCommutatior::findWork()`, where emergency can happen, yet
   *       in its current configuration, the Layer-2 has no access to the top-level
   *       Scheduler component.                             /////////////////////////////////////////////////TICKET #1362 : where to handle Scheduler-Emergency
   */
  inline void
  Scheduler::triggerEmergency()
  {
    UNIMPLEMENTED ("scheduler overrun -- trigger Emergency");
  }
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_SCHEDULER_H_*/
