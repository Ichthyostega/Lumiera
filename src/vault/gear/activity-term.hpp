/*
  ACTIVITY-TERM.hpp  -  definition language framework for scheduler activities

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
 ** # Wiring schemes
 ** 
 ** The »mechanics« of render activities are considered finite, and cohesive low-level code.
 ** Thus only a [limited selection](\ref activity::Term::Template) of wiring schemes is provided,
 ** offering just enough leeway to implement the foreseeable variations in functionality.
 ** The underlying operational sequence is as follows
 ** - as _entrance point,_ the complete chain of activities is _posted,_
 **   thereby defining a start time and deadline window; moreover this indirection ensures
 **   that the following execution until `WORKSTART` happens in »management mode«, which
 **   is performed single threaded and may alter the scheduler queue (enqueue, dequeue).
 ** - optionally, a `GATE` can enforce the deadline and block until a predetermined number
 **   of prerequisite notifications has been received
 ** - next follows the actual job invocation, which is bracketed in `WORKSTART` and `WORKSTOP`
 ** - the actual job invocation relies on _two additional_ `FEED` records to hold parameters
 ** - at the end of the chain, optionally a `NOTIFY` can be appended and linked to the `GATE`
 **   of a follow-up job, which thereby becomes dependent on this actual job's completion.
 ** 
 ** While the regular media computation job employs this complete scheme, the Template::META_JOB
 ** uses the minimal setup, comprised only of post and invocation (since the meta job is used for
 ** planning further jobs and thus entirely in »management mode« and without further inhibitions
 ** or notifications). Another variation is presented by the Template::LOAD_JOB, which enables
 ** the integration of asynchronous IO. The actual JobFunctor in this case is assumed just to
 ** dispatch a long running external IO operation and then return quickly. Upon completion
 ** of the external work, a _callback_ will be activated, which must be wired such as to
 ** re-activate the rest of the activities. Consequently for this mode of operation the
 ** activity chain becomes _severed_ into two segments, cutting right behind the
 ** last `FEED`-Activity (i.e. after issuing the Job) but before the `WORKSTOP`.
 ** The callback then dispatches the `WORKSTOP` and possibly a chained `NOTIFY`.
 ** 
 ** @see SchedulerActivity_test
 ** @see activity-lang.hpp Entrance point to Activity definition
 ** @see activity.hpp definition of verbs
 **
 */


#ifndef SRC_VAULT_GEAR_ACTIVITY_TERM_H_
#define SRC_VAULT_GEAR_ACTIVITY_TERM_H_


#include "vault/gear/activity.hpp"
#include "vault/gear/block-flow.hpp"
#include "vault/gear/job.h"
#include "lib/time/timevalue.hpp"

#include <string>
#include <utility>


namespace vault{
namespace gear {
  
  using lib::time::Time;
  using lib::time::TimeValue;
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
        Activity* callback_{nullptr};   ///< @note indicates also this is an async job
        
        
      public:
        enum Template {CALC_JOB  ///< scheme for a synchronous media calculation job
                      ,LOAD_JOB  ///< scheme for an asynchronous data retrieval job
                      ,META_JOB  ///< scheme for planning and organisational job
                      };
        
        explicit
        Term (AllocHandle&& allocHandle, Template kind, Time start, Time dead, Job job)
          : alloc_{move (allocHandle)}
          , invoke_{setupInvocation (job)}
          , post_{setupPost (start,dead, invoke_)}
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
         * @return re-entrance point for completion after async IO.
         * @remark initiate follow-up processing by ActivityLang::dispatchChain().
         */
        Activity&
        callback()
          {
            REQUIRE (callback_, "Activity Term properly configured for async IO");
            return *callback_;
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
        expectNotification (Activity& notificationSrc, bool unlimitedTime =false)
          {
            REQUIRE (notificationSrc.is (Activity::NOTIFY));
            setupGate();
            gate_->incDependencies();
            Time triggerTimeStart{unlimitedTime? Time::ANYTIME : post_->data_.timeWindow.life};
            notificationSrc.setNotificationTarget (gate_, triggerTimeStart);
            return *this;
          }
        
        /**
         * Builder operation: append a Notification link to the end of this Term's chain.
         * @param targetTerm another Term, which thereby becomes dependent on this Term.
         * @remark the \q targetTerm will be inhibited, until this Term's chain has
         *         been activated and processed up to emitting the inserted `NOTIFY`.
         */
        Term&
        appendNotificationTo (Term& targetTerm, bool unlimitedTime =false)
          {
            Activity& success = alloc_.create (Activity::NOTIFY);
            insert (findTail (callback_? callback_ : invoke_), &success);
            targetTerm.expectNotification (success, unlimitedTime);
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
         * @deprecated 12/23 this feature seemed necessary in the first implementation,
         *         yet after integration and follow-up refactorings (NOTIFY-handling) it
         *         turned out both superfluous and potentially dangerous, since it creates
         *         additional management work and possible contention on the Grooming-Token.
         *         The way NOTIFY-activities are handled now already ensures that they are
         *         activated only after their target's start time.
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
                insertWorkBracket();
                severAsyncChain();
                break;
              case META_JOB:
                /* use the minimal default wiring */
                break;
              default:
                NOTREACHED ("unknown wiring scheme for Render Jobs.");
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
        setupPost (Time start, Time dead, Activity* followUp)
          {
            return & alloc_.create (start,dead,followUp);
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
        
        void
        severAsyncChain()
          {
            if (callback_) return;
            Activity& cut = *invoke_->next->next;
            REQUIRE (cut.is (Activity::FEED));
            callback_ = cut.next;
            cut.next = nullptr;
            ENSURE (callback_);
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
    
  }//(End)namespace activity
}}// namespace vault::gear
#endif /*SRC_VAULT_GEAR_ACTIVITY_TERM_H_*/
