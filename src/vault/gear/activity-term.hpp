/*
  ACTIVITY-TERM.hpp  -  definition language framework for scheduler activities

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


/** @file activity-term.hpp
 ** A term of the _activity language_ describing interconnected scheduler activities.
 ** The activity::Term is created as a transient object in _builder notation,_ and used
 ** to lay out the specifics of the planned operations necessary to calculate a single
 ** frame or to carry out some administrative task. The actual \ref Activity verbs are
 ** allocated independently, within the BlockFlow allocation scheme, while the Term
 ** links and configures these data records and prepares them to instruct the Scheduler.
 ** Through the activity::Term, relevant attachment points are exposed for configuration,
 ** yet after posting the entrance point of an activity chain to the Scheduler, the term
 ** is obsoleted and can be discarded without affecting the individual Activities awaiting
 ** activation through the Scheduler.
 ** 
 ** @see SchedulerActivity_test
 ** @see activity-lang.hpp Entrance point to Activity definition
 ** @see activity.hpp definition of verbs
 ** 
 ** @todo WIP-WIP-WIP 8/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_ACTIVITY_TERM_H_
#define SRC_VAULT_GEAR_ACTIVITY_TERM_H_


#include "vault/gear/activity.hpp"
#include "vault/gear/block-flow.hpp"
#include "vault/gear/job.h"
//#include "lib/symbol.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/util.hpp"

//#include <tuple>
#include <string>
#include <utility>


namespace vault{
namespace gear {
  
  using lib::time::Time;
  using lib::time::TimeValue;
//  using util::isnil;
  using std::string;
  using std::move;
  
  using BlockFlowAlloc = BlockFlow<blockFlow::RenderConfig>;
  
  
  namespace activity {
    
    /**
     * A Term of the »Activity Language«, describing the steps necessary
     * to perform the calculation of a single frame or similar tasks.
     */
    class Term
      {
        
        using AllocHandle = BlockFlowAlloc::AllocatorHandle;
        
        AllocHandle alloc_;
        
        Activity* invoke_{nullptr};
        Activity* post_{nullptr};
        
        Activity* gate_{nullptr};
        
        
      public:
        enum Template {CALC_JOB  ///< scheme for a synchronous media calculation job
                      ,LOAD_JOB  ///< scheme for an asynchronous data retrieval job
                      ,META_JOB  ///< scheme for planning and organisational job
                      };
        
        explicit
        Term (AllocHandle&& allocHandle, Template kind, Time start, Time after, Job job)
          : alloc_{move (allocHandle)}
          , invoke_{setupInvocation (job)}
          , post_{setupPost (start,after, invoke_)}
          {
            configureTemplate (kind);
          }
        
        // standard copy acceptable
        
        operator std::string()  const
          {
            return "Term-"
                 + (post_? string{*post_} : util::BOTTOM_INDICATOR)
                 + "⧐"
                 + (invoke_? string{*invoke_} : util::BOTTOM_INDICATOR);
          }
        
        
        /**
         * @return entrance point to this Activity-chain setup.
         * @remark use this call for instructing the Scheduler.
         */
        Activity&
        post()
          {
            REQUIRE (post_, "Activity Term not yet fully configured");
            return *post_;
          }
        
        /**
         * Builder operation: block this Term waiting for prerequisite notification.
         * @param notificationSrc an `NOTIFY`-Activity to attach the notification-link
         * @note using this feature implies to wire in a `GATE`-Activity (if not already
         *       present) and to increase the Gate's latch counter. Moreover, the Argument,
         *       _must be a `NOTIFY`_ and will be modified to store the link to this receiving
         *       Gate; typically this function is actually invoked starting from the other
         *       Term — the prerequisite — by invoking #appendNotificationTo(targetTerm).
         */
        Term&
        expectNotification (Activity& notificationSrc)
          {
            REQUIRE (notificationSrc.is (Activity::NOTIFY));
            setupGate();
            gate_->incDependencies();
            notificationSrc.setNotificationTarget (gate_);
            return *this;
          }
        
        /**
         * Builder operation: append a Notification link to the end of this Term's chain.
         * @param targetTerm another Term, which thereby becomes dependent on this Term.
         * @remark the \q targetTerm will be inhibited, until this Term's chain has
         *         been activated and processed up to emitting the inserted `NOTIFY`.
         */
        Term&
        appendNotificationTo (Term& targetTerm)
          {
            Activity& success = alloc_.create (Activity::NOTIFY);
            insert (findTail (invoke_->next), &success);
            targetTerm.expectNotification (success);
            return *this;
          }
        
        /**
         * Insert a self-inhibition to enforce activation is possible only after the
         * scheduled start time. Relevant for Jobs, which are to be triggered by external
         * events, while the actual computation must not be start prior to activating the
         * main chain, even if all prerequisites are already fulfilled.
         * @remark typical example is when a target buffer is known to be available only
         *         after the planned start time and until the planned deadline.
         * @note the actual activation always goes through Activity::dispatch() and the
         *         primary chain is aborted with activity::SKIP. However, since the additional
         *         notification is inserted at a point executed holding the `GroomingToken`,
         *         the `dispatch()` actually happens synchronous and immediately processes
         *         the activated tail-chain in a nested call.
         */
        Term&
        requireDirectActivation()
          {
            Activity& trigger = alloc_.create (Activity::NOTIFY);
            expectNotification (trigger);
            insert (post_, &trigger);
            return *this;
          }
        
      private:
        void
        configureTemplate (Template kind)
          {
            switch (kind) {
              case CALC_JOB:
                setupGate();
                insertWorkBracket();
                break;
              case LOAD_JOB:
                UNIMPLEMENTED ("wiring for async job");
                break;
              case META_JOB:
                /* use the minimal default wiring */
                break;
              default:
                NOTREACHED ("uncovered Activity verb in activation function.");
                break;
              }
          }
        
        
        Activity*
        setupInvocation (Job& job)
          {
            Activity& feed1 = alloc_.create (job.parameter.invoKey.code.w1
                                            ,job.parameter.invoKey.code.w2);
            Activity& feed2 = alloc_.create (Activity::FEED);         ///////////////////////////////////////TICKET #1295 : rework Job parameters to accommodate input / output info required for rendering          
            feed1.next = &feed2;
            
            JobClosure* functor = static_cast<JobClosure*> (job.jobClosure);             ////////////////////TICKET #1287 : fix actual interface down to JobFunctor (after removing C structs)
            REQUIRE (functor);
            Activity& invo  = alloc_.create (*functor
                                            , Time{TimeValue{job.parameter.nominalTime}}
                                            , feed1);             ///////////////////////////////////////////TICKET #1287 : get rid of C-isms in Job descriptor
            return & invo;
          }
        
        Activity*
        setupPost (Time start, Time after, Activity* followUp)
          {
            return & alloc_.create (start,after,followUp);
          }
        
        void
        setupGate()
          {
            if (gate_) return;
            gate_ = & alloc_.create (0, Time{post_->data_.timeWindow.dead});
            ENSURE (gate_);
            ENSURE (gate_->is (Activity::GATE));
            insert (post_, gate_);
          }

        void
        insertWorkBracket()
          {
            Activity& start = alloc_.create (Activity::WORKSTART);
            Activity& stop  = alloc_.create (Activity::WORKSTOP);
            /////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1283 define the "quality" parameter to distinguish observable execution times
            
            insert (gate_? gate_: post_,  &start);
            insert (findTail (start.next), &stop);
          }
        
        
        static void
        insert (Activity* anchor, Activity* target)
          {
            REQUIRE (anchor);
            REQUIRE (target);
            target->next = anchor->next;
            anchor->next = target;
          }
        
        static Activity*
        findTail (Activity* chain)
          {
            REQUIRE (chain);
            while (chain->next)
              chain = chain->next;
            return chain;
          }
      };
    
    
    /** */
    
  }//(End)namespace activity
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_ACTIVITY_TERM_H_*/
