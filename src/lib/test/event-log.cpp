/*
  EventLog  -  test facility to verify the occurrence of expected events

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/

/** @file event-log.cpp
 ** Implementation details of event logging and verification for unit tests.
 ** The EventLog is a helper for writing tests or for special rigging within
 ** the test or debugging or mock variant of some common services. It allows
 ** to log some events to memory and then to build match expressions to verify
 ** those captured information.
 ** 
 ** The machinery for matching on the occurrence of some events can be quite
 ** intricate at implementation level; we use an implementation based on a
 ** filtering iterator pipeline. This kind of code is template heavy and
 ** can cause a significant increase of generated code size -- especially
 ** on debug builds. For that reason, we prefer to call through regular
 ** (non-inline) functions and package all the actual implementation
 ** code into this dedicated translation unit.
 ** 
 ** @see EventLog_test
 ** @see IterChainSearch_test
 ** @see iter-explorer.hpp
 ** 
 */



#include "lib/error.hpp"
#include "lib/test/event-log.hpp"



namespace lib {
namespace test{
  namespace error = lumiera::error;
  
  using util::stringify;
  using util::contains;
  using util::isnil;
  using lib::Symbol;
  using std::string;
  
  
  namespace { /* == elementary matchers == */
    
    using Filter = decltype( buildSearchFilter (std::declval<Log const&>()) );
    
    using ArgSeq = lib::diff::RecordSetup<string>::Storage;
    using RExSeq = std::vector<std::regex>;
    
    
    inline auto
    find (string match)
    {
      return [=](Entry const& entry)
                {
                  return contains (string(entry), match);
                };
    }
    
    inline auto
    findRegExp (string regExpDef)
    {
      std::regex regExp(regExpDef);
      return [=](Entry const& entry)
                {
                  return std::regex_search(string(entry), regExp);
                };
    }
    
    inline auto
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
                     and not isnil(entry.scope())
                     and contains (*entry.scope(), match);
                };
    }
    
    inline auto
    findEvent (string classifier, string match)
    {
      return [=](Entry const& entry)
                {
                  return (  entry.getType() == classifier
                         or (entry.hasAttribute("ID") and contains (entry.get("ID"), classifier))
                         )
                     and not isnil(entry.scope())
                     and contains (*entry.scope(), match);
                };
    }
    
    inline auto
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
    inline auto
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
    
    /** refinement filter to match a specific positional argument */
    inline auto
    matchArgument (size_t idx, string match)
    {
      return [=](Entry const& entry)
                {
                  return idx < entry.childSize()
                     and contains (entry.child(idx), match);
                };
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
    inline auto
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
    inline auto
    matchType (string typeID)
    {
      return [=](Entry const& entry)
                {
                  return contains (entry.getType(), typeID);
                };
    }
    
    
    /** refinement filter to ensure a specific attribute is present on the log entry */
    inline auto
    ensureAttribute (string key)
    {
      return [=](Entry const& entry)
                {
                  return entry.hasAttribute(key);
                };
    }
    
    
    /** refinement filter to ensure a specific attribute is present on the log entry */
    inline auto
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
      FORWARD, BACKWARD, CURRENT
    };
    
    template<typename COND>
    inline void
    attachNextSerchStep (Filter& solution, COND&& searchCond, Direction direction)
    {
      if (CURRENT == direction)
        solution.search (forward<COND> (searchCond));
      else
        solution.addStep ([predicate{forward<COND> (searchCond)}, direction]
                          (auto& filter)
                            {
                              filter.reverse (BACKWARD == direction);
                              filter.disableFilter();  // deactivate any filtering temporarily
                              ++filter;               //  move one step in the required direction
                              filter.setNewFilter (predicate);
                            });
    }
    
    template<typename COND>
    inline void
    refineSerach (Filter& solution, COND&& additionalCond)
    {
      solution.addStep ([predicate{forward<COND> (additionalCond)}]
                        (auto& filter)
                          {
                            filter.andFilter (predicate);
                          });
    }
    
  }//(End) Elementary Matchers and handling of the search engine
  
  
  
  
  
  EventMatch::EventMatch (Log const& srcSeq)
    : solution_{buildSearchFilter (srcSeq)}
    , lastMatch_{"HEAD "+ solution_->get("this")}
    , look_for_match_{true}
    , violation_{}
    { }
  
  
  
  /** core of the evaluation machinery:
   * apply a filter predicate and then pull
   * through the log to find a acceptable entry
   */
  bool
  EventMatch::foundSolution()
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
  EventMatch::evaluateQuery (string matchSpec, Literal rel)
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

    
    
  /**
   * basic search function: continue linear lookup over the elements of the
   * EventLog to find a match (substring match) of the given text. The search begins
   * at the current position and proceeds in the currently configured direction.
   * Initially the search starts at the first record and proceeds forward.
   */
  EventMatch&
  EventMatch::locate (string match)
  {
    attachNextSerchStep (solution_, find(match), CURRENT);
    evaluateQuery ("match(\""+match+"\")");
    return *this;
  }
  
  /**
   * basic search like locate() but with the given regular expression
   */
  EventMatch&
  EventMatch::locateMatch (string regExp)
  {
    attachNextSerchStep (solution_, findRegExp(regExp), CURRENT);
    evaluateQuery ("find-RegExp(\""+regExp+"\")");
    return *this;
  }
  
  /**
   * basic search for a matching  "event"
   * @param match perform a substring match against the arguments of the event
   * @see beforeEvent() for a description of possible "events"
   */
  EventMatch&
  EventMatch::locateEvent (string match)
  {
    attachNextSerchStep (solution_, findEvent(match), CURRENT);
    evaluateQuery ("match-event(\""+match+"\")");
    return *this;
  }
  
  EventMatch&
  EventMatch::locateEvent (string classifier, string match)
  {
    attachNextSerchStep (solution_, findEvent(classifier,match), CURRENT);
    evaluateQuery ("match-event(ID=\""+classifier+"\", \""+match+"\")");
    return *this;
  }
  
  /**
   * basic search for some specific function invocation
   * @param match perform a substring match against the name of the function invoked
   */
  EventMatch&
  EventMatch::locateCall (string match)
  {
    attachNextSerchStep (solution_, findCall(match), CURRENT);
    evaluateQuery ("match-call(\""+match+"\")");
    return *this;
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
  EventMatch::before (string match)
  {
    attachNextSerchStep (solution_, find(match), FORWARD);
    evaluateQuery ("match(\""+match+"\")");
    return *this;
  }
  
  /**
   * find a match with the given regular expression
   */
  EventMatch&
  EventMatch::beforeMatch (string regExp)
  {
    attachNextSerchStep (solution_, findRegExp(regExp), FORWARD);
    evaluateQuery ("find-RegExp(\""+regExp+"\")");
    return *this;
  }
  
  /**
   * find a match for an "event" _after_ the current point of reference
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
  EventMatch::beforeEvent (string match)
  {
    attachNextSerchStep (solution_, findEvent(match), FORWARD);
    evaluateQuery ("match-event(\""+match+"\")");
    return *this;
  }
  
  EventMatch&
  EventMatch::beforeEvent (string classifier, string match)
  {
    attachNextSerchStep (solution_, findEvent(classifier,match), FORWARD);
    evaluateQuery ("match-event(ID=\""+classifier+"\", \""+match+"\")");
    return *this;
  }
  
  /**
   * find a match for some function invocation _after_ the current point of reference
   * @param match perform a substring match against the name of the function invoked
   * @see ::findCall
   */
  EventMatch&
  EventMatch::beforeCall (string match)
  {
    attachNextSerchStep (solution_, findCall(match), FORWARD);
    evaluateQuery ("match-call(\""+match+"\")");
    return *this;
  }
  
  
  
  
  /**
   * find a match (substring match) of the given text
   * in an EventLog entry _before_ the current position,
   * switching to backwards search direction
   */
  EventMatch&
  EventMatch::after (string match)
  {
    attachNextSerchStep (solution_, find(match), BACKWARD);
    evaluateQuery ("match(\""+match+"\")", "before");
    return *this;
  }
  
  EventMatch&
  EventMatch::afterMatch (string regExp)
  {
    attachNextSerchStep (solution_, findRegExp(regExp), BACKWARD);
    evaluateQuery ("find-RegExp(\""+regExp+"\")", "before");
    return *this;
  }
  
  EventMatch&
  EventMatch::afterEvent (string match)
  {
    attachNextSerchStep (solution_, findEvent(match), BACKWARD);
    evaluateQuery ("match-event(\""+match+"\")", "before");
    return *this;
  }
  
  EventMatch&
  EventMatch::afterEvent (string classifier, string match)
  {
    attachNextSerchStep (solution_, findEvent(classifier,match), BACKWARD);
    evaluateQuery ("match-event(ID=\""+classifier+"\", \""+match+"\")", "before");
    return *this;
  }
  
  /**
   * find a function invocation backwards, before the current point of reference
   */
  EventMatch&
  EventMatch::afterCall (string match)
  {
    attachNextSerchStep (solution_, findCall(match), BACKWARD);
    evaluateQuery ("match-call(\""+match+"\")", "before");
    return *this;
  }
  
  
  
  /**
   * @internal refine filter condition additionally to match a specific positional call argument.
   */
  void
  EventMatch::refineSerach_matchArgument (size_t idx, string match)
  {
    refineSerach (solution_, matchArgument(idx, match));
    evaluateQuery ("match-argument(["+util::toString(idx)+"]="+match+")");
  }
  
  /**
   * @internal refine filter condition additionally to match a sequence of call arguments.
   */
  void
  EventMatch::refineSerach_matchArguments (ArgSeq&& argSeq)
  {
    string argList(util::join(argSeq));
    refineSerach (solution_, matchArguments(move(argSeq)));
    evaluateQuery ("match-arguments("+argList+")");
  }
  
  /**
   * @internal refine filter condition additionally to cover call arguments by
   *    a sequence of regular expressions. Each regExp is used consecutively,
   *    until it fails to match; at that point, the next regExp is tried.
   *    When reaching the end of the argument sequence, we also expect
   *    to have used up all available regular expressions.
   */
  void
  EventMatch::refineSerach_matchArgsRegExp (RExSeq&& regExpSeq, string rendered_regExps)
  {
    refineSerach (solution_, matchArgsRegExp (move (regExpSeq)));
    evaluateQuery ("match-args-RegExp("+rendered_regExps+")");
  }
  
  
  
  /**
   * refine filter to additionally require a matching log entry type
   */
  EventMatch&
  EventMatch::type (string typeID)
  {
    refineSerach (solution_, matchType(typeID));
    evaluateQuery ("match-type("+typeID+")");
    return *this;
  }
  
  /**
   * refine filter to additionally require the presence an attribute
   */
  EventMatch&
  EventMatch::key (string key)
  {
    refineSerach (solution_, ensureAttribute(key));
    evaluateQuery ("ensure-attribute("+key+")");
    return *this;
  }
  
  /**
   * refine filter to additionally match on a specific attribute
   */
  EventMatch&
  EventMatch::attrib (string key, string valueMatch)
  {
    refineSerach (solution_, matchAttribute(key,valueMatch));
    evaluateQuery ("match-attribute("+key+"=\""+valueMatch+"\")");
    return *this;
  }
  
  /**
   * refine filter to additionally match on the ID attribute
   */
  EventMatch&
  EventMatch::id (string classifier)
  {
    refineSerach (solution_, matchAttribute("ID",classifier));
    evaluateQuery ("match-ID(\""+classifier+"\")");
    return *this;
  }
  
  /**
   * refine filter to additionally match the `'this'` attribute
   */
  EventMatch&
  EventMatch::on (string targetID)
  {
    refineSerach (solution_, matchAttribute("this",targetID));
    evaluateQuery ("match-this(\""+targetID+"\")");
    return *this;
  }
  
  EventMatch&
  EventMatch::on (const char* targetID)
  {
    refineSerach (solution_, matchAttribute("this",targetID));
    evaluateQuery ("match-this(\""+string(targetID)+"\")");
    return *this;
  }

  
  
  
  
  /* ==== EventLog implementation ==== */

  EventLog::EventLog (string logID)
    : log_(new Log)
    {
      log_->reserve(2000);
      log({"type=EventLogHeader", "this="+logID});
    }
  
  
  /**
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
   */
  EventLog&
  EventLog::joinInto (EventLog& otherLog)
  {
    Log& target = *otherLog.log_;
    target.reserve (target.size() + log_->size() + 1);
    target.emplace_back (log_->front());
    auto p = log_->begin();
    while (++p != log_->end()) // move our log's content into the other log
      target.emplace_back (std::move(*p));
    this->log_->resize(1);
    this->log({"type=joined", otherLog.getID()});   // leave a tag to indicate
    otherLog.log({"type=logJoin", this->getID()}); //  where the `joinInto` took place,
    this->log_ = otherLog.log_;                   //   connect this to the other storage
    return *this;
  }
  
  
  
  EventLog&
  EventLog::clear()
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
  EventLog::clear (string alteredLogID)
  {
    log_.reset (new Log);
    log({"type=EventLogHeader", "this="+alteredLogID});
    return *this;
  }
  
  EventLog&
  EventLog::clear (const char* alteredLogID)
  {
    return clear (string{alteredLogID});
  }
  
  
  
  /* ==== Logging API ==== */
  
  EventLog&
  EventLog::event (string text)
  {
    log ("event", ArgSeq{}, ArgSeq{text});  // we use this ctor variant to ensure
    return *this;                          //  that text is not misinterpreted as attribute,
  }                                       //   which might happen when text contains a '='
  
  EventLog&
  EventLog::event (string classifier, string text)
  {
    log ("event", ArgSeq{"ID="+classifier}, ArgSeq{text});
    return *this;
  }
  
  EventLog&
  EventLog::call (string target, string function)
  {
    return call(target, function, ArgSeq());
  }
  
  EventLog&
  EventLog::call (string target, string function, ArgSeq&& args)
  {
    log ("call", ArgSeq{"fun="+function, "this="+target}, std::forward<ArgSeq>(args));
    return *this;
  }
  
  EventLog&
  EventLog::call (const char* target, const char* function, ArgSeq&& args)
  {
    return call (string(target), string(function), std::forward<ArgSeq>(args));
  }
  
  
  EventLog&
  EventLog::warn (string text)
  {
    log({"type=warn", text});
    return *this;
  }
  
  EventLog&
  EventLog::error (string text)
  {
    log({"type=error", text});
    return *this;
  }

  EventLog&
  EventLog::fatal (string text)
  {
    log({"type=fatal", text});
    return *this;
  }

  
  
  EventLog&
  EventLog::create (string text)
  {
    log({"type=create", text});
    return *this;
  }
  
  EventLog&
  EventLog::destroy (string text)
  {
    log({"type=destroy", text});
    return *this;
  }
  
  
  
  
  /* ==== Query/Verification API ==== */
  
  EventMatch
  EventLog::verify (string match)  const
  {
    EventMatch matcher(*log_);
    matcher.locate (match);   // new matcher starts linear search from first log element
    return matcher;
  }
  
  
  EventMatch
  EventLog::verifyMatch (string regExp)  const
  {
    EventMatch matcher(*log_);
    matcher.locateMatch (regExp);
    return matcher;
  }
  
  
  EventMatch
  EventLog::verifyEvent (string match)  const
  {
    EventMatch matcher(*log_);
    matcher.locateEvent (match);
    return matcher;
  }
  
  
  EventMatch
  EventLog::verifyEvent (string classifier, string match)  const
  {
    EventMatch matcher(*log_);
    matcher.locateEvent (classifier, match);
    return matcher;
  }
  
  
  EventMatch
  EventLog::verifyCall (string match)  const
  {
    EventMatch matcher(*log_);
    matcher.locateCall (match);
    return matcher;
  }
  
  
  /**
   * @remarks the query expression is built similar to the other queries,
   *          but the logic of evaluation is flipped: whenever we find any match
   *          the overall result (when evaluating to `bool`) will be `false`.
   * @note    since the chained search involves backtracking, this should perform
   *          an exhaustive negation, searching for a possible match until no
   *          further search options are left.
   */
  EventMatch
  EventLog::ensureNot (string match)  const
  {
    EventMatch matcher(*log_);
    matcher.look_for_match_ = false; // flip logic; fail if match succeeds
    matcher.locate (match);
    return matcher;
  }
  
  
  
}} // namespace lib::test
