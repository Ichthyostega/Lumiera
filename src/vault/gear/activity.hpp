/*
  ACTIVITY.hpp  -  elementary operation to be handled by the scheduler

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


/** @file activity.hpp
 ** Descriptor for a piece of operational logic performed by the scheduler.
 ** The Scheduler in Lumiera's Vault coordinates the rendering activities,
 ** by passing and responding to _Activity messages,_ thereby observing
 ** temporal limits (deadlines) and dependency constraints. A predefined
 ** and fixed set of _Activity verbs_ defines atomic operations the engine
 ** is able to perform, directed by the scheduler. Complex calculation
 ** tasks are constructed from interconnected activities, passing through
 ** the messaging and priority queue of the scheduler. While an Activity
 ** is in processing, the corresponding descriptor data record is maintained
 ** by the BlockStream custom memory manager.
 ** 
 ** @note right now this is a pipe dream
 ** @see ////TODO_test usage example
 ** @see scheduler.cpp implementation
 ** 
 ** @todo WIP-WIP-WIP 6/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_ACTIVITY_H_
#define SRC_VAULT_GEAR_ACTIVITY_H_


#include "vault/common.hpp"
#include "vault/gear/job.h"
#include "lib/time/timevalue.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <string>


namespace vault{
namespace gear {
  
  using lib::time::TimeValue;
  using lib::time::TimeVar;
//  using util::isnil;
//  using std::string;
  
  /**
   * Wrapper to hold Time values in trivially constructible union.
   * By an unfortunate design decision, lib::time::Time values are
   * non-copyable, which prevents placing them into POD data
   * 
   * @todo 7/2023 this decision should be revised  //////////////////////////////////////////////////////////TICKET #1261 : reconsider (im)mutability of time entities
   */
  class Instant
    {
      int64_t microTick_;
      
    public:
      Instant()  =default;  // @suppress("Class members should be properly initialized")
      
      Instant(TimeValue time)
        : microTick_{_raw(time)}
      { }
      
      operator TimeVar()  const
        {
          return TimeValue{microTick_};
        }
      
      // default copy acceptable
    };
  
  
  
  /**
   * Term to describe an Activity,
   * to happen within the Scheduler's control flow.
   * @note Activity is a »POD with constructor«
   *       - trivially *destructible*
   *       - trivially *copyable*
   *       - standard layout
   */
  class Activity
    {
    public:
      /**  All possible kinds of activities */
      enum Verb {INVOKE     ///< dispatch a JobFunctor into a worker thread
                ,TIMESTART  ///< signal start of some processing (for timing measurement)
                ,TIMESTOP   ///< correspondingly signal end of some processing
                ,NOTIFY     ///< push a message to another Activity
                ,GATE       ///< probe window + count-down; activate next Activity, else re-schedule
                ,FEED       ///< supply additional payload data for a preceding Activity
                ,POST       ///< post a message providing a chain of further time-bound Activities
                ,TICK       ///< internal engine »heart beat« for internal maintenance hook(s)
                };
      
      const Verb verb_;
      
      /**
       *  Activities are organised into _chains_
       *  to represent _relations_ based on verbs.
       */
      Activity* next;
      
      
      /* === Activity Data Arguments === */
      
      /** Payload data to provide */
      struct Feed
        {
          size_t one;
          size_t two;
        };
      
      /** Timing observation to propagate */
      struct Timing
        {
          Instant instant;
          size_t  quality;
        };
      
      /** Access gate condition to evaluate */
      struct Condition
        {
          int     rest;     ///< alive while rest > 0
          Instant dead;     ///< alive while time < dead
        };
      
      /** Time window to define for activation */
      struct TimeWindow
        {
          Instant life;
          Instant dead;
        };
      
      /** External work functor to activate */
      struct Invocation
        {
          JobFunctor* task;
          Instant     time;
        };
      
      /** Notification towards another Activity */
      struct Notification
        {
          Activity* target;
          size_t    report;
        };
      
      
      /** Storage of Argument data dependent on Activity::verb_ */
      union ArgumentData
        {
                Feed feed;
              Timing timing;
           Condition condition;
          TimeWindow timeWindow;
          Invocation invocation;
        Notification notification;
        };
      ArgumentData data_;
      
      
      explicit
      Activity (Verb verb)  noexcept
        : verb_{verb}
        , next{nullptr}
        {
          setDefaultArg (verb);
        }
      
      // using default copy/assignment
      
      
      /* ==== special case initialisation ==== */
      
      Activity (size_t o1, size_t o2)  noexcept
        : Activity{FEED}
        {
          data_.feed.one = o1;
          data_.feed.two = o2;
        }
      
      explicit
      Activity (Activity* target)  noexcept
        : Activity{NOTIFY}
        {
          data_.notification.target = target;
        }
      
      explicit
      Activity (int expectNotifications, Time deadline = Time::NEVER)  noexcept
        : Activity{GATE}
        {
          data_.condition.rest = expectNotifications;
          data_.condition.dead = deadline;
        }
      
      Activity (Time when, Activity* followUp)  noexcept
        : verb_{POST}
        , next{followUp}
        {
          data_.timeWindow = {when,when};
        }
      
      Activity (Time start, Time after, Activity* followUp)  noexcept
        : verb_{POST}
        , next{followUp}
        {
          data_.timeWindow = {start,after};
        }
      
      Activity()  noexcept
        : Activity{TICK}
        { }
      
      
    private:
      void
      setDefaultArg (Verb verb)  noexcept
        {
          data_ = {0,0};
          switch (verb) {
            case INVOKE:
              data_.invocation.time = Time::ANYTIME;
              break;
            case TIMESTART:
            case TIMESTOP:
              data_.timing.instant = Time::NEVER;
              break;
            case GATE:
              data_.condition.rest = 1;
              data_.condition.dead = Time::NEVER;
              break;
            case POST:
              data_.timeWindow.life = Time::ANYTIME;
              data_.timeWindow.dead = Time::NEVER;
              break;
            default:
              break;
            }
        }
    };
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_ACTIVITY_H_*/
