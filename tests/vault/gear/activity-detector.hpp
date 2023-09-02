/*
  ACTIVITY-DETECTOR.hpp  -  test scaffolding to observe activities within the scheduler

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

/** @file activity-detector.hpp
 ** Diagnostic setup to instrument and observe \ref Activity activations.
 ** The [Scheduler](\ref scheduler.hpp) powering the Lumiera render engine
 ** is implemented in terms of Activities, which can be time-bound and depend
 ** on each other. For performance reasons, these _operational atoms_ must be
 ** implemented as a tightly knit network of lightweight POD records without
 ** much indirection. This setup poses a challenge for unit tests and similar
 ** white box testing, due to the lack of a managed platform and any further
 ** means of indirection and extension. As a remedy, a set of preconfigured
 ** _detector Activity records_ is provided, which drop off event log messages
 ** by side effect. These detector probes can be wired in as decorators into
 ** an otherwise valid Activity-Term, allowing to watch and verify patterns
 ** of invocation.
 ** 
 ** # Usage
 ** 
 ** An ActivityDetector instance can be created in local storage to get an arsenal
 ** of probing tools and detectors, which are internally wired to record activation
 ** into an lib::test::EventLog embedded into the ActivityDetector instance. A
 ** _verification DSL_ is provided, internally relying on the building blocks and
 ** the chained-search mechanism known from the EventLog. To distinguish similar
 ** invocations and activations, a common _sequence number_ is maintained within
 ** the ActivityDetector instance, which can be incremented explicitly. All
 ** relevant events also capture the current sequence number as an attribute
 ** of the generated log record.
 ** 
 ** ## Observation tools
 ** - ActivityDetector::buildDiadnosticFun(id) generates a functor object with
 **   _arbitrary signature,_ which records any invocation and arguments.
 **   The corresponding verification matcher is #verifyInvocation(id)
 ** - ActivityDetector::buildMockJobFunctor(id) a JobFunctor implementation
 **   suitably rigged to record invocations and arguments
 ** - ActivityDetector::buildActivationProbe a debugging Activity to record activation
 ** - ActivityDetector::insertActivationTap hooks this Activation-Probe before an
 **   existing Activity-connection, so that passing on the activation can be detected
 ** - ActivityDetector::watchGate rig a `GATE` activity by prepending and appending
 **   an Activation-Probe, so that both incoming and outgoing activations can be traced
 ** - ActivityDetector::executionCtx test setup of the execution environment abstraction
 **   for performing chains of Activities; it provides the expected λ-functions as
 **   instances of ActivityDetctor::DiagnosticFun, so that any invocation is recorded
 ** 
 ** @see SchedulerActivity_test
 ** @see ActivityDetector_test
 ** @see EventLog_test (demonstration of EventLog capbabilities)
 */


#ifndef VAULT_GEAR_TEST_ACTIVITY_DETECTOR_H
#define VAULT_GEAR_TEST_ACTIVITY_DETECTOR_H


#include "vault/common.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/event-log.hpp"

#include "vault/gear/job.h"
#include "vault/gear/activity.hpp"
#include "vault/gear/nop-job-functor.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/meta/variadic-helper.hpp"
#include "lib/meta/function.hpp"
#include "lib/wrapper.hpp"
#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include <functional>
#include <utility>
#include <string>
#include <deque>


namespace vault{
namespace gear {
namespace test {
  
  using std::string;
  using std::function;
  using lib::time::TimeValue;
  using lib::time::Time;
  using lib::time::FSecs;
  using lib::time::Offset;
  using lib::meta::RebindVariadic;
  using util::isnil;
  using std::forward;
  using std::move;
  
  
  namespace {// Diagnostic markers
    const string MARK_INC{"IncSeq"};
    const string MARK_SEQ{"Seq"};
    
    using SIG_JobDiagnostic = void(Time, int32_t);
    const size_t JOB_ARG_POS_TIME = 0;
    
    const string CTX_POST{"CTX-post"};
    const string CTX_WORK{"CTX-work"};
    const string CTX_DONE{"CTX-done"};
    const string CTX_TICK{"CTX-tick"};
    
    Offset POLL_DELAY{FSecs(1)};
    Time SCHED_TIME_MARKER{555,5};  ///< marker value for "current scheduler time" used in tests
  }
  
  class ActivityDetector;
  
  
  /**
   * @internal ongoing evaluation and match of observed activities.
   * @remark this temporary object provides a builder API for creating
   *         chained verifications, similar to the usage of lib::test::EventLog.
   *         Moreover, it is convertible to `bool` to retrieve the verification result.
   */
  class ActivityMatch
    : private lib::test::EventMatch
    {
      using _Parent = lib::test::EventMatch;
      
      ActivityMatch (lib::test::EventMatch&& matcher)
        : _Parent{move (matcher)}
        { }
      
      friend class ActivityDetector;
      
    public:
      // standard copy acceptable
      
      /** final evaluation of the verification query,
       *  usually triggered from the unit test `CHECK()`.
       * @note failure cause is printed to STDERR.
       */
      operator bool()  const { return _Parent::operator bool(); }
      
      
      /* query builder(s) to find a match stepping forwards */
      ActivityMatch& beforeInvocation (string match) { return delegate (&EventMatch::beforeCall, move(match)); }
      // more here...

      /* query builders to find a match stepping backwards */
      ActivityMatch& afterInvocation (string match) { return delegate (&EventMatch::afterCall, move(match)); }
      // more here...
      
      
      /** qualifier: additionally match the function arguments */
      template<typename...ARGS>
      ActivityMatch&
      arg (ARGS const& ...args)
        {
          return delegate (&EventMatch::arg<ARGS...>, args...);
        }
      
      /** qualifier: additionally require the indicated sequence number */
      ActivityMatch&
      seq (uint seqNr)
        {
          _Parent::attrib (MARK_SEQ, util::toString (seqNr));
          return *this;
        }
      
      /** special query to match an increment of the sequence number */
      ActivityMatch&
      beforeSeqIncrement (uint seqNr)
        {
          _Parent::beforeEvent(MARK_INC, util::toString(seqNr));
          return  *this;
        }
      ActivityMatch&
      afterSeqIncrement (uint seqNr)
        {
          _Parent::afterEvent(MARK_INC, util::toString(seqNr));
          return  *this;
        }
      
      /** qualifier: additionally match the nominal time argument of JobFunctor invocation */
      ActivityMatch&
      timeArg (Time const& time)
        {
          return delegate (&EventMatch::argPos<Time const&>, size_t(JOB_ARG_POS_TIME), time);
        }
      
      
    private:
      /** @internal helper to delegate to the inherited matcher building blocks
       *  @note since ActivityMatch can only be created by ActivityDetector,
       *        we can be sure the EventMatch reference returned from these calls
       *        is actually a reference to `*this`, and can thus be downcasted.
       *  */
      template<typename...ARGS>
      ActivityMatch&
      delegate (_Parent& (_Parent::*fun) (ARGS...),  ARGS&& ...args)
        {
          return static_cast<ActivityMatch&> (
                   (this->*fun) (forward<ARGS> (args)...));
        }
    };
  
  
  
  /**
   * Diagnostic context to record and evaluate activations within the Scheduler.
   * The provided tools and detectors are wired back internally, such as to record
   * any observations into an lib::test::EventLog instance. Thus, after performing
   * rigged functionality, the expected activities and their order can be verified.
   * @see ActivityDetector_test
   */
  class ActivityDetector
    : util::NonCopyable
    {
      using EventLog = lib::test::EventLog;
      
      EventLog eventLog_;
      uint invocationSeq_;
      
      /**
       * A Mock functor, logging all invocations into the EventLog
       */
      template<typename RET, typename...ARGS>
      class DiagnosticFun
        {
          using RetVal   = lib::wrapper::ItemWrapper<RET>;
          using ImplFun  = std::function<RET(ARGS...)>;
          
          string id_;
          EventLog* log_;
          uint const* seqNr_;
          ImplFun implFun_;
          RetVal retVal_;
          
        public:
          DiagnosticFun (string id, EventLog& masterLog, uint const& invocationSeqNr)
            : id_{id}
            , log_{&masterLog}
            , seqNr_{&invocationSeqNr}
            , implFun_{}
            , retVal_{}
            {
              retVal_.defaultInit();
            }
          
          /** prepare a response value to return from the mock invocation */
          template<typename VAL>
          DiagnosticFun&&
          returning (VAL&& riggedResponse)
            {
              retVal_ = std::forward<VAL> (riggedResponse);
              return std::move (*this);
            }
          
          /** use the given λ to provide (optional) implementation logic */
          template<class FUN>
          DiagnosticFun&&
          implementedAs (FUN&& customImpl)
            {
              implFun_ = std::forward<FUN> (customImpl);
              return std::move (*this);
            }
          
          // default copyable
          
          /** mock function call operator: logs all invocations */
          RET
          operator() (ARGS ...args)
            {
              log_->call (log_->getID(), id_, args...)
                   .addAttrib (MARK_SEQ, util::toString(*seqNr_));
              return implFun_? implFun_(std::forward<ARGS>(args)...)
                             : *retVal_;
            }
          
          operator string()  const
            {
              return log_->getID()+"."+id_;
            }
        };
      
      /** @internal type rebinding helper */
      template<typename SIG>
      struct _DiagnosticFun
        {
          using Ret = typename lib::meta::_Fun<SIG>::Ret;
          using Args = typename lib::meta::_Fun<SIG>::Args;
          using ArgsX = typename lib::meta::StripNullType<Args>::Seq;    ////////////////////////////////////TICKET #987 : make lib::meta::Types<TYPES...> variadic
          using SigTypes = typename lib::meta::Prepend<Ret, ArgsX>::Seq;
          
          using Type  = typename RebindVariadic<DiagnosticFun, SigTypes>::Type;
        };
      
      using Logger = _DiagnosticFun<void(string)>::Type;
      
      
      /**
       * A Mocked job operation to detect any actual invocation
       */
      class MockJobFunctor
        : public NopJobFunctor
        {
          using MockOp = typename _DiagnosticFun<SIG_JobDiagnostic>::Type;
          
          MockOp mockOperation_;
          
          /** rigged diagnostic implementation of job invocation
           * @note only data relevant for diagnostics is explicitly unpacked
           */
          void
          invokeJobOperation (JobParameter param)  override
            {
              mockOperation_(Time{TimeValue{param.nominalTime}}, param.invoKey.part.a);
            }
          
          string diagnostic()  const override
            {
              return "JobFun-"+string{mockOperation_};
            }
          
        public:
          MockJobFunctor (MockOp mockedJobOperation)
            : mockOperation_{move (mockedJobOperation)}
            { }
        };
      
      
      /**
       * A rigged CALLBACK-Activity to watch passing of activations.
       */
      class ActivityProbe
        : public Activity
        , activity::Hook
        {
          Logger log_;
          
          activity::Proc
          activation ( Activity& thisHook
                     , Time now
                     , void* executionCtx)  override
          {
            REQUIRE (thisHook.is (Activity::HOOK));
            if (data_.callback.arg == 0)
              {// no adapted target; just record this activation
                log_(util::toString(now) + " ⧐ ");
                return activity::PASS;
              }
            else
              {// forward activation to the adapted target Activity
                Activity& target = *reinterpret_cast<Activity*> (data_.callback.arg);
                auto ctx = *static_cast<FakeExecutionCtx*> (executionCtx);
                log_(util::toString(now) + " ⧐ " + util::toString (target));
                return target.activate (now, ctx);
              }
          }
          
          activity::Proc
          notify     ( Activity& thisHook
                     , Time now
                     , void* executionCtx)  override
          {
            REQUIRE (thisHook.is (Activity::HOOK));
            if (data_.callback.arg == 0)
              {// no adapted target; just record this notification
                log_(util::toString(now) + " --notify-↯• ");
                return activity::PASS;
              }
            else
              {// forward notification-dispatch to the adapted target Activity
                Activity& target = *reinterpret_cast<Activity*> (data_.callback.arg);
                auto ctx = *static_cast<FakeExecutionCtx*> (executionCtx);
                log_(util::toString(now) + " --notify-↯> " + util::toString (target));
                return target.notify (now, ctx);
              }
          }
          
        std::string
        diagnostic()  const override
          {
            return "Probe("+string{log_}+")";
          }
          
        public:
          ActivityProbe (string id, EventLog& masterLog, uint const& invocationSeqNr)
            : Activity{*this, 0}
            , log_{id, masterLog, invocationSeqNr}
            { }
            
          ActivityProbe (Activity const& subject, string id, EventLog& masterLog, uint const& invocationSeqNr)
            : Activity{*this, reinterpret_cast<size_t> (&subject)}
            , log_{id, masterLog, invocationSeqNr}
            {
              next = subject.next;
            }
          
          operator string()  const
            {
              return diagnostic();
            }
        };
      
      
      /* ===== Maintain throw-away mock instances ===== */
      
      std::deque<MockJobFunctor> mockOps_{};
      std::deque<ActivityProbe> mockActs_{};
      
      
    public:
      ActivityDetector(string id ="")
        : eventLog_{"ActivityDetector" + (isnil (id)? string{}: "("+id+")")}
        , invocationSeq_{0}
        { }
      
      operator string()  const
        {
          return util::join (eventLog_);
        }
      
      string
      showLog()  const
        {
          return "\n____Event-Log___________________________\n"
               + util::join (eventLog_, "\n")
               + "\n────╼━━━━━━━━╾──────────────────────────"
               ;
        }
      
      void
      clear(string newID)
        {
          if (isnil (newID))
            eventLog_.clear();
          else
            eventLog_.clear (newID);
        }
      
      /** increment the internal invocation sequence number */
      uint
      incrementSeq()
        {
          ++invocationSeq_;
          eventLog_.event (MARK_INC, util::toString(invocationSeq_));
          return invocationSeq_;
        }
      
      uint
      currSeq()  const
        {
          return invocationSeq_;
        }
      
      
      /**
       * Generic testing helper: build a λ-mock, logging all invocations
       * @tparam SIG signature of the functor to be generated
       * @param  id  human readable ID, to designate invocations in the log
       * @return a function object with signature #SIG
       */
      template<typename SIG>
      auto
      buildDiagnosticFun (string id)
        {
          using Functor = typename _DiagnosticFun<SIG>::Type;
          return Functor{id, eventLog_, invocationSeq_};
        }
      
      JobClosure&                         ///////////////////////////////////////////////////////////////////TICKET #1287 : fix actual interface down to JobFunctor (after removing C structs)
      buildMockJobFunctor (string id)
        {
          return mockOps_.emplace_back (
                   buildDiagnosticFun<SIG_JobDiagnostic> (id));
        }
      
      Job
      buildMockJob (string id =""
                   ,Time nominal = lib::test::randTime()
                   ,size_t extra = rand())
        {
          InvocationInstanceID invoKey;
          invoKey.part.a = extra;
          invoKey.part.t = _raw(nominal);
          return Job{buildMockJobFunctor (isnil(id)? "mockJob-"+util::toString(nominal) : id)
                    ,invoKey
                    ,nominal};
        }
      
      /** build a rigged HOOK-Activity to record each invocation */
      Activity&
      buildActivationProbe (string id)
        {
          return mockActs_.emplace_back (id, eventLog_, invocationSeq_);
        }
      
      /** build ActivationProbe to record each activation before passing it to the subject */
      Activity&
      buildActivationTap (Activity const& subject, string id ="")
        {
          return mockActs_.emplace_back (subject
                                        ,isnil(id)? "tap-"+subject.showVerb()+util::showAddr(subject)
                                                  : id
                                        ,eventLog_
                                        ,invocationSeq_);
        }
      
      /** build ActivationProbe to record each activation before passing it to the subject */
      Activity&
      insertActivationTap (Activity*& wiring, string id ="")
        {
          wiring = wiring? & buildActivationTap (*wiring, id)
                         : & buildActivationProbe (isnil(id)? "tail-"+util::showAddr(&wiring) : id);
          return *wiring;
        }
      
      Activity&
      buildGateWatcher (Activity& gate, string id ="")
        {
          insertActivationTap (gate.next, "after-" + (isnil(id)? gate.showVerb()+util::showAddr(gate) : id));
          return buildActivationTap (gate, id);
        }
      
      Activity&
      watchGate (Activity*& wiring, string id ="")
        {
          wiring = wiring? & buildGateWatcher (*wiring, id)
                         : & buildActivationProbe (isnil(id)? "tail-"+util::showAddr(&wiring) : id);
          return *wiring;
        }
      
      
      struct FakeExecutionCtx;
      using SIG_post = activity::Proc(Time, Activity&, FakeExecutionCtx&);
      using SIG_work = void(Time, size_t);
      using SIG_done = void(Time, size_t);
      using SIG_tick = activity::Proc(Time);
      
      /**
       * Mock setup of the execution context for Activity activation.
       * The instance #executionCtx is wired back with the #eventLog_
       * and allows thus to detect and verify all callbacks from the Activities.
       * @note the return value of the #post and #tick functions can be changed
       *       to another fixed response by calling DiagnosticFun::returning
       */
      struct FakeExecutionCtx
        {
          _DiagnosticFun<SIG_post>::Type post;
          _DiagnosticFun<SIG_work>::Type work;
          _DiagnosticFun<SIG_done>::Type done;
          _DiagnosticFun<SIG_tick>::Type tick;
          
          function<Offset()> getWaitDelay = []    { return POLL_DELAY;       };
          function<Time()>   getSchedTime = [this]{ return SCHED_TIME_MARKER;};
          
          FakeExecutionCtx (ActivityDetector& detector)
            : post{detector.buildDiagnosticFun<SIG_post>(CTX_POST).returning(activity::PASS)}
            , work{detector.buildDiagnosticFun<SIG_work>(CTX_WORK)}
            , done{detector.buildDiagnosticFun<SIG_done>(CTX_DONE)}
            , tick{detector.buildDiagnosticFun<SIG_tick>(CTX_TICK).returning(activity::PASS)}
            { }
          
          operator string()  const { return "≺test::CTX≻"; }
        };
      
      FakeExecutionCtx executionCtx{*this};
      
      
      
      ActivityMatch
      verifyInvocation (string fun)
        {
          return ActivityMatch{move (eventLog_.verifyCall(fun))};
        }
      
      ActivityMatch
      ensureNoInvocation (string fun)
        {
          return ActivityMatch{move (eventLog_.ensureNot(fun).locateCall(fun))};
        }
      
      ActivityMatch
      verifySeqIncrement (uint seqNr)
        {
          return ActivityMatch{move (eventLog_.verifyEvent(MARK_INC, util::toString(seqNr)))};
        }
      
      
    private:
    };
  
  
}}} // namespace vault::gear::test
#endif /*VAULT_GEAR_TEST_ACTIVITY_DETECTOR_H*/
