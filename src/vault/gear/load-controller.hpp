/*
  LOAD-CONTROLLER.hpp  -  coordinator for scheduler resource usage

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


/** @file load-controller.hpp
 ** Scheduler resource usage coordination.
 ** Operating the render activities in the engine involves several low-level
 ** support systems, which must be actively guided to remain within sustainable
 ** limits. While all parts of the engine are tuned towards typical expected
 ** scenarios, a wide array of load patterns may be encountered, complicating
 ** any generic performance optimisation. Rather, the participating components
 ** are designed to withstand a short-term imbalance, expecting that general
 ** engine parametrisation will be adjusted based on moving averages.
 **
 ** # Principles for Engine Load Control
 ** 
 ** Scheduling and dispatch of Activities are driven by active workers invoking
 ** the Scheduler-Service to retrieve the next piece of work. While this scheme
 ** ensures that the scarce resource (computation or IO capacity) is directed
 ** towards the most urgent next task, achieving a smooth operation of the engine
 ** without wasted capacity requires additionally to control the request cycles
 ** of the workers, possibly removing excess capacity. Whenever a worker pulls
 ** the next task, an assessment of the timing situation is conducted, and the
 ** worker is placed into some partition of the overall available capacity,
 ** to reflect the current load and demand. Workers are thus moved between
 ** the segments, preferring to assign work to workers already in the active
 ** segment, thereby allowing idle workers to be shut down after some time.
 ** 
 ** The key element to decide upon the classification of a worker is the current
 ** scheduling situation: are some Activities overdue? does the next Activity
 ** to be considered reach far into the future? If there is immediately imminent
 ** work, then capacity is kept around; otherwise the capacity can be considered
 ** to be in excess for now. A worker not required right now can be sent into a
 ** targeted sleep delay, in order to shift its capacity into a zone where it
 ** will more likely be required. It is essential to apply some randomisation
 ** on such capacity shifts, in order to achieve an even distribution of free
 ** capacity and avoid contention between workers asking for new assignments.
 ** 
 ** When a worker becomes available and is not needed at the moment, the first
 ** thing to check is the time of the next approaching Activity; this worker
 ** can then be directed close to this next task, which thereby has been tended
 ** for and can be marked accordingly. Any further worker appearing meanwhile
 ** can then be directed into the time zone _after_ the next approaching task.
 ** Workers immediately returning from active work are always preferred for
 ** assigning new tasks, while workers returning from idle state are typically
 ** sent back into idle state, unless there is direct need for more capacity.
 ** 
 ** @see scheduler.hpp
 ** @see SchedulerStress_test
 ** 
 ** @todo WIP-WIP-WIP 10/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_LOAD_CONTROLLER_H_
#define SRC_VAULT_GEAR_LOAD_CONTROLLER_H_


#include "lib/error.hpp"
//#include "vault/gear/block-flow.hpp"
//#include "vault/gear/activity-lang.hpp"
//#include "lib/symbol.hpp"
#include  "lib/nocopy.hpp"
//#include "lib/util.hpp"

//#include <string>
#include <chrono>
#include <utility>


namespace vault{
namespace gear {
  
//  using util::isnil;
//  using std::string;
  
  using lib::time::Time;
  using lib::time::FSecs;
  using lib::time::TimeVar;
  using lib::time::TimeValue;
  using lib::time::Duration;
  using lib::time::Offset;
  using std::chrono_literals::operator ""ms;
  using std::chrono_literals::operator ""us;
  
  namespace { // Scheduler default config
    
    inline TimeValue
    _uTicks (std::chrono::microseconds us)
    {
      return TimeValue{us.count()};
    }
    
    
    Duration SLEEP_HORIZON{_uTicks (20ms)};
    Duration WORK_HORIZON {_uTicks ( 5ms)};
    Duration NOW_HORIZON  {_uTicks (50us)};
  }
  
  
  
  /**
   * Controller to coordinate resource usage related to the Scheduler.
   * @todo WIP-WIP 10/2023 gradually filling in functionality as needed
   * @see BlockFlow
   * @see Scheduler
   */
  class LoadController
    : util::NonCopyable
    {
    public:
      struct Wiring
        {
          size_t maxCapacity{2};
          ///////TODO add here functors to access performance indicators
        };
      
      explicit
      LoadController (Wiring&& wiring)
        : wiring_{std::move (wiring)}
        { }
      
      LoadController()
        : LoadController{Wiring{}}
        { }
      
    private:
      const Wiring wiring_;
      
      TimeVar tendedHead_{Time::ANYTIME};
      
    public:
      /**
       * did we already tend for the indicated next head time?
       * @note const and non-grooming
       */
      bool
      tendedNext (Time nextHead)  const
        {
          return nextHead == tendedHead_;
        }
      
      /**
       * Mark the indicated next head time as tended.
       * @remark while this is just implemented as simple state,
       *   the meaning is that some free capacity has been directed
       *   towards that time, and thus further capacity go elsewhere.
       * @warning must hold the grooming-Token to use this mutation.
       */
      void
      tendNext (Time nextHead)
        {
          tendedHead_ = nextHead;
        }
      
      
      
      /** Allocation of capacity to time horizon of expected work */
      enum Capacity {DISPATCH   ///< sent to work
                    ,TENDNEXT   ///< reserved for next task
                    ,SPINTIME   ///< awaiting imminent activities
                    ,NEARTIME   ///< capacity for active processing required
                    ,WORKTIME   ///< typical stable work task rhythm expected
                    ,IDLETIME   ///< time to go to sleep
                    };
      
      /** classification of time horizon for scheduling */
      static Capacity
      classifyTimeHorizon (Offset off)
        {
          if (off > SLEEP_HORIZON) return IDLETIME;
          if (off > WORK_HORIZON)  return WORKTIME;
          if (off > NOW_HORIZON)   return NEARTIME;
          if (off > Time::ZERO)    return SPINTIME;
          else                     return DISPATCH;
        }
      
      
      /** decide how this thread's capacity shall be used
       *  after it returned from being actively employed */
      Capacity
      markOutgoingCapacity (Time head, Time now)
        {
          auto horizon = classifyTimeHorizon (Offset{head - now});
          return horizon > SPINTIME
             and not tendedNext(head)? TENDNEXT
                                     : horizon;
        }
      
      /** decide how this thread's capacity shall be used
       *  when returning from idle wait and asking for work */
      Capacity
      markIncomingCapacity (Time head, Time now)
        {
          return classifyTimeHorizon (Offset{head - now})
               > NEARTIME ? IDLETIME
                          : markOutgoingCapacity(head,now);
        }
      
      
      
      /**
       * Generate a time offset to relocate currently unused capacity
       * to a time range where it's likely to be needed. Assuming the
       * classification is based on the current distance to the next
       * Activity known to the scheduler (the next tended head time).
       * - for capacity immediately to be dispatched this function
       *   will not be used, yet returns logically sound values.
       * - after the next head time has been tended for, free capacity
       *   should be relocated into a time span behind that point
       * - the closer the next head time, the more focused this relocation
       * - but each individual delay is randomised within those time bounds,
       *   to produce an even »flow« of capacity on average. Randomisation
       *   relies on a hash (bit rotation) of current time, broken down
       *   to the desired time horizon.
       */
      Offset
      scatteredDelayTime (Time now, Capacity capacity)
        {
          auto scatter = [&](Duration horizon)
                            {
                              gavl_time_t wrap = hash_value(now) % _raw(horizon);
                              ENSURE (0 <= wrap and wrap < _raw(horizon));
                              return TimeValue{wrap};
                            };
          
          switch (capacity) {
            case DISPATCH:
              return Offset::ZERO;
            case SPINTIME:
              return Offset::ZERO;
            case TENDNEXT:
              return Offset{tendedHead_-now};
            case NEARTIME:
              return Offset{tendedHead_-now  + scatter(WORK_HORIZON)};
            case WORKTIME:
              return Offset{tendedHead_-now  + scatter(SLEEP_HORIZON)};
            case IDLETIME:
              return Offset{/*no base offset*/ scatter(SLEEP_HORIZON)};
            default:
              NOTREACHED ("uncovered work capacity classification.");
            }
        }
    };
  
  
  
}}// namespace vault::gear
#endif /*SRC_VAULT_GEAR_LOAD_CONTROLLER_H_*/
