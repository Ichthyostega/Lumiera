/*
  SCHEDULER-COMMUTATOR.hpp  -  coordination layer of the render engine scheduler

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


/** @file scheduler-commutator.hpp
 ** Layer-2 of the Scheduler: coordination and interaction of activities.
 ** This is the upper layer of the implementation and provides high-level functionality.
 ** Rendering Activities are represented as a chain of \ref Activity verbs (records),
 ** which are interconnected to create a low-level _execution language._ The prime
 ** Activity obviously is to \ref Activity::Verb::INVOKE a \ref JobFunctor encapsulating
 ** media processing operations; further Activity verbs provide building blocks for
 ** execution logic, to check preconditions, notify follow-up tasks after calculation
 ** results are available and to control the scheduling process itself. The Scheduler
 ** as a service allows to execute Activities while observing time and dependency
 ** constraints and in response to external events (notably after IO callback).
 **
 ** Activity records are tiny data records (standard layout and trivially constructible);
 ** they are comprised of a verb tag and a `union` for variant parameter storage, and will
 ** be managed _elsewhere_ relying on the \ref BlockFlow allocation scheme. Within the
 ** scheduler queues and execution environment it is thus sufficient to pass `Activity*`.
 ** While the actual media processing is performed concurrently by a \ref WorkForce with
 ** a pool of _actively pulling workers,_ any allocations and changes to internal state
 ** and data structures of the Scheduler itself must be protected against data corruption
 ** by concurrency. The intended usage scenario involves media data computations which
 ** are by several orders of magnitude more expensive than all the further internal
 ** management operations. Thus the design of the Scheduler relies on simple mutual
 ** exclusion (implemented by atomic lock, see \ref SchedulerCommutator::groomingToken_).
 ** Each worker in search for the next task will first _acquire_ the Grooming-Token, then
 ** execute some internal Activities until encountering an actual media computation JobFunctor.
 ** At this point, the execution will switch from _»grooming mode«_ into _work mode;_ the
 ** worker _drops_ the Grooming-Token at this point and will then refrain from touching
 ** any further Scheduler internals. Finally, after completion of the current Render Job,
 ** the worker will again contend for the Grooming-Token to retrieve more work.
 **
 ** In typical usage, Layer-2 of the Scheduler will perform the following operations
 ** - accept and enqueue new task descriptions (as chain-of-Activities)
 ** - retrieve the most urgent entry from Layer-1
 ** - silently dispose of any outdated entries
 ** - use the [Activity Language environment](\ref ActivityLang) to _perform_
 **   the retrieved chain within some worker thread; this is called _dispatch_
 ** The main entrance point into this implementation is the #postChain function.
 ** @see SchedulerCommutator::acquireGroomingToken()
 ** @see SchedulerCommutator::findWork()
 ** @see SchedulerCommutator::postChain()
 ** @see SchedulerCommutator_test
 ** @see scheduler.hpp usage
 **
 */


#ifndef SRC_VAULT_GEAR_SCHEDULER_COMMUTATOR_H_
#define SRC_VAULT_GEAR_SCHEDULER_COMMUTATOR_H_


#include "vault/common.hpp"
#include "vault/gear/activity.hpp"
#include "vault/gear/scheduler-invocation.hpp"
#include "vault/gear/load-controller.hpp"
#include "vault/gear/activity-lang.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-string.hpp"
#include "lib/nocopy.hpp"

#include <thread>
#include <atomic>


namespace vault{
namespace gear {
  
  using lib::time::Offset;
  using lib::time::FSecs;
  using lib::time::Time;
  using std::atomic;
  using std::memory_order::memory_order_relaxed;
  using std::memory_order::memory_order_acquire;
  using std::memory_order::memory_order_release;
  using std::chrono_literals::operator ""us;
  using std::chrono::microseconds;
  
  namespace { // Configuration / Scheduling limit
    
    microseconds GROOMING_WAIT_CYCLE{70us};   ///< wait-sleep in case a thread must forcibly acquire the Grooming-Token
    
    /** convenient short-notation, also used by SchedulerService */
    auto inline thisThread() { return std::this_thread::get_id(); }
  }
  
  
  
  /*************************************************************//**
   * Scheduler Layer-2 : execution of Scheduler Activities.
   * - protect data structures through exclusive »grooming mode«
   * - use the underlying Layer-1 to retrieve the most urgent work
   * - dispatch and execute a chain of _Render Activities_
   * @see SchedulerInvocation (Layer-1)
   * @see SchedulerCommutator_test
   */
  class SchedulerCommutator
    : util::NonCopyable
    {
      using ThreadID = std::thread::id;
      atomic<ThreadID> groomingToken_{};
      
      
    public:
      SchedulerCommutator()  = default;
      
      /**
       * acquire the right to perform internal state transitions.
       * @return `true` if this attempt succeeded
       * @note only one thread at a time can acquire the GoomingToken successfully.
       * @remark only if _testing and branching_ on the return value, this also constitutes
       *         a valid sync barrier; _in this case you can be sure_ to see the real values
       *         of any scheduler internals and are free to manipulate.
       */
      bool
      acquireGoomingToken()  noexcept
        {
          ThreadID expect_noThread;                   // expect no one else to be in...
          return groomingToken_.compare_exchange_strong (expect_noThread, thisThread()
                                                        ,memory_order_acquire // success also constitutes an acquire barrier
                                                        ,memory_order_relaxed // failure has no synchronisation ramifications
                                                        );
        }
      
      /**
       * relinquish the right for internal state transitions.
       * @remark any changes done to scheduler internals prior to this call will be
       *         _sequenced-before_ anything another thread does later, _bot only_
       *         if the other thread first successfully acquires the GroomingToken.
       */
      void
      dropGroomingToken()  noexcept
        {          // expect that this thread actually holds the Grooming-Token
          REQUIRE (groomingToken_.load(memory_order_relaxed) == thisThread());
          const ThreadID noThreadHoldsIt;
          groomingToken_.store (noThreadHoldsIt, memory_order_release);
        }
      
      /**
       * check if the indicated thread currently holds
       * the right to conduct internal state transitions.
       */
      bool
      holdsGroomingToken (ThreadID id)  noexcept
        {
          return id == groomingToken_.load (memory_order_relaxed);
        }
      
      
      class ScopedGroomingGuard;
      /** a scope guard to force acquisition of the GroomingToken */
      ScopedGroomingGuard requireGroomingTokenHere();
      
      
      
      /** tend to the input queue if possible */
      void
      maybeFeed (SchedulerInvocation& layer1)
        {
          if (layer1.hasPendingInput()
              and (holdsGroomingToken(thisThread())
                   or acquireGoomingToken()))
            layer1.feedPrioritisation();
        }
      
      /**
       * Look into the queues and possibly retrieve work due by now.
       * @note transparently discards any outdated entries,
       *       but blocks if a compulsory entry becomes outdated.
       */
      ActivationEvent
      findWork (SchedulerInvocation& layer1, Time now)
        {
          if (holdsGroomingToken (thisThread())
              or acquireGoomingToken())
            {
              layer1.feedPrioritisation();
              while (layer1.isOutdated (now) and not layer1.isOutOfTime(now))
                layer1.pullHead();
              if (layer1.isDue (now))
                {
                  if (layer1.isOutOfTime(now))
                    UNIMPLEMENTED ("how to trigger a Scheduler-Emergency from here");   ///////////////////////TICKET #1362 : not clear where Scheduler-Emergency is to be handled and how it can be triggered. See Scheduler::triggerEmergency()
                  else
                    return layer1.pullHead();
            }   }
          return ActivationEvent();
        }
      
      
      /***********************************************************//**
       * This is the primary entrance point to the Scheduler.
       * Place the given event into the schedule, with prioritisation
       * according to its start time.
       * @param event the chain of Render Activities to be scheduled,
       *              including start time and deadline
       * @return Status value to indicate how to proceed processing
       *       - activity::PASS continue processing in regular operation
       *       - activity::WAIT nothing to do now, check back later
       *       - activity::HALT serious problem, cease processing
       * @note Never attempts to acquire the GroomingToken itself,
       *       but if current thread holds the token, the task can
       *       be placed directly into the scheduler queue.
       */
      activity::Proc
      postChain (ActivationEvent event, SchedulerInvocation& layer1)
        {
          if (holdsGroomingToken (thisThread()))
            layer1.feedPrioritisation (move (event));
          else
            layer1.instruct (move (event));
          return activity::PASS;
        }

      
      /**
       * Implementation of the worker-Functor:
       * - redirect work capacity in accordance to current scheduler and load
       * - dequeue and dispatch the Activity chains from the queue to perform the render jobs.
       */
      template<class DISPATCH, class CLOCK>
      activity::Proc
      dispatchCapacity (SchedulerInvocation&, LoadController&, DISPATCH, CLOCK);
      
      
      
    private:
      activity::Proc
      scatteredDelay (Time now, Time head
                     ,LoadController& loadController
                     ,LoadController::Capacity capacity);
      
      void
      ensureDroppedGroomingToken()
        {
          if (holdsGroomingToken (thisThread()))
            dropGroomingToken();
        }
      
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
    };
  
  
  
  
  
  
  /**
   * @remarks this function is invoked from within the worker thread(s) and will
   *   - decide if and how the capacity of this worker shall be used right now
   *   - possibly go into a short targeted wait state to redirect capacity at a better time point
   *   - and most notably commence with dispatch of render Activities, to calculate media data.
   * @return an instruction for the work::Worker how to proceed next:
   *   - activity::PASS causes the worker to poll again immediately
   *   - activity::KICK to contend (spin) on GroomingToken
   *   - activity::WAIT induces a sleep state
   *   - activity::HALT terminates the worker
   * @note Under some circumstances, this function depends on acquiring the »grooming-token«,
   *     which is an atomic lock to ensure only one thread at a time can alter scheduler internals.
   *     In the regular processing sequence, this token is dropped after dequeuing and processing
   *     some Activities, yet prior to invoking the actual »Render Job«. Explicitly dropping the
   *     token at the end of this function is a safeguard against deadlocking the system.
   *     If some other thread happens to hold the token, SchedulerCommutator::findWork
   *     will bail out, leading to active spinning wait for the current thread.
   */
  template<class DISPATCH, class CLOCK>
  inline activity::Proc
  SchedulerCommutator::dispatchCapacity (SchedulerInvocation& layer1
                                        ,LoadController& loadController
                                        ,DISPATCH executeActivity
                                        ,CLOCK getSchedTime
                                        )
  {
    try {
        auto res = WorkerInstruction{}
                      .performStep([&]{
                                        maybeFeed(layer1);
                                        Time now = getSchedTime();
                                        Time head = layer1.headTime();
                                        return scatteredDelay(now, head, loadController,
                                                  loadController.markIncomingCapacity (head,now));
                                      })
                      .performStep([&]{
                                        Time now = getSchedTime();
                                        auto toDispatch = findWork (layer1,now);
                                        if (not toDispatch) return activity::KICK; // contention
                                        return executeActivity (toDispatch);
                                      })
                      .performStep([&]{
                                        maybeFeed(layer1);
                                        Time now = getSchedTime();
                                        Time head = layer1.headTime();
                                        return scatteredDelay(now, head, loadController,
                                                  loadController.markOutgoingCapacity (head,now));
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
   * A worker [asking for work](\ref #doWork) constitutes free capacity,
   * which can be redirected into a focused zone of the scheduler time axis
   * where it is most likely to be useful, unless there is active work to
   * be carried out right away.
   * @param capacity classification of the capacity to employ this thread
   * @return how to proceed further with this worker
   *       - activity::PASS indicates to proceed or call back immediately
   *       - activity::SKIP causes to exit this round, yet call back again
   *       - activity::KICK signals contention (not emitted here)
   *       - activity::WAIT exits and places the worker into sleep mode
   * @note as part of the regular work processing, this function may
   *       place the current thread into a short-term targeted sleep.
   */
  inline activity::Proc
  SchedulerCommutator::scatteredDelay (Time now, Time head
                                      ,LoadController& loadController
                                      ,LoadController::Capacity capacity)
  {
    auto doTargetedSleep = [&]
          { // ensure not to block the Scheduler after management work
            ensureDroppedGroomingToken();
            // relocate this thread(capacity) to a time where its more useful
            Offset targetedDelay = loadController.scatteredDelayTime (now, capacity);
            std::this_thread::sleep_for (std::chrono::microseconds (_raw(targetedDelay)));
          };
    auto doTendNextHead = [&]
          {
            if (not loadController.tendedNext(head)
                and (holdsGroomingToken(thisThread())
                    or acquireGoomingToken()))
              loadController.tendNext(head);
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
        doTargetedSleep();         //  let this thread wait until next head time is due
        return activity::SKIP;
      default:
        doTargetedSleep();
        return activity::SKIP;     //  prompts to abort this processing-chain for good
      }
  }
  
  
  
  
  
  class SchedulerCommutator::ScopedGroomingGuard
    : util::MoveOnly
    {
      SchedulerCommutator& commutator_;
      bool handledActively_;
      
      bool
      ensureHoldsToken()
        {
          if (commutator_.holdsGroomingToken(thisThread()))
            return false;
          while (not commutator_.acquireGoomingToken())
            std::this_thread::sleep_for (GROOMING_WAIT_CYCLE);
          return true;
        }
      
    public:
      /** @warning can block indefinitely if someone hogs the token */
      ScopedGroomingGuard (SchedulerCommutator& layer2)
        : commutator_(layer2)
        , handledActively_{ensureHoldsToken()}
        { }
      
     ~ScopedGroomingGuard()
        {
          if (handledActively_ and
              commutator_.holdsGroomingToken(thisThread()))
            commutator_.dropGroomingToken();
        }
    };
  
  
  /**
   * @warning this provides very specific functionality
   *    required by the »Scheduler Service« to handle both
   *    _external_ and _internal_ calls properly.
   *  - whenever a thread already holds the GroomingToken,
   *    no further action is performed (so the cost of this
   *    feature is one additional atomic read on the token)
   *  - however, a thread coming _from the outside_ and not
   *    belonging to the Scheduler ecosystem is typically not
   *    aware of the GroomingToken altogether. The token is
   *    acquired, possibly incurring a *blocking wait*, and
   *    it is dropped transparently when leaving the scope.
   */
  inline SchedulerCommutator::ScopedGroomingGuard
  SchedulerCommutator::requireGroomingTokenHere()
    {
      return ScopedGroomingGuard(*this);
    }

  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_SCHEDULER_COMMUTATOR_H_*/
