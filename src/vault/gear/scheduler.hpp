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
        , loadControl_{activityAllocator}
        , engineObserver_{engineObserver}
        { }
      
      
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
      
      
      /**
       * send this thread into a targeted short-time wait.
       * @return how to proceed further with this worker
       */
      activity::Proc
      scatteredDelay (LoadController::Capacity capacity)
        {
          UNIMPLEMENTED("scattered short-term delay");
        }
      
      
      /**
       * monad-like step sequence: perform sequence of steps,
       * as long as the result remains activity::PASS
       */
      struct WorkerInstruction
        {
          activity::Proc lastResult = activity::PASS;
          
          /*** exposes the latest verdict as overall result */
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
      
      /** @internal expose a binding for Activity execution */
      class ExecutionCtx;
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
      
      Time
      getSchedTime()
        {
          UNIMPLEMENTED ("access scheduler Time");
        }
    };
  
  
  
  
  /**
   * @remarks this function is invoked from within the worker thread(s) and will
   *   - decide if and how the capacity of this worker shall be used right now
   *   - possibly go into a short targeted wait state to redirect capacity at a better time point
   *   - and most notably jump into the dispatch of the render Activities, to calculate media data.
   * @return an instruction for the work::Worker how to proceed next:
   *   - activity::PROC causes the worker to poll again immediately
   *   - activity::SLEEP induces a sleep state
   *   - activity::HALT terminates the worker
   */
  inline activity::Proc
  Scheduler::getWork()
  {
    ExecutionCtx& ctx = ExecutionCtx::from(*this);
    Time now = ctx.getSchedTime();
    Time head = layer1_.headTime();
    
    return WorkerInstruction{}
              .performStep([&]{ return scatteredDelay(
                                          loadControl_.incomingCapacity (head,now)); 
                              })
              .performStep([&]{
                                Activity* act = layer2_.findWork (layer1_,now);
                                return ctx.post (now, act, ctx);
                              })
              .performStep([&]{ return scatteredDelay(
                                          loadControl_.outgoingCapacity (head,now)); 
                              })
              ;
  }

  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_SCHEDULER_H_*/
