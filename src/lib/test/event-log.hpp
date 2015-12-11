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
 ** implemented as a "PImpl" to allow sharing of logs, which helps to trace
 ** events from transient UI elements and from destructor code. The front-end
 ** used for access offers a query facility, so the test code may express some
 ** expected patterns of incidence and verify match or non-match.
 ** 
 ** Failure of match prints a detailed trace message to `STDERR`, in order to
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
#include "lib/iter-adapter.hpp"
#include "lib/iter-cursor.hpp"
#include "lib/format-string.hpp"
#include "lib/format-util.hpp"
#include "lib/diff/record.hpp"
#include "lib/util.hpp"

//#include <boost/lexical_cast.hpp>
#include <iostream>
#include <memory>
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
      using Iter  = lib::IterCursor<Log::const_iterator>;
      using Filter = ExtensibleFilterIter<Iter>;
      
      /** match predicate evaluator */
      Filter solution_;
      
      /** record last match for diagnostics */
      string lastMatch_;
      
      /** support for positive and negative queries.
       * @note negative queries enforced in dtor */
      bool look_for_match_;
      
      string violation_;
      
      /** core of the evaluation machinery:
       * apply a filter predicate and then pull
       * through the log to find a acceptable entry
       */
      bool
      foundSolution()
        {
          return !isnil (solution_);
        }
      
      /** this is actually called after each refinement
       * of the filter and matching conditions. The effect is to search
       * for an (intermediary) solution right away and to mark failure
       * as soon as some condition can not be satisfied. Rationale is to
       * indicate the point where a chained match fails
       * @see ::operator bool() for extracting the result
       * @par matchSpec diagnostics description of the predicate just being added
       * @par rel indication of the searching relation / direction
       */
      void
      evaluateQuery (string matchSpec, Literal rel = "after")
        {
          if (look_for_match_ and not isnil (violation_)) return;
             // already failed, no further check necessary
          
          if (foundSolution())
            {
              lastMatch_ = matchSpec+" @ "+string(*solution_)
                         + (isnil(lastMatch_)? ""
                                             : "\n.."+rel+" "+lastMatch_);
              if (not look_for_match_)
                violation_ = "FOUND at least "+lastMatch_;
            }
          else
            {
              if (look_for_match_)
                violation_ = "FAILED to "+matchSpec
                           + "\n.."+rel
                           + " "+lastMatch_;
              else
                violation_ = "";
            }
        }
      
      
      /** @internal for creating EventLog matchers */
      EventMatch(Log const& srcSeq)
        : solution_(Iter(srcSeq))
        , lastMatch_("HEAD "+ solution_->get("ID"))
        , look_for_match_(true)
        , violation_()
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
      /** final evaluation of the match query,
       *  usually triggered from the unit test `CHECK()`.
       * @note failure cause is printed to STDERR.
       */
      operator bool()  const
        {
          if (!isnil (violation_))
            {
              std::cerr << "__Log_condition_violated__\n"+violation_ <<"\n";
              return false;
            }
          return true;
        }
      
      
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
          solution_.underlying().switchForwards();
          solution_.setNewFilter(find(match));
          evaluateQuery ("match(\""+match+"\")");
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
      beforeEvent (string classifier, string match)
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
          solution_.underlying().switchBackwards();
          solution_.setNewFilter(find(match));
          evaluateQuery ("match(\""+match+"\")", "before");
          return *this;
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
      afterEvent (string classifier, string match)
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
      type (string typeID)
        {
          UNIMPLEMENTED("process additional filter on type of the log entry");
        }
      
      EventMatch&
      key (string key)
        {
          UNIMPLEMENTED("process additional filter on the presence of a specific key");
        }
      
      EventMatch&
      attrib (string key, string valueMatch)
        {
          UNIMPLEMENTED("process additional filter on a specific attribute of the log entry");
        }
      
      EventMatch&
      id (string classifier)
        {
          UNIMPLEMENTED("process additional filter on ID classifier of log entry");
        }
      
      EventMatch&
      on (string targetID)
        {
          UNIMPLEMENTED("process additional filter on source of log entry");
        }
      
      template<typename X>
      EventMatch&
      on (const X *const targetObj)
        {
          UNIMPLEMENTED("process additional filter on source of log entry");
        }
    };
  
  
  
  /**
   * Helper to log and verify the occurrence of events.
   * The EventLog object is a front-end handle, logging flexible
   * [information records][lib::Record] into a possibly shared (vector)
   * buffer in heap storage. An extended query DSL allows to write
   * assertions to cover the occurrence of events in unit tests.
   * @see TestEventLog_test
   */
  class EventLog
    {
      using Entry = lib::diff::Record<string>;
      using Log   = std::vector<Entry>;
      using Iter  = lib::RangeIter<Log::const_iterator>;
      
      std::shared_ptr<Log> log_;
      
      void
      log (std::initializer_list<string> const& ili)
        {
          log_->emplace_back(ili);
        }
      
      string
      getID()  const
        {
          return log_->front().get("ID");
        }
      
    public:
      explicit
      EventLog (string logID)
        : log_(new Log)
        {
          log({"type=EventLogHeader", "ID="+logID});
        }
      
      explicit
      EventLog (const char* logID)
        : EventLog(string(logID))
        { }
      
      template<class X>
      explicit
      EventLog (const X *const obj)
        : EventLog(idi::instanceTypeID (obj))
        { }
      
      
      // standard copy operations acceptable
      
      
      /** Merge this log into another log, forming a combined log
       * @param otherLog target to integrate this log's contents into.
       * @return reference to the merged new log instance
       * @remarks EventLog uses a heap based, sharable log storage,
       *          where the EventLog object is just a front-end (shared ptr).
       *          The `joinInto` operation both integrates this logs contents
       *          into the other log, and then disconnects from the old storage
       *          and connects to the storage of the combined log.
       * @warning beware of clone copies. Since copying EventLog is always a
       *          shallow copy, all copied handles actually refer to the same
       *          log storage. If you invoke `joinInto` in such a situation,
       *          only the current EventLog front-end handle will be rewritten
       *          to point to the combined log, while any other clone will
       *          continue to point to the original log storage.
       * @see TestEventLog_test::verify_logJoining()
       */
      EventLog&
      joinInto (EventLog& otherLog)
        {
          Log& target = *otherLog.log_;
          target.reserve (target.size() + log_->size() + 1);
          target.emplace_back (log_->front());
          auto p = log_->begin();
          while (++p != log_->end()) // move our log's content into the other log
            target.emplace_back(std::move(*p));
          this->log_->resize(1);
          this->log({"type=joined", otherLog.getID()});   // leave a tag to indicate
          otherLog.log({"type=logJoin", this->getID()}); //  where the `joinInto` took place,
          this->log_ = otherLog.log_;                   //   connect this to the other storage
          return *this;
        }
      
      
      
      /* ==== Logging API ==== */
      
      using ArgSeq = lib::diff::RecordSetup<string>::Storage;
      
      EventLog&
      event (string text)
        {
          log({"type=event", text});
          return *this;
        }
      
      EventLog&
      event (string classifier, string text)
        {
          UNIMPLEMENTED ("Log event with additional classifier");
        }
      
      EventLog&
      call (string target, string function)
        {
          UNIMPLEMENTED ("Log function call with no arguments");
        }
      
      EventLog&
      call (string target, string function, ArgSeq&& argSeq)
        {
          UNIMPLEMENTED ("Log function call with a sequence of stringified arguments");
        }
      
      template<typename...ARGS>
      EventLog&
      call (string target, string function, ARGS&& ...args)
        {
          UNIMPLEMENTED ("Log function call with arbitrary arguments");
        }
      
      template<class X, typename...ARGS>
      EventLog&
      call (const X *const targetObj, string function, ARGS&& ...args)
        {
          UNIMPLEMENTED ("Log function call on given object with arguments");
        }
      
      template<typename...ELMS>
      EventLog&
      note (ELMS&& ...initialiser)
        {
          UNIMPLEMENTED ("Log generic entry with arbritrary attributes and values");
        }

      
      EventLog&
      warn (string text)
        {
          UNIMPLEMENTED ("Log a warning entry");
        }

      
      EventLog&
      error (string text)
        {
          UNIMPLEMENTED ("Log an error note");
        }

      
      EventLog&
      fatal (string text)
        {
          UNIMPLEMENTED ("Log a fatal failure");
        }

      
      EventLog&
      create (string text)
        {
          UNIMPLEMENTED ("Log the creation of an object");
        }

      
      EventLog&
      destroy (string text)
        {
          UNIMPLEMENTED ("Log the destruction of an object");
        }
      
      
      
      /* ==== Iteration ==== */
      
      bool
      empty()  const
        {
          return 1 >= log_->size();
        }
      
      
      typedef Iter const_iterator;
      typedef const Entry value_type;
      
      const_iterator begin() const { return Iter(log_->begin(), log_->end()); }
      const_iterator end()   const { return Iter(); }
      
      friend const_iterator begin (EventLog const& log) { return log.begin(); }
      friend const_iterator end   (EventLog const& log) { return log.end(); }
      
      
      
      
      /* ==== Query/Verification API ==== */
      
      EventMatch
      verify (string match)  const
        {
          EventMatch matcher(*log_);
          matcher.before (match);
          return matcher;
        }
      
      EventMatch
      verifyMatch (string regExp)  const
        {
          UNIMPLEMENTED("start matching sequence for regular expression match");
        }
      
      EventMatch
      verifyEvent (string match)  const
        {
          UNIMPLEMENTED("start matching sequence");
        }
      
      EventMatch
      verifyEvent (string classifier, string match)  const
        {
          UNIMPLEMENTED("start matching sequence");
        }
      
      EventMatch
      verifyCall (string match)  const
        {
          UNIMPLEMENTED("start matching sequence");
        }
      
      EventMatch
      ensureNot (string match)  const
        {
          EventMatch matcher(*log_);
          matcher.look_for_match_ = false; // flip logic; fail if match succeeds
          matcher.before (match);
          return matcher;
        }
      
      
      /** equality comparison is based on the actual log contents */
      friend bool
      operator== (EventLog const& l1, EventLog const& l2)
      {
        return l1.log_ == l2.log_
            or (l1.log_ and l2.log_
                and *l1.log_ == *l2.log_);
      }
      friend bool
      operator!= (EventLog const& l1, EventLog const& l2)
      {
        return not (l1 == l2);
      }
    };
  
  
  
}} // namespace lib::test
#endif /*LIB_TEST_EVENT_LOG_H*/
