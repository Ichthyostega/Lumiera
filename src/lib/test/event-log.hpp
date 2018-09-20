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
 ** used for access offers a query DSL, so the test code may express some
 ** expected patterns of incidence and verify match or non-match.
 ** 
 ** # Usage
 ** 
 ** The EventLog just captures invocations in memory, as sequence of
 ** [string records](\ref lib::diff::Record). The _logging API_ offers some
 ** dedicated functions to record invocations, events, the creation and destruction
 ** of objects, warnings and error messages. Data passed to those logging functions
 ** is rendered into string, using Lumiera's generic string conversion helpers.
 ** 
 ** To _verify and match_ on log contents, start an evaluation as a builder (DSL)
 ** expression, starting from one of the #verify functions, or starting with #ensureNot,
 ** to probe that a given set of matches does not occur in the log. These entrance
 ** functions set up the initial search condition, and further search conditions
 ** can be chained up by invoking suitable matching functions on the generated
 ** EventMatch builder. Most notably, it is possible to search for some further
 ** match condition _before_ or _after_ the position where the preceding condition
 ** produced a match -- allowing to specify sequences and patterns of events to
 ** expect in the log
 ** \code{.cpp}
 **   EventLog log("baked beans");
 **   log.event("spam");
 **   log.event("ham");
 **   
 **   CHECK (log.verify("ham")
 **             .after("beans")
 **             .before("spam")
 **             .before("ham"));
 ** \endcode
 ** In general, this kind of search with a switch of the search direction in the middle
 ** of a match requires some backtracking to try possible matches until exhaustion.
 ** 
 ** Failure of match prints a detailed trace message to `STDERR`, in order
 ** to deliver a precise indication what part of the condition failed.
 ** @note this sequence prints the matches succeeding _at the point_ where each
 **       condition is added to the chain. Adding more conditions, especially when
 **       combined with changed search direction, might lead to backtracking, which
 **       happens silently within the search engine, without printing any further
 **       diagnostics. This means: the sequence of matches you see in this diagnostic
 **       output is not necessarily the last match patch, which lead to the final failure
 ** 
 ** @see TestEventLog_test
 ** @see [usage example](\ref AbstractTangible_test)
 ** 
 */


#ifndef LIB_TEST_EVENT_LOG_H
#define LIB_TEST_EVENT_LOG_H


#include "lib/error.hpp"
#include "lib/iter-cursor.hpp"
#include "lib/iter-chain-search.hpp"
#include "lib/format-util.hpp"
#include "lib/format-cout.hpp"
#include "lib/diff/record.hpp"
#include "lib/idi/genfunc.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <memory>
#include <vector>
#include <string>
#include <regex>



namespace lib {
namespace test{
  
  using lib::Symbol;
  using std::string;
  using util::stringify;
  
  namespace {
    using Entry = lib::diff::Record<string>;
    using Log   = std::vector<Entry>;
    
    auto
    buildSearchFilter(Log const& srcSeq)
    {
      using Cursor  = lib::iter::CursorGear<Log::const_iterator>;
      return iter::chainSearch (Cursor{srcSeq.begin(), srcSeq.end()});
    }
  }
  
  
  /**
   * @internal ongoing evaluation and match within an [EventLog].
   * @remarks an EventMatch object is returned when building query expression
   *          to verify the occurrence of some events within the EventLog.
   *          This "matcher" object implements the query logic with backtracking.
   *          The query expressions are added as filter functors to a filtering
   *          iterator; when all of the log's contents are filtered away,
   *          the evaluation counts as failed.
   */
  class EventMatch
    {
      using Filter = decltype( buildSearchFilter (std::declval<Log const&>()) );
      
      using ArgSeq = lib::diff::RecordSetup<string>::Storage;
      using RExSeq = std::vector<std::regex>;
      
      
      /** match predicate evaluator */
      Filter solution_;
      
      /** record last match for diagnostics */
      string lastMatch_;
      
      /** support for positive and negative queries. */
      bool look_for_match_;
      
      /** record when the underlying query has failed */
      string violation_;
      
      
      
      /** @internal for creating EventLog matchers */
      EventMatch(Log const& srcSeq);
      
      friend class EventLog;
      
      
    public:
      /** final evaluation of the match query,
       *  usually triggered from the unit test `CHECK()`.
       * @note failure cause is printed to STDERR.
       */
      operator bool()  const
        {
          if (not util::isnil (violation_))
            {
              cerr << "__Log_condition_violated__\n"+violation_ <<"\n";
              return false;
            }
          return true;
        }
      
      
      /* query builders to continue search at current position */
      
      EventMatch& locate (string match);
      EventMatch& locateMatch (string regExp);
      EventMatch& locateEvent (string match);
      EventMatch& locateEvent (string classifier, string match);
      EventMatch& locateCall (string match);
      
      
      /* query builders to find a match stepping forwards */
      
      EventMatch& before (string match);
      EventMatch& beforeMatch (string regExp);
      EventMatch& beforeEvent (string match);
      EventMatch& beforeEvent (string classifier, string match);
      EventMatch& beforeCall (string match);
      
      
      /* query builders to find a match stepping backwards */
      
      EventMatch& after (string match);
      EventMatch& afterMatch (string regExp);
      EventMatch& afterEvent (string match);
      EventMatch& afterEvent (string classifier, string match);
      EventMatch& afterCall (string match);
      
      /** refine filter to additionally require specific arguments
       * @remarks the refined filter works on each record qualified by the
       *          query expression established thus far; it additionally
       *          looks into the arguments (children list) of the log entry.
       * @warning match is processed by comparison of string representation.
       */
      template<typename...ARGS>
      EventMatch&
      arg (ARGS const& ...args)
        {
          refineSerach_matchArguments (stringify<ArgSeq> (args...));
          return *this;
        }
      
      
      /** refine filter to additionally cover all arguments
       *  with a series of regular expressions.
       * @note For this match to succeed, any arguments of the log entry
       *       must be covered by applying the given regular expressions
       *       consecutively. Each regular expression is applied to further
       *       arguments, as long as it matches. It is possible to have
       *       just one RegExp to "rule them all", but as soon as there
       *       is one argument, that can not be covered by the next
       *       RegExp, the whole match counts as failed.
       */
      template<typename...ARGS>
      EventMatch&
      argMatch (ARGS const& ...regExps)
        {
          refineSerach_matchArgsRegExp (stringify<RExSeq> (regExps...),
                                        util::join(stringify<ArgSeq>(regExps...)));
          return *this;
        }
      
      
      
      /* query builders to augment and refine the currently defined search condition*/
      
      EventMatch& type (string typeID);
      EventMatch& key (string key);
      EventMatch& attrib (string key, string valueMatch);
      EventMatch& id (string classifier);
      EventMatch& on (string targetID);
      EventMatch& on (const char* targetID);
      
      template<typename X>
      EventMatch& on (const X *const targetObj)
        {
          string targetID = idi::instanceTypeID (targetObj);
          return this->on(targetID);
        }
      
    private:
      bool foundSolution();
      void evaluateQuery (string matchSpec, Literal rel = "after");
      
      void refineSerach_matchArguments (ArgSeq&& argSeq);
      void refineSerach_matchArgsRegExp (RExSeq&& regExpSeq, string rendered_regExps);
    };
  
  
  
  
  
  
  
  
  /****************************************************************//**
   * Helper to log and verify the occurrence of events.
   * The EventLog object is a front-end handle, logging flexible
   * [information records](\ref lib::Record) into a possibly shared (vector)
   * buffer in heap storage. An extended query DSL allows to write
   * assertions to cover the occurrence of events in unit tests.
   * @see TestEventLog_test
   */
  class EventLog
    {
      
      std::shared_ptr<Log> log_;
      
      
      void
      log (std::initializer_list<string> const& ili)
        {
          log_->emplace_back(ili);
        }
      
      template<typename ATTR, typename ARGS>
      void
      log (Symbol typeID, ATTR&& attribs, ARGS&& args)
        {
          log_->emplace_back(typeID, std::forward<ATTR>(attribs)
                                   , std::forward<ARGS>(args));
        }
      
      string
      getID()  const
        {
          return log_->front().get("this");
        }
      
      
    public:
      explicit
      EventLog (string logID);
      
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
       * @see TestEventLog_test::verify_logJoining()
       */
      EventLog& joinInto (EventLog& otherLog);
      
      
      /** purge log contents while retaining just the original Header-ID */
      EventLog& clear();
      EventLog& clear (string alteredLogID);
      EventLog& clear (const char* alteredLogID);
      
      template<class X>
      EventLog&
      clear (const X *const obj)
        {
          return clear (idi::instanceTypeID (obj));
        }
      
      
      
      /* ==== Logging API ==== */
      
      using ArgSeq = lib::diff::RecordSetup<string>::Storage;
      
      /** log some text as event */
      EventLog& event (string text);
      
      /** log some event, with additional ID or classifier
       * @param classifier info to be saved into the `ID` attribute
       * @param text actual payload info, to be logged as argument
       */
      EventLog& event (string classifier, string text);
      
      /** Log occurrence of a function call with no arguments.
       * @param target the object or scope on which the function is invoked
       * @param function name of the function being invoked
       */
      EventLog& call (string target, string function);
      
      /** Log a function call with a sequence of stringified arguments */
      EventLog& call (string target, string function, ArgSeq&& args);
      
      EventLog& call (const char* target, const char* function, ArgSeq&& args);
      
      /** Log a function call with arbitrary arguments */
      template<typename...ARGS>
      EventLog&
      call (string target, string function, ARGS const& ...args)
        {
          return call (target, function, stringify<ArgSeq>(args...));
        }
      
      /** Log a function call on given object ("`this`")... */
      template<class X, typename...ARGS>
      EventLog&
      call (const X *const targetObj, string function, ARGS const& ...args)
        {
          return call (idi::instanceTypeID (targetObj), function, args...);
        }
      
      template<typename...ARGS>
      EventLog&
      call (const char* target, string function, ARGS const& ...args)
        {
          return call (string(target), function, args...);
        }
      
      template<typename...ELMS>
      EventLog&
      note (ELMS const& ...initialiser)
        {
          log_->emplace_back (stringify<ArgSeq> (initialiser...));
          return *this;
        }

      
      /** Log a warning entry */
      EventLog& warn (string text);
      
      /** Log an error note */
      EventLog& error (string text);
      
      /** Log a fatal failure */
      EventLog& fatal (string text);
      
      /** Log the creation of an object.
       *  Such an entry can be [matched as event](\ref ::verifyEvent) */
      EventLog& create (string text);

      
      /** Log the destruction of an object.
       *  Can be matched as event. */
      EventLog& destroy (string text);
      
      
      
      
      /* ==== Iteration ==== */
      
      bool
      empty()  const
        {
          return 1 >= log_->size();  // do not count the log header
        }
      
      
      using Iter  = lib::RangeIter<Log::const_iterator>;
      
      typedef Iter const_iterator;
      typedef const Entry value_type;
      
      const_iterator begin() const { return Iter(log_->begin(), log_->end()); }
      const_iterator end()   const { return Iter(); }
      
      friend const_iterator begin (EventLog const& log) { return log.begin(); }
      friend const_iterator end   (EventLog const& log) { return log.end(); }
      
      
      
      
      /* ==== Query/Verification API ==== */
      
      /** start a query to match for some substring.
       *  The resulting matcher object will qualify on any log entry
       *  containing the given string. By adding subsequent further
       *  query expressions on the returned [matcher object](\ref EventMatch),
       *  the query can be refined. Moreover it is possible to chain up further
       *  search queries, which will be executing starting from the position of the
       *  previous match. The final result can be retrieved by `bool` conversion
       */
      EventMatch verify (string match)  const;
      
      /** start a query to match with a regular expression
       * @param regExp definition
       * @remarks the expression will work against the full
       *          `string` representation of the log entries.
       *          Meaning, it can also match type and attributes
       */
      EventMatch verifyMatch (string regExp)  const;
      
      /** start a query to match for some event.
       * @remarks only a subset of all log entries is treated as "event",
       *          any other log entry will not be considered for this query.
       *          Besides the regular [events](\ref ::event()), also errors,
       *          creation and destruction of objects count as "event".
       * @param match text to (substring)match against the argument logged as event
       */
      EventMatch verifyEvent (string match)  const;
      
      /** start a query to match for an specific kind of element
       * @param classifier select kind of event by match on type or ID
       */
      EventMatch verifyEvent (string classifier, string match)  const;
      
      template<typename X>
      EventMatch
      verifyEvent (string classifier, X const& something)  const
        {
          return verifyEvent (classifier, util::toString (something));
        }
      
      /** start a query to match especially a function call
       * @param match text to match against the function name
       */
      EventMatch verifyCall (string match)  const;
      
      /** start a query to ensure the given expression does _not_ match. */
      EventMatch
      ensureNot (string match)  const;
      
      
      
      
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
