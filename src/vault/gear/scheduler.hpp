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
 ** 
 ** @see SchedulerUsage_test Component integration test
 ** @see scheduler.cpp implementation details
 ** @see SchedulerInvocation Layer-1
 ** @see SchedulerCommutator Layer-2
 ** 
 ** @todo WIP-WIP 10/2023 »Playback Vertical Slice«
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
    
    const auto   IDLE_WAIT = 20ms;           ///< sleep-recheck cycle for workers deemed _idle_
    const size_t DISMISS_CYCLES = 100;       ///< number of wait cycles before an idle worker terminates completely
    Offset POLL_WAIT_DELAY{FSecs(1,1000)};   ///< delay until re-evaluating a condition previously found unsatisfied
  }
  
  
  
  
  /******************************************************//**
   * »Scheduler-Service« : coordinate render activities.
   * @todo WIP-WIP 10/2023
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
       * 
       */
      void
      ignite()
        {
          UNIMPLEMENTED("suicide");
        }
      
      
      /**
       * 
       */
      void
      terminateProcessing()
        {
          UNIMPLEMENTED("suicide");
        }
      
      
      /**
       * 
       */
      double
      getLoadIndicator()
        {
          UNIMPLEMENTED("load indicator");
        }
      
      
      /**
       * 
       */
      void
      seedCalcStream()
        {
          UNIMPLEMENTED("get it going");
        }
      
      
      /**
       * 
       */
      void
      buildJob()
        {
          UNIMPLEMENTED("wrap the ActivityTerm");
        }
      
      
      /**
       * The worker-Functor: called by the active Workers from the
       * \ref WorkForce to pull / perform the actual render Activities.
       */
      activity::Proc getWork();
      
      
    private:
      void
      handleWorkerTermination (bool isFailure)
        {
          UNIMPLEMENTED("die harder");
        }
      
      
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
          setup.maxCapacity = work::Config::COMPUTATION_CAPACITY;
          return setup;
        }
      
      /** @internal expose a binding for Activity execution */
      class ExecutionCtx;
      
      
      /** open private backdoor for tests */
      friend class test::SchedulerService_test;
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
    : private Scheduler
    {
    public:
      static ExecutionCtx&
      from (Scheduler& self)
      {
        return static_cast<ExecutionCtx&> (self);
      }
      
      /* ==== Implementation of the Concept ExecutionCtx ==== */
      
      /**
       * λ-post: enqueue for time-bound execution, possibly dispatch immediately.
       * This is the »main entrance« to get some Activity scheduled.
       * @remark the \a ctx argument is redundant (helpful for test/mocking)
       */
      activity::Proc
      post (Time when, Activity* chain, ExecutionCtx& ctx)
        {
          return layer2_.postDispatch (chain, when, ctx, layer1_);
        }
      
      void
      work (Time, size_t)
        {
          UNIMPLEMENTED ("λ-work");
        }
      
      void
      done (Time, size_t)
        {
          UNIMPLEMENTED ("λ-done");
        }
      
      activity::Proc
      tick (Time)
        {
          UNIMPLEMENTED ("λ-tick");
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
          return RealClock::now();
        }
    };
  
  
  
  
  /**
   * @remarks this function is invoked from within the worker thread(s) and will
   *   - decide if and how the capacity of this worker shall be used right now
   *   - possibly go into a short targeted wait state to redirect capacity at a better time point
   *   - and most notably commence with dispatch of render Activities, to calculate media data.
   * @return an instruction for the work::Worker how to proceed next:
   *   - activity::PROC causes the worker to poll again immediately
   *   - activity::SLEEP induces a sleep state
   *   - activity::HALT terminates the worker
   */
  inline activity::Proc
  Scheduler::getWork()
  {
    ExecutionCtx& ctx = ExecutionCtx::from(*this);
    
    return WorkerInstruction{}
              .performStep([&]{
                                Time now = ctx.getSchedTime();
                                Time head = layer1_.headTime();
                                return scatteredDelay(now,
                                          loadControl_.markIncomingCapacity (head,now));
                              })
              .performStep([&]{
                                Time now = ctx.getSchedTime();
                                Activity* act = layer2_.findWork (layer1_,now);
                                return ctx.post (now, act, ctx);
                              })
              .performStep([&]{
                                Time now = ctx.getSchedTime();
                                Time head = layer1_.headTime();
                                return scatteredDelay(now,
                                          loadControl_.markOutgoingCapacity (head,now));
                              })
              ;
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
          {
            Offset targetedDelay = loadControl_.scatteredDelayTime (now, capacity);
            std::this_thread::sleep_for (std::chrono::microseconds (_raw(targetedDelay)));
          };
    auto doTendNextHead = [&]
          {
            Time head = layer1_.headTime();
            auto self = std::this_thread::get_id();
            if (not loadControl_.tendedNext(head)
                and (layer2_.holdsGroomingToken(self)
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
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_SCHEDULER_H_*/
