/*
  EventLog(Test)  -  helper for event registration and verification

   Copyright (C)
     2015,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file event-log-test.cpp
 ** unit test \ref EventLog_test
 */


#include "lib/test/run.hpp"
#include "lib/format-util.hpp"
#include "lib/test/event-log.hpp"
#include "lib/util.hpp"

#include <string>

using util::join;
using util::isnil;

using std::string;


namespace lib {
namespace test{
namespace test{
  
  
  
  
  
  /***********************************************************//**
   * @test verify a logging facility, which can be used to ensure
   *       some events happened while running test code.
   *       - various kinds of events or function calls
   *         are logged via the logging API.
   *       - within the test code, a match is performed against
   *         the contents of the log, using a DSL to represent
   *         matches relative to other matches
   *       - when a match fails, additional diagnostics are
   *         printed to STDERR
   * @see event-log.hpp
   */
  class EventLog_test : public Test
    {
      void
      run (Arg)
        {
          verify_simpleUsage();
          verify_backwardMatch();
          verify_negatedMatch();
          verify_logJoining();
          verify_callLogging();
          verify_eventLogging();
          verify_genericLogging();
          verify_regExpMatch();
          verify_logPurging();
        }
      
      
      void
      verify_simpleUsage ()
        {
          EventLog log(this);
          CHECK (isnil (log));
          
          log.event("α");
          log.event("β");
          CHECK (!isnil(log));
          
          CHECK (log.verify("α"));
          CHECK (log.verify("β"));
          CHECK (not log.verify("γ"));
          
          CHECK (log.verify("α").before("β"));
          CHECK (not log.verify("β").before("α"));
          
          CHECK (join(log) == "Rec(EventLogHeader| this = "+idi::instanceTypeID(this)+" ), "
                           +  "Rec(event|{α}), "
                           +  "Rec(event|{β})");
        }
      
      
      void
      verify_backwardMatch ()
        {
          EventLog log("baked beans");
          log.event("spam");
          log.event("ham");
          
          CHECK (log.verify("ham").after("spam").after("beans"));
          CHECK (log.verify("ham").after("beans").before("spam").before("ham"));
          CHECK (not log.verify("spam").after("beans").after("ham"));
          
          log.event("beans");
          CHECK (log.verify("beans").after("spam"));   // Note: Backtracking! The first match on beans fails,
                                                       //       only the match on second beans succeeds.
          
          // consecutive matches always move by at least one step
          CHECK (    log.verify("beans").after("ham").after("spam")              .after("baked"));
          CHECK (not log.verify("beans").after("ham").after("spam").after("spam").after("baked"));
          CHECK (    log.verify("beans").after("ham").after("spam").locate("spam").locate("spam").after("baked"));
        }                                                        // ^^^^^^ locate re-applies at current pos without moving
      
      
      void
      verify_negatedMatch ()
        {
          EventLog log("eggs");
          log.event("spam");
          log.event("ham");
          log.event("spam");
          
          CHECK (log.ensureNot("baked beans"));
          CHECK (log.ensureNot("ham").before("eggs"));
          CHECK (log.ensureNot("spam").after("spam").before("eggs"));
          CHECK (not log.ensureNot("spam").before("spam").after("eggs").before("ham"));
        }
      
      
      /** @test combining several logs
       * The EventLog objects are actually just lightweight front-end handles,
       * while the actual log lives on the Heap. This allows to have several handles
       * hold onto the same actual log; this way, we can access and verify logs
       * even after the managing object is destroyed.
       * 
       * The "log joining" functionality covered here is just an obvious extension
       * to this setup: it allows to attach one log to another log after the fact;
       * the contents of the joined log are integrated into the target log.
       * @remarks this functionality is "low hanging fruit" -- not sure if it's useful.
       */
      void
      verify_logJoining ()
        {
          EventLog log1("spam");
          EventLog log2("ham");
          
          log1.event("baked beans");
          log2.event("eggs");
          
          CHECK (log1.verify("spam").before("baked beans"));
          CHECK (log2.verify("ham").before("eggs"));
          
          CHECK (log1.ensureNot("ham"));
          CHECK (log1.ensureNot("eggs"));
          CHECK (log2.ensureNot("spam"));
          CHECK (log2.ensureNot("baked beans"));
          
          EventLog copy(log2);
          copy.event("bacon");
          CHECK (copy.verify("ham").before("eggs").before("bacon"));
          CHECK (log2.verify("ham").before("eggs").before("bacon"));
          CHECK (log1.ensureNot("bacon"));
          
          CHECK (log1 != log2);
          CHECK (copy == log2);
          
          log2.joinInto(log1);
          
          CHECK (log1 == log2);
          CHECK (copy != log2);
          
          CHECK (log1.verify("logJoin|{ham}").after("baked beans"));
          CHECK (log1.verify("logJoin|{ham}").after("EventLogHeader| this = ham").before("eggs").before("bacon").before("logJoin"));
          
          log2.event("sausage");
          CHECK (log1.verify("sausage").after("logJoin").after("spam"));
          
          CHECK (copy.ensureNot("logJoin"));
          CHECK (copy.ensureNot("sausage"));
          CHECK (copy.verify("joined|{spam}").after("EventLogHeader"));
          
          copy.event("spam tomato");
          CHECK (log1.ensureNot("spam tomato"));
          CHECK (log2.ensureNot("spam tomato"));
          CHECK (copy.verify("joined|{spam}").before("spam tomato"));
          
          
          CHECK (join(log1) == string(
                               "Rec(EventLogHeader| this = spam ), "
                               "Rec(event|{baked beans}), "
                               "Rec(EventLogHeader| this = ham ), "
                               "Rec(event|{eggs}), "
                               "Rec(event|{bacon}), "
                               "Rec(logJoin|{ham}), "
                               "Rec(event|{sausage})"));
          
          CHECK (join(copy) == string(
                               "Rec(EventLogHeader| this = ham ), "
                               "Rec(joined|{spam}), "
                               "Rec(event|{spam tomato})"));
        }
      
      
      void
      verify_callLogging ()
        {
          EventLog log("funCall");
          log.call (this, "fun1");
          log.call ("some", "fun2");
          log.call ("more", "fun3", "facts", 3.2,1);
          
          CHECK(log.verify("fun1").before("fun2").before("fun3"));
          
          CHECK (join(log) == string(
                              "Rec(EventLogHeader| this = funCall ), "
                              "Rec(call| fun = fun1, this = "+idi::instanceTypeID(this)+" ), "
                              "Rec(call| fun = fun2, this = some ), "
                              "Rec(call| fun = fun3, this = more |{facts, 3.2, 1})"));
          
          CHECK (log.verifyCall("fun1"));
          CHECK (log.verifyCall("fun2"));
          CHECK (log.verifyCall("fun3"));
          
          CHECK (log.verifyCall("fun"));
          CHECK (log.verifyCall("fun").after("fun").after("fun"));
          CHECK (log.ensureNot("fun").after("fun").after("fun2"));
          
          CHECK (log.verifyCall("fun3").arg("facts", 3.2, 1));
          CHECK (log.verifyCall("fun3").arg(string("facts"), 3.2f, int64_t(1)));
          CHECK (log.verifyCall("fun3").arg("facts", "3.2", "1"));
          CHECK (log.verifyCall("fun3").argPos(0, "facts"));
          CHECK (log.verifyCall("fun3").argPos(0, "act"));
          CHECK (log.verifyCall("fun3").argPos(1, ".2"));
          CHECK (log.verifyCall("fun3").argPos(1, 3.2));
          CHECK (log.verifyCall("fun3").argPos(2, 1u));
          
          CHECK (log.ensureNot("fun").arg(" facts ","3.2", "1"));     // the match is on the exact textual representation...
          CHECK (log.ensureNot("fun").arg("facts",  "3.20","1"));
          CHECK (log.ensureNot("fun").arg("facts",  "3.2", "1L"));
          CHECK (log.ensureNot("fun").argPos(1, "anything"));         // matches first invocation, which has no arguments
          CHECK (log.ensureNot("fun3").argPos(3, 5555));              // the "fun3" invocation has only 3 arguments
          CHECK (log.ensureNot("fun3").argPos(1, 3.3));               // the second argument is 2.3, not 3.3
          CHECK (log.ensureNot("fun3").argPos(2, 5));                 // the last argument is 1, not 5
          
          CHECK (log.verifyCall("fun1").arg());
          CHECK (log.verifyCall("fun2").arg());
          
          CHECK (log.verify("fun").arg().before("fun").arg("facts", 3.2, 1));
          
          CHECK (log.verify("fun").on(this));
          CHECK (log.verify("fun").on("some"));
          CHECK (log.verify("fun").on("more"));
          CHECK (log.verify("fun").on("more").on("more"));
          CHECK (log.ensureNot("fun").on("some").on("more"));
          
          CHECK (log.verify("fun").on("some").arg());
          CHECK (log.ensureNot("fun").arg().on("more"));
          CHECK (log.ensureNot("fun").on("some").arg("facts", "3.2", "1"));
          CHECK (log.verifyCall("fun").arg("facts", "3.2", "1").on("more"));
        }
      
      
      void
      verify_eventLogging ()
        {
          EventLog log("event trace");
          log.event("no","fun");
          log.call("some","fun");
          
          CHECK (log.verify("fun").before("fun"));
          CHECK (log.verify("no").before("some"));
          
          CHECK (log.verifyEvent("fun").beforeCall("fun").on("some"));
          CHECK (!log.verifyEvent("fun").after("some"));
          
          CHECK (log.verifyEvent("no","fun"));
          CHECK (log.verifyEvent("fun").id("no"));
          CHECK (log.verify("no").arg("fun"));
          
          CHECK (join(log) == string(
                              "Rec(EventLogHeader| this = event trace ), "
                              "Rec(event| ID = no |{fun}), "
                              "Rec(call| fun = fun, this = some )"));
        }
      
      
      void
      verify_genericLogging ()
        {
          EventLog log("theHog");
          log.note ("type=some","ID=weird","stuff");
          log.warn ("danger");
          log.error ("horrible");
          log.fatal ("destiny");
          log.create ("something");
          log.destroy ("everything");
          
          CHECK (log.verify("theHog")
                    .before("stuff")
                    .before("danger")
                    .before("horrible")
                    .before("destiny")
                    .before("something")
                    .before("everything"));
          CHECK (log.verify("this").type("EventLogHeader")
                    .before("weird").type("some")
                    .before("danger").type("warn")
                    .before("horrible").type("error")
                    .before("destiny").type("fatal")
                    .before("something").type("create")
                    .before("everything").type("destroy"));
          
          CHECK (log.verify("some").attrib("ID","weird").arg("stuff"));
          
          // NOTE: there is some built-in leeway in event-matching...
          CHECK (log.verifyEvent("horrible").beforeEvent("something").beforeEvent("everything"));
          CHECK (!log.verifyEvent("stuff"));       // not every entry type is an event by default
          CHECK (!log.verifyEvent("danger"));      // warning is not an event by default
          CHECK (log.verifyEvent("some","stuff")); // but the classifier-param matches on the type
          CHECK (log.verifyEvent("weird","stuff"));
          CHECK (log.verifyEvent("warn","danger"));
          CHECK (log.verifyEvent("fatal","destiny"));
          CHECK (log.verifyEvent("destroy","everything"));
          
          CHECK (join(log) == string(
                              "Rec(EventLogHeader| this = theHog ), "
                              "Rec(some| ID = weird |{stuff}), "
                              "Rec(warn|{danger}), "
                              "Rec(error|{horrible}), "
                              "Rec(fatal|{destiny}), "
                              "Rec(create|{something}), "
                              "Rec(destroy|{everything})"));
        }
      
      
      void
      verify_regExpMatch ()
        {
          EventLog log("Lovely spam!");
          log.note ("type=spam", "egg and bacon"
                               , "egg sausage and bacon"
                               , "egg and spam"
                               , "egg bacon and spam"
                               , "egg bacon sausage and spam"
                               , "spam bacon sausage and spam"
                               , "spam egg spam spam bacon and spam"
                               , "spam sausage spam spam bacon spam tomato and spam");
          log.fatal("Lobster Thermidor a Crevette with a mornay sauce served in a Provencale manner "
                    "with shallots and aubergines garnished with truffle pate, brandy and with a fried egg on top and spam");
          
          CHECK (log.verify("spam").before("(spam|").before("egg on top and spam"));
          CHECK (log.verify("and spam").after("(spam|").after("spam!").before("bacon"));
          CHECK (log.ensureNot("and spam").after("(spam|").after("spam!").after("bacon"));
          
          // RegExp on full String representation
          CHECK (log.verifyMatch("spam.+spam"));
          CHECK (log.verifyMatch("spam.+spam").beforeMatch("spam(?!.+spam)"));
          CHECK (log.verifyEvent("fatal","spam").afterMatch("(spam.*){15}"));
          
          // Cover all arguments with sequence of regular expressions
          CHECK (log.verify("spam").argMatch("^egg ", "^spam .+spam$"));
          CHECK (log.verifyMatch("Rec.+fatal").afterMatch("\\{.+\\}").argMatch("bacon$","and spam$"));
          
          // argument match must cover all arguments...
          CHECK (log.ensureNot("spam").argMatch("bacon|^spam"));
        }
      
      
      void
      verify_logPurging ()
        {
          EventLog log("obnoxious");
          log.create("spam").create("spam").create("spam");
          CHECK (log.verify("spam").after("obnoxious"));
          
          log.clear();
          CHECK (log.ensureNot("spam"));
          CHECK (log.verify("obnoxious").type("EventLogHeader").on("obnoxious"));
          
          log.warn("eggs");
          log.clear("unbearable");
          CHECK (log.ensureNot("eggs"));
          CHECK (log.ensureNot("obnoxious"));
          CHECK (log.verify("unbearable").type("EventLogHeader").on("unbearable"));
        }
    };
  
  LAUNCHER (EventLog_test, "unit common");
  
  
}}} // namespace lib::test::test

