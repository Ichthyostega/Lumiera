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
 ** This is the lower layer of the implementation and provides low-level functionality.
 ** 
 ** @see SchedulerInvocation_test
 ** @see SchedulerUsage_test integrated usage
 ** @see scheduler.cpp implementation details
 ** 
 ** @todo WIP-WIP-WIP 10/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_SCHEDULER_INVOCATION_H_
#define SRC_VAULT_GEAR_SCHEDULER_INVOCATION_H_


#include "vault/common.hpp"
#include "lib/nocopy.hpp"
//#include "lib/symbol.hpp"
#include "vault/gear/activity.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/util.hpp"

//#include <string>
#include <queue>
#include <boost/lockfree/queue.hpp>
#include <utility>

namespace vault{
namespace gear {
  
  using lib::time::Time;
//  using util::isnil;
//  using std::string;
  using std::move;
  
  namespace error = lumiera::error;
  
  namespace {// Internal defaults
    const size_t INITIAL_CAPACITY = 128;
  }
  
  
  /**
   * Scheduler Layer-1 : time based dispatch.
   * 
   * @see Scheduler
   * @see SchedulerInvocation_test
   */
  class SchedulerInvocation
    : util::NonCopyable
    {
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
       * and enqueue them according to time order
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
       * If there is an Activity to process now, pick it from the scheduling queue
       * @return `nullptr` if the prioritisation queue is empty,
       *         else a pointer to the most urgent Activity dequeued thereby.
       */
      Activity*
      pullHead()
        {
          Activity* activity = peekHead();
          if (activity)
            priority_.pop();
          return activity;
        }
      
      /** Determine if there is work to do right now */
      bool
      isDue (Time now)  const
        {
          return not priority_.empty()
             and priority_.top().waterlevel <= waterLevel(now);
        }
      
    private:
      static int64_t
      waterLevel (Time time)
        {
          return _raw(time);
        }
    };
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_SCHEDULER_INVOCATION_H_*/
