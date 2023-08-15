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
 ** of invocation -- which might even happen concurrently.
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
 ** 
 ** @todo WIP-WIP-WIP 8/2023 gradually gaining traction.
 ** @see SchedulerActivity_test
 ** @see EventLog_test (demonstration of EventLog capbabilities)
 */


#ifndef VAULT_GEAR_TEST_ACTIVITY_DETECTOR_H
#define VAULT_GEAR_TEST_ACTIVITY_DETECTOR_H


#include "vault/common.hpp"
//#include "lib/test/test-helper.hpp"
#include "lib/test/event-log.hpp"

//#include "steam/play/dummy-play-connection.hpp"
//#include "steam/fixture/node-graph-attachment.hpp"
//#include "steam/fixture/segmentation.hpp"
//#include "steam/mobject/model-port.hpp"
//#include "steam/engine/dispatcher.hpp"
//#include "steam/engine/job-ticket.hpp"
#include "vault/gear/job.h"
#include "vault/gear/activity.hpp"
#include "vault/gear/nop-job-functor.hpp"
//#include "vault/real-clock.hpp"
//#include "lib/allocator-handle.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/diff/gen-node.hpp"
//#include "lib/linked-elements.hpp"
#include "lib/meta/variadic-helper.hpp"
#include "lib/meta/function.hpp"
#include "lib/wrapper.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"
//#include "lib/itertools.hpp"
//#include "lib/depend.hpp"
#include "lib/util.hpp"

#include <functional>
#include <utility>
#include <string>
#include <deque>
//#include <tuple>
//#include <map>


namespace vault{
namespace gear {
namespace test {
  
  using std::string;
//  using std::make_tuple;
//  using lib::diff::GenNode;
//  using lib::diff::MakeRec;
  using lib::time::TimeValue;
//  using lib::time::Time;
//  using lib::HashVal;
  using lib::meta::RebindVariadic;
  using util::isnil;
  using std::forward;
  using std::move;
//  using util::isSameObject;
//  using fixture::Segmentation;
//  using vault::RealClock;
//  using vault::gear::Job;
//  using vault::gear::JobClosure;
  
  
  namespace {// Event markers
    const string MARK_INC{"IncSeq"};
    const string MARK_SEQ{"Seq"};
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
      
      
//      EventMatch& locate (string match);
//      EventMatch& locateMatch (string regExp);
//      EventMatch& locateEvent (string match);
//      EventMatch& locateEvent (string classifier, string match);
//      EventMatch& locateCall (string match);
//      
//      
//      /* query builders to find a match stepping forwards */
//      
//      EventMatch& before (string match);
//      EventMatch& beforeMatch (string regExp);
//      EventMatch& beforeEvent (string match);
//      EventMatch& beforeEvent (string classifier, string match);
      ActivityMatch& beforeInvocation (string match) { return delegate (&EventMatch::beforeCall, move(match)); }
//      
//      
//      /* query builders to find a match stepping backwards */
//      
//      EventMatch& after (string match);
//      EventMatch& afterMatch (string regExp);
//      EventMatch& afterEvent (string match);
//      EventMatch& afterEvent (string classifier, string match);
      ActivityMatch& afterInvocation (string match) { return delegate (&EventMatch::afterCall, move(match)); }
      
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
   * @todo WIP-WIP-WIP 8/23 gradually building the verification tools needed...
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
          
          string id_;
          EventLog* log_;
          uint const* seqNr_;
          RetVal retVal_;
          
        public:
          DiagnosticFun (string id, EventLog& masterLog, uint const& invocationSeqNr)
            : id_{id}
            , log_{&masterLog}
            , seqNr_{&invocationSeqNr}
            , retVal_{}
            { }
          
          /** prepare a response value to return from the mock invocation */
          template<typename VAL>
          DiagnosticFun&&
          returning (VAL&& riggedResponse)
            {
              retVal_ = std::forward<VAL> (riggedResponse);
              return std::move (*this);
            }
          
          /** mock function call operator: logs all invocations */
          RET
          operator() (ARGS const& ...args)
            {
              log_->call (log_->getID(), id_, args...)
                   .addAttrib (MARK_SEQ, util::toString(*seqNr_));
              return *retVal_;
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
      
      
      /**
       * A Mocked job operation to detect any actual invocation
       */
      class MockJobFunctor
        : public NopJobFunctor
        {
        public:
          using SIG_Diagnostic = void(TimeValue, int32_t);
          
        private:
          using MockOp = typename _DiagnosticFun<SIG_Diagnostic>::Type;
          
          MockOp mockOperation_;
          
          /** rigged diagnostic implementation of job invocation
           * @note only data relevant for diagnostics is explicitly unpacked
           */
          void
          invokeJobOperation (JobParameter param)  override
            {
              mockOperation_(TimeValue{param.nominalTime}, param.invoKey.part.a);
            }
          
        public:
          MockJobFunctor (MockOp mockedJobOperation)
            : mockOperation_{move (mockedJobOperation)}
            { }
        };
      
      
      /* ===== Maintain throw-away mock instances ===== */
      
      std::deque<MockJobFunctor> mockOps_{};
      
      
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
      operator++()
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
                   buildDiagnosticFun<MockJobFunctor::SIG_Diagnostic> (id));
        }
      
      
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
