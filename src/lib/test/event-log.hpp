/*
  EVENT-LOG.hpp  -  test facility to verify the occurrence of expected events

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file event-log.hpp
 ** Support for verifying the occurrence of events from unit tests.
 ** Typically used within special rigging and instrumentation for tests,
 ** the [EventLog] allows to record invocations and similar events. It is
 ** implemented as a "PImpl" to allow sharing of logs. The front-end used
 ** for access offers a query facility, so the test code may express some
 ** expected patterns of incidence and verify match or non-match.
 ** 
 ** Failure of match deliberately throws an assertion failure, in order to
 ** deliver a precise indication what part of the condition failed.
 ** 
 ** @todo as of 11/2015 this is complete WIP-WIP-WIP
 ** 
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef LIB_TEST_EVENT_LOG_H
#define LIB_TEST_EVENT_LOG_H


#include "lib/error.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/iter-adapter-stl.hpp"
//#include "lib/time/timevalue.hpp"
#include "lib/format-string.hpp"
#include "lib/format-util.hpp"
#include "lib/diff/record.hpp"
#include "lib/util.hpp"

//#include <boost/lexical_cast.hpp>
#include <vector>
#include <string>



namespace lib {
namespace test{
  namespace error = lumiera::error;
  
//  using lib::Literal;
  using std::string;
  using util::contains;
  using util::isnil;
  using util::_Fmt;
//  using std::rand;
  
  /**
   * @internal ongoing evaluation and match within an [EventLog].
   * @throws error::Fatal when the expected match fails (error::LUMIERA_ERROR_ASSERTION)
   */
  class EventMatch
    {
      using Entry = lib::diff::Record<string>;
      using Log   = std::vector<Entry>;
      using Iter  = lib::RangeIter<Log::const_iterator>;
      using Filter = ExtensibleFilterIter<Iter>;
      
      /** match predicate evaluator */
      Filter solution_;
      
      /** record last match for diagnostics */
      string lastMatch_;
      
      /** core of the evaluation machinery:
       * apply a filter predicate and then pull
       * through the log to find a acceptable entry
       */
      bool
      eval()
        {
          return !isnil (solution_);
        }
      
      /** this is actually called after each refinement
       * of the filter and matching conditions. The effect is
       * to search for an (intermediary) solution right away
       * and to throw an assertion failure as soon as some
       * condition can not be satisfied. Rationale is to
       * indicate the point where a chained match fails
       * @par matchSpec diagnostics description of the predicate just being added
       * @par rel indication of the searching relation / direction
       * @throws error::Fatal ([assertion failure][error::LUMIERA_ERROR_ASSERTION]
       *         when the filtering condition built up thus far can not be
       *         satisfied at all on the current event log contents
       */
      void
      enforce (string matchSpec, Literal rel = "after")
        {
          if (!eval())
            throw error::Fatal(_Fmt("Failed to %s %s %s")
                                   % matchSpec % rel % lastMatch_
                              , error::LUMIERA_ERROR_ASSERTION);
          
          lastMatch_ = matchSpec+" @ "+string(*solution_);
        }
      
      /** @internal for creating EventLog matchers */
      EventMatch(Iter&& srcSeq)
        : solution_(srcSeq)
        , lastMatch_("HEAD "+ solution_->get("ID"))
        { }
      
      friend class EventLog;
      
      
      /* == elementary matchers == */
      
      auto
      find (string match)
        {
          return [=](Entry const& entry)
                    {
                      return contains (string(entry), match);
                    };
        }
      
    public:
      /**
       * find a match (substring match) of the given text
       * in an EventLog entry after the current position
       * @remarks the name of this junctor might seem counter intuitive;
       *          it was chosen due to expected DSL usage: `log.verify("α").before("β")`.
       *          Operationally this means first to find a Record matching the substring "α"
       *          and then to forward from this point until hitting a record to match "β". 
       */
      EventMatch&
      before (string match)
        {
          solution_.setNewFilter(find(match));
          enforce ("match(\""+match+"\")");
          return *this;
        }
      
      EventMatch&
      beforeMatch (string regExp)
        {
          UNIMPLEMENTED("process combined relational regular expression match");
        }
      
      EventMatch&
      beforeEvent (string match)
        {
          UNIMPLEMENTED("process combined relational match");
        }
      
      EventMatch&
      beforeCall (string match)
        {
          UNIMPLEMENTED("process combined relational match");
        }
      
      EventMatch&
      after (string match)
        {
          UNIMPLEMENTED("process combined relational match backwards");
        }
      
      EventMatch&
      afterMatch (string regExp)
        {
          UNIMPLEMENTED("process combined relational regular expression match backwards");
        }
      
      EventMatch&
      afterEvent (string match)
        {
          UNIMPLEMENTED("process combined relational match backwards");
        }
      
      EventMatch&
      afterCall (string match)
        {
          UNIMPLEMENTED("process combined relational match backwards");
        }
      
      EventMatch&
      arg ()
        {
          UNIMPLEMENTED("process match on no-arg call argument list");
        }
      
      template<typename... MORE>
      EventMatch&
      arg (string match, MORE...args)
        {
          UNIMPLEMENTED("process match on call argument list");
        }
      
      template<typename... MORE>
      EventMatch&
      argMatch (string regExp, MORE...args)
        {
          UNIMPLEMENTED("process regular expression match on call argument list");
        }
      
      EventMatch&
      on (string sourceID)
        {
          UNIMPLEMENTED("process additional filter on source of log entry");
        }
    };
  
  
  /**
   * Helper to log and verify the occurrence of events.
   */
  class EventLog
    {
      using Entry = lib::diff::Record<string>;
      using Log   = std::vector<Entry>;
      using Iter  = lib::RangeIter<Log::const_iterator>;
      
      string logID_;
      Log log_;
      
      void
      log (std::initializer_list<string> const&& ili)
        {
          log_.emplace_back(ili);
        }
      
    public:
      explicit
      EventLog (string logID)
        : logID_(logID)
        {
          log({"type=EventLogHeader", "ID="+logID_});
        }
      
      template<class X>
      explicit
      EventLog (const X *const obj)
        : EventLog(idi::instanceTypeID (obj))
        { }
      
      
      // standard copy operations acceptable
      
      
      EventLog&
      join (EventLog& otherLog)
        {
          UNIMPLEMENTED("log join");
          return *this;
        }
      
      
      
      /* ==== Logging API ==== */
      
      EventLog&
      event (string text)
        {
          log({"type=event", text});
          return *this;
        }
      
      
      
      /* ==== Iteration ==== */
      
      bool
      empty()  const
        {
          return 1 >= log_.size();
        }
      
      
      typedef Iter const_iterator;
      typedef const Entry value_type;
      
      const_iterator begin() const { return Iter(log_.begin(), log_.end()); }
      const_iterator end()   const { return Iter(); }
      
      friend const_iterator begin (EventLog const& log) { return log.begin(); }
      friend const_iterator end   (EventLog const& log) { return log.end(); }
      
      
      
      
      /* ==== Query/Verification API ==== */
      
      EventMatch
      verify (string match)
        {
          EventMatch matcher(this->begin());
          matcher.before (match);
          return matcher;
        }
      
      EventMatch
      verifyMatch (string regExp)
        {
          UNIMPLEMENTED("start matching sequence for regular expression match");
        }
      
      EventMatch
      verifyEvent (string match)
        {
          UNIMPLEMENTED("start matching sequence");
        }
      
      EventMatch
      verifyCall (string match)
        {
          UNIMPLEMENTED("start matching sequence");
        }
      
      EventMatch
      ensureNot (string match)
        {
          UNIMPLEMENTED("start matching sequence");
        }
    };
  
  
  
}} // namespace lib::test
#endif /*LIB_TEST_EVENT_LOG_H*/
