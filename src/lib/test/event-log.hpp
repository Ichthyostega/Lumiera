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
 ** Failure of match prints a detailed trace message to `STDERR`, in order
 ** to deliver a precise indication what part of the condition failed.
 ** 
 ** @see TestEventLog_test
 ** @see [usage example](\ref AbstractTangible_test)
 ** 
 */


#ifndef LIB_TEST_EVENT_LOG_H
#define LIB_TEST_EVENT_LOG_H


#include "lib/error.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/iter-cursor.hpp"
#include "lib/format-util.hpp"
#include "lib/format-cout.hpp"
#include "lib/diff/record.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <memory>
#include <vector>
#include <string>
#include <regex>



namespace lib {
namespace test{
  namespace error = lumiera::error;
  
  using util::stringify;
  using util::contains;
  using util::isnil;
  using lib::Symbol;
  using std::string;
  
  
  
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
      using Entry = lib::diff::Record<string>;
      using Log   = std::vector<Entry>;
      using Iter  = lib::IterCursor<Log::const_iterator>;
      using Filter = ExtensibleFilterIter<Iter>;
      
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
      
      /** core of the evaluation machinery:
       * apply a filter predicate and then pull
       * through the log to find a acceptable entry
       */
      bool
      foundSolution()
        {
          return not isnil (solution_);
        }
      
      /** this is actually called after each refinement of
       * the filter and matching conditions. The effect is to search
       * for an (intermediary) solution right away and to mark failure
       * as soon as some condition can not be satisfied. Rationale is to
       * indicate the point where a chained match fails
       * @see ::operator bool() for extracting the result
       * @param matchSpec diagnostics description of the predicate just being added
       * @param rel indication of the searching relation / direction
       */
      void
      evaluateQuery (string matchSpec, Literal rel = "after")
        {
          if (look_for_match_ and not isnil (violation_)) return;
             // already failed, no further check necessary
          
          if (foundSolution()) // NOTE this pulls the filter
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
        , lastMatch_("HEAD "+ solution_->get("this"))
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
      
      auto
      findRegExp (string regExpDef)
        {
          std::regex regExp(regExpDef);
          return [=](Entry const& entry)
                    {
                      return std::regex_search(string(entry), regExp);
                    };
        }
      
      auto
      findEvent (string match)
        {
          return [=](Entry const& entry)
                    {
                      return (  entry.getType() == "event"
                             or entry.getType() == "error"
                             or entry.getType() == "create"
                             or entry.getType() == "destroy"
                             or entry.getType() == "logJoin"
                             )
                         and !isnil(entry.scope())
                         and contains (*entry.scope(), match);
                    };
        }
      
      auto
      findEvent (string classifier, string match)
        {
          return [=](Entry const& entry)
                    {
                      return (  entry.getType() == classifier
                             or (entry.hasAttribute("ID") and contains (entry.get("ID"), classifier))
                             )
                         and !isnil(entry.scope())
                         and contains (*entry.scope(), match);
                    };
        }
      
      auto
      findCall (string match)
        {
          return [=](Entry const& entry)
                    {
                      return entry.getType() == "call"
                         and contains (entry.get("fun"), match);
                    };
        }
      
      
      /** this filter functor is for refinement of an existing filter
       * @param argSeq perform a substring match consecutively
       *        for each of the log entry's arguments
       * @note the match also fails, when the given log entry
       *       has more or less arguments, than the number of
       *       given match expressions in `argSeq`
       * @see ExtensibleFilterIter::andFilter()
       */
      auto
      matchArguments (ArgSeq&& argSeq)
        {
          return [=](Entry const& entry)
                    {
                      auto scope = entry.scope();
                      for (auto const& match : argSeq)
                        if (isnil (scope) or not contains(*scope, match))
                          return false;
                        else
                          ++scope;
                      
                      return isnil(scope);  // must be exhausted by now
                    };                     //  otherwise the sizes do not match...
        }
      
      
      /** refinement filter, to cover all arguments by regular expression(s)
       * @param regExpSeq several regular expressions, which, when applied
       *        consecutively until exhaustion, must cover and verify _all_
       *        arguments of the log entry.
       * @remarks to explain, we "consume" arguments with a regExp from the list,
       *        and when this one doesn't match anymore, we try the next one.
       *        When we'ver tried all regular expressions, we must have also
       *        consumed all arguments, otherwise we fail.
       */
      auto
      matchArgsRegExp (RExSeq&& regExpSeq)
        {
          return [=](Entry const& entry)
                    {
                      auto scope = entry.scope();
                      for (auto const& rex : regExpSeq)
                        {
                          if (isnil (scope)) return false;
                          while (scope and std::regex_search(*scope, rex))
                            ++scope;
                        }
                      
                      return isnil(scope);  // must be exhausted by now
                    };                     //  otherwise we didn't get full coverage...
        }
      
      
      /** refinement filter to match on the given typeID */
      auto
      matchType (string typeID)
        {
          return [=](Entry const& entry)
                    {
                      return contains (entry.getType(), typeID);
                    };
        }
      
      
      /** refinement filter to ensure a specific attribute is present on the log entry */
      auto
      ensureAttribute (string key)
        {
          return [=](Entry const& entry)
                    {
                      return entry.hasAttribute(key);
                    };
        }
      
      
      /** refinement filter to ensure a specific attribute is present on the log entry */
      auto
      matchAttribute (string key, string valueMatch)
        {
          return [=](Entry const& entry)
                    {
                      return entry.hasAttribute(key)
                         and contains (entry.get(key), valueMatch);
                    };
        }
      
      
      /* === configure the underlying search engine === */
      
      enum Direction {
        FORWARD, BACKWARD
      };
      
      template<typename COND>
      void
      attachNextSerchStep (COND&& filter, Direction direction)
        {
          switch (direction)
            {
              case FORWARD:
                solution_.underlying().switchForwards();
                break;
              case BACKWARD:
                solution_.underlying().switchBackwards();
                break;
            }
          solution_.setNewFilter (forward<COND> (filter));
        }
      
      template<typename COND>
      void
      refineSerach (COND&& additionalFilter)
        {
          solution_.andFilter (forward<COND> (additionalFilter));
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
              cerr << "__Log_condition_violated__\n"+violation_ <<"\n";
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
          attachNextSerchStep (find(match), FORWARD);
          evaluateQuery ("match(\""+match+"\")");
          return *this;
        }
      
      /** find a match with the given regular expression */
      EventMatch&
      beforeMatch (string regExp)
        {
          attachNextSerchStep (findRegExp(regExp), FORWARD);
          evaluateQuery ("find-RegExp(\""+regExp+"\")");
          return *this;
        }
      
      /** find a match for an "event" _after_ the current point of reference
       * @remarks the term "event" designates several types of entries, which
       *          typically capture something happening within the observed entity.
       *          Especially, the following [record types](\ref lib::Record::getType())
       *          qualify as event:
       *          - `event`
       *          - `error`
       *          - `create`
       *          - `destroy`
       *          - `logJoin`
       * @param match perform a substring match against the arguments of the event
       * @see ::findEvent
       */
      EventMatch&
      beforeEvent (string match)
        {
          attachNextSerchStep (findEvent(match), FORWARD);
          evaluateQuery ("match-event(\""+match+"\")");
          return *this;
        }
      
      EventMatch&
      beforeEvent (string classifier, string match)
        {
          attachNextSerchStep (findEvent(classifier,match), FORWARD);
          evaluateQuery ("match-event(ID=\""+classifier+"\", \""+match+"\")");
          return *this;
        }
      
      /** find a match for some function invocation _after_ the current point of reference
       * @param match perform a substring match against the name of the function invoked
       * @see ::findCall
       */
      EventMatch&
      beforeCall (string match)
        {
          attachNextSerchStep (findCall(match), FORWARD);
          evaluateQuery ("match-call(\""+match+"\")");
          return *this;
        }
      
      EventMatch&
      after (string match)
        {
          attachNextSerchStep (find(match), BACKWARD);
          evaluateQuery ("match(\""+match+"\")", "before");
          return *this;
        }
      
      EventMatch&
      afterMatch (string regExp)
        {
          attachNextSerchStep (findRegExp(regExp), BACKWARD);
          evaluateQuery ("find-RegExp(\""+regExp+"\")", "before");
          return *this;
        }
      
      EventMatch&
      afterEvent (string match)
        {
          attachNextSerchStep (findEvent(match), BACKWARD);
          evaluateQuery ("match-event(\""+match+"\")", "before");
          return *this;
        }
      
      EventMatch&
      afterEvent (string classifier, string match)
        {
          attachNextSerchStep (findEvent(classifier,match), BACKWARD);
          evaluateQuery ("match-event(ID=\""+classifier+"\", \""+match+"\")", "before");
          return *this;
        }
      
      /** find a function invocation backwards, before the current point of reference */
      EventMatch&
      afterCall (string match)
        {
          attachNextSerchStep (findCall(match), BACKWARD);
          evaluateQuery ("match-call(\""+match+"\")", "before");
          return *this;
        }
      
      /** refine filter to additionally require specific arguments
       * @remarks the refined filter works on each record qualified by the
       *          query expression established thus far; it additionally
       *          looks into the arguments (children list) of the log entry.
       * @warning match is processed by comparision of string representation.
       */
      template<typename...ARGS>
      EventMatch&
      arg (ARGS const& ...args)
        {
          ArgSeq argSeq(stringify<ArgSeq> (args...));
          string argList(util::join(argSeq));
          
          refineSerach (matchArguments(move(argSeq)));
          evaluateQuery ("match-arguments("+argList+")");
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
          refineSerach (matchArgsRegExp (stringify<RExSeq> (regExps...)));
          evaluateQuery ("match-args-RegExp("+util::join(stringify<ArgSeq>(regExps...))+")");
          return *this;
        }
      
      /** refine filter to additionally require a matching log entry type */
      EventMatch&
      type (string typeID)
        {
          refineSerach (matchType(typeID));
          evaluateQuery ("match-type("+typeID+")");
          return *this;
        }
      
      /** refine filter to additionally require the presence an attribute */
      EventMatch&
      key (string key)
        {
          refineSerach (ensureAttribute(key));
          evaluateQuery ("ensure-attribute("+key+")");
          return *this;
        }
      
      /** refine filter to additionally match on a specific attribute */
      EventMatch&
      attrib (string key, string valueMatch)
        {
          refineSerach (matchAttribute(key,valueMatch));
          evaluateQuery ("match-attribute("+key+"=\""+valueMatch+"\")");
          return *this;
        }
      
      /** refine filter to additionally match on the ID attribute */
      EventMatch&
      id (string classifier)
        {
          refineSerach (matchAttribute("ID",classifier));
          evaluateQuery ("match-ID(\""+classifier+"\")");
          return *this;
        }
      
      /** refine filter to additionally match the `'this'` attribute */
      EventMatch&
      on (string targetID)
        {
          refineSerach (matchAttribute("this",targetID));
          evaluateQuery ("match-this(\""+targetID+"\")");
          return *this;
        }
      
      EventMatch&
      on (const char* targetID)
        {
          refineSerach (matchAttribute("this",targetID));
          evaluateQuery ("match-this(\""+string(targetID)+"\")");
          return *this;
        }
      
      template<typename X>
      EventMatch&
      on (const X *const targetObj)
        {
          string targetID = idi::instanceTypeID (targetObj);
          refineSerach (matchAttribute("this",targetID));
          evaluateQuery ("match-this(\""+targetID+"\")");
          return *this;
        }
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
      using Entry = lib::diff::Record<string>;
      using Log   = std::vector<Entry>;
      using Iter  = lib::RangeIter<Log::const_iterator>;
      
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
      EventLog (string logID)
        : log_(new Log)
        {
          log({"type=EventLogHeader", "this="+logID});
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
      
      
      /** purge log contents while retaining just the original Header-ID */
      EventLog&
      clear()
        {
          string originalLogID = this->getID();
          return this->clear (originalLogID);
        }
      
      /** purge log contents and also reset Header-ID
       * @note actually we're starting a new log
       *       and let the previous one go away.
       * @warning while this also unties any joined logs,
       *       other log front-ends might still hold onto
       *       the existing, combined log. Just we are
       *       detached and writing to a pristine log.
       */
      EventLog&
      clear (string alteredLogID)
        {
          log_.reset (new Log);
          log({"type=EventLogHeader", "this="+alteredLogID});
          return *this;
        }
      
      EventLog&
      clear (const char* alteredLogID)
        {
          return clear (string{alteredLogID});
        }
      
      template<class X>
      EventLog&
      clear (const X *const obj)
        {
          return clear (idi::instanceTypeID (obj));
        }
      
      
      
      /* ==== Logging API ==== */
      
      using ArgSeq = lib::diff::RecordSetup<string>::Storage;
      
      EventLog&
      event (string text)
        {
          log ("event", ArgSeq{}, ArgSeq{text});  // we use this ctor variant to ensure
          return *this;                          //  that text is not misinterpreted as attribute,
        }                                       //   which might happen when text contains a '='
      
      /** log some event, with additional ID or classifier
       * @param classifier info to be saved into the `ID` attribute
       * @param text actual payload info, to be logged as argument
       */
      EventLog&
      event (string classifier, string text)
        {
          log ("event", ArgSeq{"ID="+classifier}, ArgSeq{text});
          return *this;
        }
      
      /** Log occurrence of a function call with no arguments.
       * @param target the object or scope on which the function is invoked
       * @param function name of the function being invoked
       */
      EventLog&
      call (string target, string function)
        {
          return call(target, function, ArgSeq());
        }
      
      /** Log a function call with a sequence of stringified arguments */
      EventLog&
      call (string target, string function, ArgSeq&& args)
        {
          log ("call", ArgSeq{"fun="+function, "this="+target}, std::forward<ArgSeq>(args));
          return *this;
        }
      
      EventLog&
      call (const char* target, const char* function, ArgSeq&& args)
        {
          return call (string(target), string(function), std::forward<ArgSeq>(args));
        }
      
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
          log_->emplace_back(stringify<ArgSeq> (initialiser...));
          return *this;
        }

      
      /** Log a warning entry */
      EventLog&
      warn (string text)
        {
          log({"type=warn", text});
          return *this;
        }

      
      /** Log an error note */
      EventLog&
      error (string text)
        {
          log({"type=error", text});
          return *this;
        }

      
      /** Log a fatal failure */
      EventLog&
      fatal (string text)
        {
          log({"type=fatal", text});
          return *this;
        }

      
      /** Log the creation of an object.
       *  Such an entry can be [matched as event](\ref ::verifyEvent) */
      EventLog&
      create (string text)
        {
          log({"type=create", text});
          return *this;
        }

      
      /** Log the destruction of an object.
       *  Can be matched as event. */
      EventLog&
      destroy (string text)
        {
          log({"type=destroy", text});
          return *this;
        }
      
      
      
      /* ==== Iteration ==== */
      
      bool
      empty()  const
        {
          return 1 >= log_->size();  // do not count the log header
        }
      
      
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
       *  the query can be refined. Refining a query might induce backtracking.
       *  The final result can be retrieved by `bool` conversion
       */
      EventMatch
      verify (string match)  const
        {
          EventMatch matcher(*log_);
          matcher.before (match);   // "the start of the log is before the match"
          return matcher;
        }
      
      /** start a query to match with a regular expression
       * @param regExp definition
       * @remarks the expression will work against the full
       *          `string` representation of the log entries.
       *          Meaning, it can also match type and attributes
       */
      EventMatch
      verifyMatch (string regExp)  const
        {
          EventMatch matcher(*log_);
          matcher.beforeMatch (regExp);
          return matcher;
        }
      
      /** start a query to match for some event.
       * @remarks only a subset of all log entries is treated as "event",
       *          any other log entry will not be considered for this query.
       *          Besides the regular [events](\ref ::event()), also errors,
       *          creation and destruction of objects count as "event".
       * @param match text to (substring)match against the argument logged as event
       */
      EventMatch
      verifyEvent (string match)  const
        {
          EventMatch matcher(*log_);
          matcher.beforeEvent (match);
          return matcher;
        }
      
      EventMatch
      verifyEvent (string classifier, string match)  const
        {
          EventMatch matcher(*log_);
          matcher.beforeEvent (classifier, match);
          return matcher;
        }
      
      template<typename X>
      EventMatch
      verifyEvent (string classifier, X const& something)  const
        {
          return verifyEvent (classifier, util::toString (something));
        }
      
      /** start a query to match especially a function call
       * @param match text to match against the function name
       */
      EventMatch
      verifyCall (string match)  const
        {
          EventMatch matcher(*log_);
          matcher.beforeCall (match);
          return matcher;
        }
      
      /** start a query to ensure the given expression does _not_ match.
       * @remarks the query expression is built similar to the other queries,
       *          but the logic of evaluation is flipped: whenever we find any match
       *          the overall result (when evaluating to `bool`) will be `false`.
       * @warning this is not an proper exhaustive negation, since the implementation
       *          does not proper backtracking with a stack of choices. This becomes
       *          evident, when you combine `ensureNot()` with a switch in search
       *          direction, like e.g. using `afterXXX` at some point in the chain.
       */
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
