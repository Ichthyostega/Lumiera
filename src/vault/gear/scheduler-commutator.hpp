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
 ** - use the [Activity Language environment](\ref ActivityLang) to _perform_
 **   the retrieved chain within some worker thread; this is called _dispatch_
 ** The central cross road of this implementation is the #postDispatch function.
 ** @see SchedulerCommutator::acquireGroomingToken()
 ** @see SchedulerCommutator::findWork()
 ** @see SchedulerCommutator::postDispatch()
 ** @see SchedulerCommutator_test
 ** @see scheduler.hpp usage
 **
 */


#ifndef SRC_VAULT_GEAR_SCHEDULER_COMMUTATOR_H_
#define SRC_VAULT_GEAR_SCHEDULER_COMMUTATOR_H_


#include "vault/common.hpp"
#include "vault/gear/activity.hpp"
#include "vault/gear/scheduler-invocation.hpp"
#include "vault/gear/activity-lang.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/nocopy.hpp"

#include <thread>
#include <atomic>


namespace vault{
namespace gear {
  
  using lib::time::Time;
  using std::atomic;
  using std::memory_order::memory_order_relaxed;
  using std::memory_order::memory_order_acquire;
  using std::memory_order::memory_order_release;
  
  
  
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
      
      auto thisThread() { return std::this_thread::get_id(); }
      
      
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
      
      
      /**
       * Decide if Activities shall be performed now and in this thread.
       * @param when the indicated time of start of the first Activity
       * @param now  current scheduler time
       * @return allow dispatch if time is due and GroomingToken was acquired
       * @warning accesses current ThreadID and changes GroomingToken state.
       */
      bool
      decideDispatchNow (Time when, Time now)
        {
          return (when <= now)
             and (holdsGroomingToken (thisThread())
                  or acquireGoomingToken());
        }
      
      
      /** look into the queues and possibly retrieve work due by now */
      Activity*
      findWork (SchedulerInvocation& layer1, Time now)
        {
          if (holdsGroomingToken (thisThread())
              or acquireGoomingToken())
            {
              layer1.feedPrioritisation();
              if (layer1.isDue (now))
                return layer1.pullHead();
            }
          return nullptr;
        }
      
      
      /***************************************************//**
       * This is the primary entrance point to the Scheduler.
       * Engage into activity as controlled by given start time.
       * Attempts to acquire the GroomingToken if Activity is due
       * immediately, otherwise just enqueue it for prioritisation.
       * @param chain the Render Activities to put into action
       * @param when  the indicated time of start for these
       * @param executionCtx abstracted execution environment for
       *              Render Activities (typically backed by the
       *              Scheduler as a whole, including notifications
       * @return Status value to indicate how to proceed processing
       *       - activity::PASS continue processing in regular operation
       *       - activity::WAIT nothing to do now, check back later
       *       - activity::HALT serious problem, cease processing
       * @note Attempts to acquire the GroomingToken for immediate
       *       processing, but not for just enqueuing planned tasks.
       *       Never drops the GroomingToken explicitly (unless when
       *       switching from grooming-mode to work-mode in the course
       *       of processing the given Activity chain regularly).
       */
      template<class EXE>
      activity::Proc
      postDispatch (Activity* chain, Time when
                   ,EXE& executionCtx
                   ,SchedulerInvocation& layer1)
        {
          if (!chain) return activity::WAIT;
          
          Time now = executionCtx.getSchedTime();
          if (decideDispatchNow (when, now))
            return ActivityLang::dispatchChain (*chain, executionCtx);
          else
            if (holdsGroomingToken (thisThread()))
              layer1.feedPrioritisation (*chain, when);
            else
              layer1.instruct (*chain, when);
          return activity::PASS;
        }
    };
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_SCHEDULER_COMMUTATOR_H_*/
