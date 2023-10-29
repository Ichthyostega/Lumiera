/*
  SCHEDULER-INVOCATION.hpp  -  invocation layer of the render engine scheduler

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


/** @file scheduler-invocation.hpp
 ** Layer-1 of the Scheduler: queueing and prioritisation of activities.
 ** This is the lower layer of the implementation and provides the data structures
 ** necessary to implement low-level scheduling functionality. The Scheduler operates
 ** on Activity records maintained elsewhere, in the \ref BlockFlow allocation scheme.
 ** Layer-2 adds the ability to _perform_ these _Render Activities,_ constituting a
 ** low-level execution language. Since the services of the Scheduler are used in
 ** a multi-threaded context, new entries will be passed in through an lock-free
 ** _Instruction Queue._ The actual time based prioritisation is achieved by the
 ** use of a _Priority Queue_ — which however must be concurrency protected.
 ** The Layer-2 thus assures that _mutating operations_ are performed
 ** exclusively from a special »grooming mode« (management mode).
 ** @see SchedulerCommutator::findWork()
 ** @see SchedulerCommutator::postDispatch()
 ** @see SchedulerInvocation_test
 ** @see SchedulerUsage_test integrated usage
 **
 */


#ifndef SRC_VAULT_GEAR_SCHEDULER_INVOCATION_H_
#define SRC_VAULT_GEAR_SCHEDULER_INVOCATION_H_


#include "vault/common.hpp"
#include "lib/nocopy.hpp"
#include "vault/gear/activity.hpp"
#include "lib/time/timevalue.hpp"

#include <queue>
#include <boost/lockfree/queue.hpp>
#include <utility>

namespace vault{
namespace gear {
  
  using lib::time::Time;
  using std::move;
  
  namespace error = lumiera::error;
  
  namespace {// Internal defaults
    const size_t INITIAL_CAPACITY = 128;
  }
  
  
  /***************************************************//**
   * Scheduler Layer-1 : time based dispatch.
   * Manages pointers to _Render Activity records._
   * - new entries passed in through the #instruct_ queue
   * - time based prioritisation in the #priority_ queue
   * @see Scheduler
   * @see SchedulerInvocation_test
   */
  class SchedulerInvocation
    : util::NonCopyable
    {
      /** @internal data record passed through the queues */
      struct ActOrder
        {
          int64_t   waterlevel{0};
          Activity* activity{nullptr};
          
          /** @internal ordering function for time based scheduling
           *  @note reversed order as required by std::priority_queue
           *        to get the earliest element at top of the queue
           */
          bool
          operator< (ActOrder const& o)  const
            {
              return waterlevel > o.waterlevel;
            }
        };
      
      using InstructQueue = boost::lockfree::queue<ActOrder>;
      using PriorityQueue = std::priority_queue<ActOrder>;
      
      InstructQueue instruct_;
      PriorityQueue priority_;
      
      
    public:
      SchedulerInvocation()
        : instruct_{INITIAL_CAPACITY}
        , priority_{}
        { }
      
      
      /** forcibly clear out the schedule */
      void
      discardSchedule()
        {
          instruct_.consume_all([](auto&){/*obliterate*/});
          priority_ = PriorityQueue();
        }
      
      /**
       * Accept an Activity for time-bound execution
       */
      void
      instruct (Activity& activity, Time when)
        {
          bool success = instruct_.push (ActOrder{waterLevel(when), &activity});
          if (not success)
            throw error::Fatal{"Scheduler entrance: memory allocation failed"};
        }
      
      
      /**
       * Pick up all new Activities from the entrance queue
       * and enqueue them to be retrieved ordered by start time.
       */
      void
      feedPrioritisation()
        {
          ActOrder actOrder;
          while (instruct_.pop (actOrder))
            priority_.push (move (actOrder));
        }
      
      
      /**
       * Feed the given Activity directly into time prioritisation,
       * effectively bypassing the thread dispatching entrance queue.
       * @remark Layer-2 uses this shortcut when in »grooming mode«.
       */
      void
      feedPrioritisation (Activity& activity, Time when)
        {
          priority_.push (ActOrder{waterLevel(when), &activity});
        }
      
      
      /**
       * @return `nullptr` if the queue is empty, else the Activity corresponding
       *         to the currently most urgent element (without dequeuing it).
       */
      Activity*
      peekHead()
        {
          return priority_.empty()? nullptr
                                  : priority_.top().activity;
        }
      
      /**
       * Retrieve from the scheduling queue the Activity with earliest start time.
       * @return `nullptr` if the prioritisation queue is empty,
       *         else a pointer to the most urgent Activity dequeued thereby.
       * @remark Activity Records are managed by the \ref BlockFlow allocator.
       */
      Activity*
      pullHead()
        {
          Activity* activity = peekHead();
          if (activity)
            priority_.pop();
          return activity;
        }
      
      
      /* ===== query functions ===== */
      
      /** Determine if there is work to do right now */
      bool
      isDue (Time now)  const
        {
          return not priority_.empty()
             and priority_.top().waterlevel <= waterLevel(now);
        }
      
      /** detect a compulsory Activity with missed deadline */
      bool
      isOutOfTime()  const
        {
          return false; /////////////////////////////////////////////////////////////////OOO compulsory Activities
        }
      
      bool
      empty()  const
        {
          return instruct_.empty()
             and priority_.empty();
        }
      
      /** @return the earliest time of prioritised work */
      Time
      headTime()  const
        {
          return priority_.empty()? Time::NEVER
                                  : Time{TimeValue{priority_.top().waterlevel}};
        }                              //Note: 64-bit waterLevel corresponds to µ-Ticks
      
    private:
      static int64_t
      waterLevel (Time time)
        {
          return _raw(time);
        }
    };
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_SCHEDULER_INVOCATION_H_*/
