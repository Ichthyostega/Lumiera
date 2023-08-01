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
 ** a otherwise valid Activity-Term, allowing to watch and verify patterns
 ** of invocation -- which might even happen concurrently.
 **
 ** @todo WIP-WIP-WIP 7/2023 right now this is a rather immature attempt
 **       towards a scaffolding to propel the build-up of the scheduler.
 ** @see SchedulerActivity_test
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
//#include "vault/gear/job.h"
//#include "vault/real-clock.hpp"
//#include "lib/allocator-handle.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/diff/gen-node.hpp"
//#include "lib/linked-elements.hpp"
#include "lib/meta/variadic-helper.hpp"
#include "lib/wrapper.hpp"
#include "lib/format-util.hpp"
//#include "lib/itertools.hpp"
//#include "lib/depend.hpp"
#include "lib/util.hpp"

#include <functional>
#include <utility>
#include <string>
//#include <tuple>
//#include <map>


namespace vault{
namespace gear {
namespace test {
  
  using std::string;
//  using std::make_tuple;
//  using lib::diff::GenNode;
//  using lib::diff::MakeRec;
//  using lib::time::TimeValue;
//  using lib::time::Time;
//  using lib::HashVal;
  using lib::meta::RebindVariadic;
  using util::isnil;
//  using util::isSameObject;
//  using fixture::Segmentation;
//  using vault::RealClock;
//  using vault::gear::Job;
//  using vault::gear::JobClosure;
  
  
  
  /**
   * Diagnostic context to record and evaluate activations within the Scheduler.
   * @todo WIP-WIP-WIP 7/23 a new loopy hope
   */
  class ActivityDetector
    : util::NonCopyable
    {
      using EventLog = lib::test::EventLog;
      
      EventLog eventLog_;
      
      /**
       * A Mock functor, logging all invocations into the EventLog
       */
      template<typename RET, typename...ARGS>
      class DiagnosticFun
        {
          using RetVal   = lib::wrapper::ItemWrapper<RET>;
          
          string id_;
          EventLog* log_;
          RetVal retVal_;
          
        public:
          DiagnosticFun (string id, EventLog& masterLog)
            : id_{id}
            , log_{&masterLog}
            , retVal_{}
            { }
          
          /** prepare a response value to return from the mock invocation */
          DiagnosticFun&&
          returning (RET&& riggedResponse)
            {
              retVal_ = std::forward<RET> (riggedResponse);
              return std::move (*this);
            }
          
          /** mock function call operator: logs all invocations */
          RET
          operator() (ARGS const& ...args)
            {
              log_->call (log_->getID(), id_, args...);
              return *retVal_;
            }
        };
      
      
    public:
      ActivityDetector(string id)
        : eventLog_{"ActivityDetector" + (isnil (id)? string{}: "("+id+")")}
        { }
      
      operator string()  const
        {
          return util::join (eventLog_);
        }
      
      void
      clear(string newID)
        {
          if (isnil (newID))
            eventLog_.clear();
          else
            eventLog_.clear (newID);
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
          using Ret = typename lib::meta::_Fun<SIG>::Ret;
          using Args = typename lib::meta::_Fun<SIG>::Args;
          using ArgsX = typename lib::meta::StripNullType<Args>::Seq;    ////////////////////////////////////TICKET #987 : make lib::meta::Types<TYPES...> variadic
          using SigTypes = typename lib::meta::Prepend<Ret, ArgsX>::Seq;
          using Functor  = typename RebindVariadic<DiagnosticFun, SigTypes>::Type;
          
          return Functor{id, eventLog_};
        }
      
    private:
    };
  
  
}}} // namespace vault::gear::test
#endif /*VAULT_GEAR_TEST_ACTIVITY_DETECTOR_H*/
