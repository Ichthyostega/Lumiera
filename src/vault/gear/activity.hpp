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
 ** # Performing Activities
 ** 
 ** The Activity data records are »POD with constructor« and can be created
 ** and copied freely; the provided constructors ensure consistent setup,
 ** since the meaning of the _variant data_ depends on the Activity::verb_.
 ** However, in actual usage, the builder functionality of the ActivityLang
 ** is used to generate linked »Activity terms«, [connecting](\ref Activity::next)
 ** chains of Activities in accordance with an implicit execution protocol, which
 ** also forms the base of the Activity _state machine:_
 ** - Activity::activate is invoked only once, when an Activity becomes _active._
 ** - the provided [Execution Context](\ref _verify_usable_as_ExecutionContext)
 **   is connected through several λ-bindings with the actual execution logic,
 **   as provided by »Layer-2« of the Scheduler
 ** - Activity::dispatch is always invoked from within the scheduler and implies
 **   single threaded operation with the ability to mutate the scheduler queue;
 **   typically this happens after de-queuing an Activity from the priority queue
 **   and leads then to Activation of the retrieved Activity; however, it can also
 **   happen _right now_ -- when the indicated time has passed.
 ** - Activity::notify receives a message/trigger from another prerequisite Activity
 ** 
 ** @see SchedulerActivity_test
 ** @see ActivityLang implementation of execution logic
 ** 
 ** @todo WIP-WIP 8/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_ACTIVITY_H_
#define SRC_VAULT_GEAR_ACTIVITY_H_


#include "vault/common.hpp"
#include "vault/gear/job.h"
#include "lib/time/timevalue.hpp"
#include "lib/meta/function.hpp"
#include "lib/util.hpp"

//#include <string>


namespace vault{
namespace gear {
  
  using lib::time::TimeValue;
  using lib::time::TimeVar;
//  using util::isnil;
//  using std::string;
  
  namespace error = lumiera::error;
  
  class Activity;
  
  
  namespace activity { ///< special definitions for the Scheduler activity language
    
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
     * Result instruction from Activity activation.
     * These codes are used by the ActivityLang to
     * coordinate further processing of activity chains.
     */
    enum Proc {PASS  ///< pass on the activation down the chain
              ,SKIP  ///< skip rest of the Activity chain for good
              ,KILL  ///< obliterate the complete Activity-Term and all its dependencies
              ,HALT  ///< abandon this play / render process
              };
    
    
    /**
     * Extension point to invoke a callback from Activity activation
     * @remark referred from the Activity::Verb::HOOK
     * @see ActivityDetector primary usage for testing
     */
    class Hook
      {
      public:
        virtual ~Hook();     ///< this is an interface
        
        /**
         * Callback on activation of the corresponding HOOK-Activity.
         * @param thisHook     the Activity record wired to this hook
         * @param now          current »wall-clock-time« as used by the Scheduler
         * @param executionCtx opaque pointer to the actual execution context
         * @return decision how to proceed with the activation
         * @remark the intended use is to rig this callback based on additional knowledge
         *         regarding the usage context. Through \a thisHook, the follow-up chain
         *         is accessible, and an additional payload data field (`size_t`). Since
         *         the execution context is a _concept,_ it is necessary to know the actual
         *         type of the concrete execution context and cast down in the implementation.
         *         This mechanism is used especially for detecting expected test invocations. 
         */
        virtual Proc activation ( Activity& thisHook
                                , Time now
                                , void* executionCtx)  =0;
        
        /** Callback when dispatching a NOTIFY-Activity to \a thisHook */
        virtual Proc notify     ( Activity& thisHook
                                , Time now
                                , void* executionCtx)  =0;
        
        virtual std::string
        diagnostic()  const
          {
            return "Activity::Hook";
          }
        
        operator std::string()  const
          {
            return diagnostic();
          }
      };
    
    
    /**
     * Definition to emulate a _Concept_ for the *Execution Context*.
     * The Execution Context need to be passed to any Activity _activation;_
     * it provides the _bindings_ for functionality defined only on a conceptual
     * level, and provided by an opaque implementation (actually the Scheduler)
     */
    template<class EXE>
    constexpr void
    _verify_usable_as_ExecutionContext ()
    {
#define ASSERT_MEMBER_FUNCTOR(_EXPR_, _SIG_) \
        static_assert (lib::meta::has_Sig<decltype(_EXPR_), _SIG_>(), \
                       "Execution-Context: " STRINGIFY(_FUN_) " expect function with signature: " STRINGIFY(_SIG_));
      
      
       ASSERT_MEMBER_FUNCTOR (EXE::post, Proc(Time, Activity&, EXE&));
       ASSERT_MEMBER_FUNCTOR (EXE::work, void(Time, size_t));
       ASSERT_MEMBER_FUNCTOR (EXE::done, void(Time, size_t));
       ASSERT_MEMBER_FUNCTOR (EXE::tick, Proc(Time));
       ASSERT_MEMBER_FUNCTOR (EXE::wait, Time(Time));

      
#undef ASSERT_MEMBER_FUNCTOR
    }
    
  }//(End)namespace activity
  
  
  
  
  /*********************************************//**
   * Term to describe an Activity,
   * to happen within the Scheduler's control flow.
   * @note Activity is a »POD with constructor«
   *       - trivially *destructible*
   *       - trivially *copyable*
   *       - standard layout
   */
  class Activity
    {
      using Instant = activity::Instant;
      
    public:
      /**  All possible kinds of activities */
      enum Verb {INVOKE     ///< dispatch a JobFunctor into a worker thread
                ,WORKSTART  ///< signal start of some processing and transition *grooming mode* ⟼ *work mode
                ,WORKSTOP   ///< correspondingly signal end of some processing
                ,NOTIFY     ///< push a message to another Activity
                ,GATE       ///< probe window + count-down; activate next Activity, else re-schedule
                ,POST       ///< post a message providing a chain of further time-bound Activities
                ,FEED       ///< supply additional payload data for a preceding Activity
                ,HOOK       ///< invoke an _extension point_ through the activity::Hook interface
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
          Instant instant;                ///////////////////////////////////////////////////////////////////TICKET #1317 : what time do we actually need to transport here, in addition to the invocation time (added automatically)?
          size_t  quality;
        };
      
      /** Extension point to invoke */
      struct Callback
        {
          activity::Hook* hook;
          size_t  arg;
        };
      
      /** Access gate condition to evaluate */
      struct Condition
        {
          int     rest;     ///< alive while rest > 0
          Instant dead;     ///< alive while time < dead
          
          bool isDead (Time now)  const { return dead <= now;}
          bool isHold ()          const { return rest > 0;   }
          bool isFree (Time now)  const { return not (isHold() or isDead(now)); }
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
            Callback callback;
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
      
      Activity (JobFunctor& job, Time nominalTime, Activity& feed)  noexcept
        : Activity{INVOKE}
        {
          data_.invocation.task = &job;
          data_.invocation.time = nominalTime;
          next = &feed;
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
      
      Activity (activity::Hook& callback, size_t arg)  noexcept
        : Activity{HOOK}
        {
          data_.callback.hook = &callback;
          data_.callback.arg = arg;
        }
      
      Activity()  noexcept
        : Activity{TICK}
        { }
      
      /// diagnostic representation
      operator std::string()  const;
      
      std::string showVerb()  const;
      std::string showData()  const;
      
      
      /********************************************************//**
       * Core Operation: _Activate_ and _perform_ this Activity.
       * @tparam EXE concrete binding for the _execution context_
       * @param now current _»wall clock time« (time used for scheduling)
       */
      template<class EXE>
      activity::Proc activate (Time now, EXE& executionCtx);
      
      template<class EXE>
      activity::Proc dispatch (Time now, EXE& executionCtx);
      
      template<class EXE>
      activity::Proc notify   (Time now, EXE& executionCtx);
      
      
    private:
      void
      setDefaultArg (Verb verb)  noexcept
        {
          data_ = {0,0};
          switch (verb) {
            case INVOKE:
              data_.invocation.time = Time::ANYTIME;
              break;
            case WORKSTART:
            case WORKSTOP:
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
      
      
      activity::Proc
      invokeFunktor (Time)  noexcept
        {
          REQUIRE (verb_ == INVOKE);
          REQUIRE (next);
          REQUIRE (next->verb_ == FEED);
          REQUIRE (next->next);
          REQUIRE (next->next->verb_ == FEED);
          REQUIRE (data_.invocation.task);
          
          JobClosure& functor = static_cast<JobClosure&> (*data_.invocation.task);      /////////////////////TICKET #1287 : fix actual interface down to JobFunctor (after removing C structs)
          lumiera_jobParameter param;
          param.nominalTime = _raw(Time{data_.invocation.time});
          param.invoKey.part.a = next->data_.feed.one;
          param.invoKey.part.b = next->data_.feed.two;
                           //////////////////////////////////////////////////////////////////////////////////TICKET #1295 : rework Job parameters to accommodate input / output info required for rendering          
          try {
              functor.invokeJobOperation (param);
            }
          ON_EXCEPTION_RETURN (activity::HALT, "Render Job invocation");
          //
          return activity::PASS;
        }
      
      
      template<class EXE>
      activity::Proc
      signalStart (Time now, EXE& executionCtx)
        {
          executionCtx.work (now, data_.timing.quality);
          return activity::PASS;
        }
      
      template<class EXE>
      activity::Proc
      signalStop (Time now, EXE& executionCtx)
        {
          executionCtx.done (now, data_.timing.quality);
          return activity::PASS;
        }
      
      template<class EXE>
      activity::Proc
      dispatchNotify (Time now, EXE& executionCtx)
        {
          return executionCtx.post (now, *this, executionCtx);
        }
      
      template<class EXE>
      activity::Proc
      notifyTarget (Time now, EXE& executionCtx)
        {
          REQUIRE (NOTIFY == verb_);
          REQUIRE (data_.notification.target);
          return data_.notification.target->notify (now, executionCtx);
        }
      
      template<class EXE>
      activity::Proc
      checkGate (Time now, EXE& executionCtx)
        {
          REQUIRE (GATE == verb_);
          if (data_.condition.isDead(now))  // beyond deadline
            return activity::SKIP;
          if (data_.condition.isHold())     // prerequisite count not(yet) fulfilled -> spin (=re-invoke later)
            return dispatchSelfDelayed (now, executionCtx);
          else
            return activity::PASS;
        }
      
      template<class EXE>
      activity::Proc
      receiveGateNotification (Time now, EXE& executionCtx)
        {
          REQUIRE (GATE == verb_);
          if (data_.condition.rest > 0)
            {
              --data_.condition.rest;
              // maybe the Gate has been opened by this notification?
              if (data_.condition.isFree(now))
                {//yes => activate gated chain but lock redundant invocations
                  data_.condition.dead = Time::MIN;
                  return postChain (now, executionCtx);
            }   }
          return activity::PASS;
        }
      
      template<class EXE>
      activity::Proc
      dispatchSelf (Time when, EXE& executionCtx)
        {
          return executionCtx.post (when, *this, executionCtx);
        }
      
      template<class EXE>
      activity::Proc
      dispatchSelfDelayed (Time now, EXE& executionCtx)
        {
          dispatchSelf (executionCtx.wait(now), executionCtx);
          return activity::SKIP;
        }
      
      template<class EXE>
      activity::Proc
      postChain (Time now, EXE& executionCtx)
        {
          REQUIRE (next);
          return executionCtx.post (now, *next, executionCtx);
        }
      
      template<class EXE>
      activity::Proc
      callHook (Time now, EXE& executionCtx)
        {
          return data_.callback.hook? data_.callback.hook->activation(*this, now, &executionCtx)
                                    : activity::PASS;
        }
      
      template<class EXE>
      activity::Proc
      notifyHook (Time now, EXE& executionCtx)
        {
          return data_.callback.hook? data_.callback.hook->notify (*this, now, &executionCtx)
                                    : activity::PASS;
        }
      
      template<class EXE>
      activity::Proc
      doTick (Time now, EXE& executionCtx)
        {
          return executionCtx.tick (now);
        }
    };
  
  
  
  
  /**
   * @remark this defines the _Activity state machine_ and implements
   *         behaviour in dependency of the kind of Activity::Verb.
   *         Actual implementation defined effects in the Scheduler
   *         are abstracted as \a executionCtx:
   *         - `post` : dispatch the given Activity with start time
   *         - `work` : drop the `GroomingToken` and start processing
   *         - `done` : record the end time of a media computation
   *         - `tick` : regular maintenance hook
   * @return activity::Proc indication how to proceed with execution
   *         - activity::PASS continue with regular processing of `next`
   *         - activity::SKIP ignore the rest of the chain, look for new work
   *         - activity::KILL abort this complete Activity term (play change)
   *         - activity::HALT serious problem, stop the Scheduler
   */
  template<class EXE>
  activity::Proc
  Activity::activate (Time now, EXE& executionCtx)
  {
    activity::_verify_usable_as_ExecutionContext<EXE>();
    
    switch (verb_) {
      case INVOKE:
        return invokeFunktor (now);
      case WORKSTART:
        return signalStart (now, executionCtx);
      case WORKSTOP:
        return signalStop (now, executionCtx);
      case NOTIFY:
        return dispatchNotify (now, executionCtx);
      case GATE:
        return checkGate (now, executionCtx);
      case POST:
        return dispatchSelf (now, executionCtx);
      case FEED:
        return activity::PASS;
      case HOOK:
        return callHook (now, executionCtx);
      case TICK:
        return doTick (now, executionCtx);
      default:
        NOTREACHED ("uncovered Activity verb in activation function.");
      }
  }
  

  /**
   * Entrance point for an activation, which has been dispatched indirectly
   * through the dispatch and/or priority queue; typically this is achieved
   * by invoking the `post`-λ on the \a executionCtx, or by _activating_
   * a `POST`-Activity. Control flow passing here has acquired the `GroomingToken`
   * and can thus assume single threaded execution until `WORKSTART`.
   * @note special twist for the `NOTIFY`-Activity: it is not _activated_
   *       itself, rather the #notify operation is invoked on its target(`next`);
   *       this is necessary since a notification passes control-flow outside
   *       the regular linear `next`-chain; when a `NOTIFY` is _activated,_
   *       it will `post()` itself to acquire the `GroomingToken` and then
   *       invoke this dispatch() function to pass the notification
   */
  template<class EXE>
  activity::Proc
  Activity::dispatch (Time now, EXE& executionCtx)
  {
    activity::_verify_usable_as_ExecutionContext<EXE>();
    
    switch (verb_) {
      case NOTIFY:
        return notifyTarget (now, executionCtx);
      case POST:
      case FEED:      // signal just to proceed with next...
        return activity::PASS;
      default:
        return activate (now, executionCtx);
      }
  }
  
  
  /**
   * Special operation to receive a message or trigger from some other Activity.
   * Notably this is used to implement _gating_ to wait for prerequisites; when
   * a notification is passed to a `GATE`-Activity, the embedded counter is
   * decremented; after all prerequisites are „checked off“ this way, the
   * Activity-chain behind the Gate is activated.
   */
  template<class EXE>
  activity::Proc
  Activity::notify (Time now, EXE& executionCtx)
  {
    activity::_verify_usable_as_ExecutionContext<EXE>();
    
    switch (verb_) {
      case GATE:
        return receiveGateNotification (now, executionCtx);
      case HOOK:
        return notifyHook (now, executionCtx);
      case POST:
      case FEED:
        return postChain (now, executionCtx);
      default:
        return dispatchSelfDelayed (now, executionCtx);
      }     // Fallback: self-re-dispatch for async execution
  }
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_ACTIVITY_H_*/
