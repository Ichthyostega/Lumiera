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
 ** @par Data maintained in Queue Entries
 **   - the [Activity itself](\ref SchedulerInvocation::ActOrder::activity)
 **     is allocated externally an only referred by pointer; however, this
 **     pointer must not be dereferenced after the *deadline* passed.
 **   - an entry can be scheduled after reaching the
 **     [start time](\ref SchedulerInvocation::ActOrder::waterlevel)
 **   - the [deadline](\ref SchedulerInvocation::ActOrder::deathtime)
 **     marks latest point in time where starting is still allowed
 **   - entries can be tagged with a distinctive
 **     [»manifestation ID«](\ref SchedulerInvocation::ActOrder::manifestationID),
 **     which allows to filter out complete _families_ of already planned entries
 **   - as a safety measure, an entry can be marked as
 **     [compulsory](\ref SchedulerInvocation::ActOrder::isCompulsory).
 **     In regular operation, this has no effect, but an *emergency state*
 **     is triggered in the SchedulerService, should such an entry
 **     [miss it's deadline](\ref SchedulerInvocation::isOutOfTime())
 ** @see SchedulerCommutator::findWork()
 ** @see SchedulerCommutator::postChain()
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
#include "lib/util.hpp"

#include <queue>
#include <boost/lockfree/queue.hpp>
#include <unordered_set>
#include <utility>

namespace vault{
namespace gear {
  
  using lib::time::Time;
  using std::move;
  
  namespace error = lumiera::error;
  
  namespace {// Internal defaults
    const size_t INITIAL_CAPACITY = 128;
  }
  
  /**
   * @internal data record passed through the queues,
   *           representing an event to be scheduled
   */
  struct ActivationEvent
    {
      Activity* activity;
      int64_t   starting;
      int64_t   deadline;
      
      uint32_t  manifestation :32;
      bool      isCompulsory  :1;
      
      ActivationEvent()
        : activity{nullptr}
        , starting{_raw(Time::ANYTIME)}
        , deadline{_raw(Time::NEVER)}
        , manifestation{0}
        , isCompulsory{false}
        { }

      ActivationEvent(Activity& act, Time when
                                   , Time dead =Time::NEVER
                                   , ManifestationID manID =ManifestationID()
                                   , bool compulsory =false)
        : activity{&act}
        , starting{_raw(act.constrainedStart(when))}
        , deadline{_raw(act.constrainedDeath(dead))}
        , manifestation{manID}
        , isCompulsory{compulsory}
        { }
       // default copy operations acceptable
      
      /** @internal ordering function for time based scheduling
       *  @note reversed order as required by std::priority_queue
       *        to get the earliest element at top of the queue
       */
      bool
      operator< (ActivationEvent const& o)  const
        {
          return starting > o.starting;
        }
      
      operator bool()      const { return bool{activity}; }
      operator Activity*() const { return activity; }
      
      Time startTime()     const { return Time{TimeValue{starting}};}
      Time deathTime()     const { return Time{TimeValue{deadline}};}
      
      void
      refineTo (Activity* chain, Time when, Time dead)
        {
          activity = chain;
          starting = _raw(activity->constrainedStart (when.isRegular()? when:startTime()));
          deadline = _raw(activity->constrainedDeath (dead.isRegular()? dead:deathTime()));
        }
    };
  
  
  
  
  /***************************************************//**
   * Scheduler Layer-1 : time based dispatch.
   * Manages pointers to _Render Activity records._
   * - new entries passed in through the #instruct_ queue
   * - time based prioritisation in the #priority_ queue
   * @warning not threadsafe; requires Layer-2 to coordinate.
   * @see Scheduler
   * @see SchedulerInvocation_test
   */
  class SchedulerInvocation
    : util::NonCopyable
    {
      using InstructQueue = boost::lockfree::queue<ActivationEvent>;
      using PriorityQueue = std::priority_queue<ActivationEvent>;
      using ActivationSet = std::unordered_set<ManifestationID>;
      
      InstructQueue instruct_;
      PriorityQueue priority_;
      
      ActivationSet allowed_;
      
    public:
      SchedulerInvocation()
        : instruct_{INITIAL_CAPACITY}
        , priority_{}
        , allowed_{}
        { }
      
      
      /** forcibly clear out the schedule */
      void
      discardSchedule()
        {
          instruct_.consume_all([](auto&){/*obliterate*/});
          priority_ = PriorityQueue();
        }
      
      
      /**
       * Accept an ActivationEvent with an Activity for time-bound execution
       */
      void
      instruct (ActivationEvent actEvent)
        {
          bool success = instruct_.push (move (actEvent));
          if (not success)
            throw error::Fatal{"Scheduler entrance: memory allocation failed"};
        }
      
      
      /**
       * Pick up all new events from the entrance queue
       * and enqueue them to be retrieved ordered by start time.
       */
      void
      feedPrioritisation()
        {
          ActivationEvent actEvent;
          while (instruct_.pop (actEvent))
            priority_.push (move (actEvent));
        }
      
      
      /**
       * Feed the given Activity directly into time prioritisation,
       * effectively bypassing the thread dispatching entrance queue.
       * @remark Layer-2 uses this shortcut when in »grooming mode«.
       */
      void
      feedPrioritisation (ActivationEvent actEvent)
        {
          priority_.push (move (actEvent));
        }
      
      
      /**
       * @return _»empty marker«_ if the queue is empty, else a copy of
       *         the currently most urgent element (without dequeuing it).
       */
      ActivationEvent
      peekHead()
        {
          return priority_.empty()? ActivationEvent()
                                  : priority_.top();
        }
      
      /**
       * Retrieve from the scheduling queue the entry with earliest start time.
       * @return _»empty marker«_ if the prioritisation queue is empty,
       *         else a pointer to the most urgent Activity dequeued thereby.
       * @remark Activity Records are managed by the \ref BlockFlow allocator.
       */
      ActivationEvent
      pullHead()
        {
          ActivationEvent head = peekHead();
          if (head)
            priority_.pop();
          return head;
        }
      
      /**
       * Enable entries marked with a specific ManifestationID to be processed.
       * By default, entries are marked with the default ManifestationID, which
       * is always implicitly activated. Any other ID must be actively allowed,
       * otherwise the entry is deemed [outdated](\ref isOutdated) and will
       * be silently discarded in regular processing by Layer-2.
       * @remark this feature allows to supersede part of a schedule
       */
      void
      activate (ManifestationID manID)
        {
          if (manID)
            allowed_.insert (manID);
        }
      
      void
      drop (ManifestationID manID)
        {
          allowed_.erase (manID);
        }
      
      
      /* ===== query functions ===== */
      
      /** Determine if there is work to do right now */
      bool
      isDue (Time now)  const
        {
          return not priority_.empty()
             and priority_.top().starting <= waterLevel(now);
        }

      /** determine if the Activity at scheduler head missed it's deadline.
       * @warning due to memory management, such an Activity must not be dereferenced */
      bool
      isMissed (Time now)  const
        {
          return not priority_.empty()
             and waterLevel(now) > priority_.top().deadline;
        }
      
      /** determine if Activities with the given ManifestationID shall be processed */
      bool
      isActivated (ManifestationID manID)  const
        {
          return manID == ManifestationID()
              or util::contains (allowed_, manID);
        }
      
      /** determine if Activity at scheduler is outdated and should be discarded */
      bool
      isOutdated (Time now)  const
        {
          return isMissed (now)
              or (not priority_.empty()
                  and not isActivated (priority_.top().manifestation));
        }
      
      /** detect a compulsory Activity at scheduler head with missed deadline */
      bool
      isOutOfTime (Time now)  const
        {
          return isMissed (now)
             and (not priority_.empty()
                  and priority_.top().isCompulsory
                  and isActivated (priority_.top().manifestation));
        }
      
      bool
      hasPendingInput()  const
        {
          return not instruct_.empty();
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
                                  : Time{TimeValue{priority_.top().starting}};
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
