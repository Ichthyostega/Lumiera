/*
  AbstractTangible(Test)  -  cover the operations of any tangible UI element

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

* *****************************************************/


/** @file abstract-tangible-test.cpp
 ** Verify the common base shared by all interface elements of relevance.
 ** This test is not so much a test, than a test of the test support for testing
 ** [primary elements](\ref gui::model::Tangible) of the Lumiera GTK UI. Any such element
 ** is connected to the [UI-Bus](\ref gui::UiBus) and responds to some generic actions and
 ** interaction patterns. This is the foundation of any presentation state recording
 ** and restoration, and it serves to invoke any persistent action on the
 ** [Session] through a single channel and access point.
 ** 
 ** What is covered here is actually a **test mock**. Which in turn enables us
 ** to cover interface interactions and behaviour in a generic fashion, without
 ** actually having to operate the interface.
 ** 
 ** @note as of 11/2015 this is a draft into the blue...
 ** @todo WIP  ///////////////////////TICKET #959
 ** @todo WIP  ///////////////////////TICKET #956
 ** @todo WIP  ///////////////////////TICKET #975
 ** @todo WIP  ///////////////////////TICKET #961 : tests to pass...
 ** 
 ** @see gui::UiBus
 ** 
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/event-log.hpp"
#include "test/mock-elm.hpp"
#include "test/test-nexus.hpp"
#include "lib/idi/entry-id.hpp"
#include "proc/control/command-def.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"
#include "lib/symbol.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"

#include <sigc++/signal.h>


using lib::Symbol;
using util::isnil;
using util::toString;
using lib::time::Time;
using gui::test::MockElm;
using lib::test::EventLog;
using lib::idi::EntryID;
using lib::diff::Rec;
using lib::diff::GenNode;
using lib::diff::DataCap;
using proc::control::Command;
using proc::control::CommandDef;
using gui::interact::InvocationTrail;



namespace gui  {
namespace model{
namespace test {
  
  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
  using ID = lib::idi::BareEntryID const&;
  
  namespace { // test fixture...
    
    // dummy operation to be invoked through the command system
    int dummyState = 0;
    
    void
    operate (int val)
    {
      dummyState = val;
    }
    
    int
    capture (int)
    {
      return dummyState;
    }
    
    void
    undoIt (int, int oldState)
    {
      dummyState = oldState;
    }
    
    
    
    const Symbol DUMMY_CMD_ID{"test.AbstractTangibleTest_dummy_command"};
    
    
    /**
     * dummy Command handler,
     * which can be hooked up to the TestNexus
     * and causes a real command invocation
     * when receiving invocation messages
     * @warning all hard wired -- works only for this command
     */
    void
    processCommandInvocation (GenNode const& commandMsg)
    {
      REQUIRE (string(DUMMY_CMD_ID) == commandMsg.idi.getSym());
      
      class CommandBindingDetector
        : public DataCap::Predicate
        {
          bool
          handle (Rec const& binding) override
            {
              auto cmd = Command::get(DUMMY_CMD_ID);
              auto arg = binding.scope()->data.get<int>();
              
              cmd.bind (arg);
              return true;
            }
          
          bool
          handle (int const&) override
            {
              auto cmd = Command::get(DUMMY_CMD_ID);
              
              cmd();
              return true;
            }
        }
        commandInvoker;
      
      commandMsg.data.accept (commandInvoker);
    }
    
    
  }//(End) test fixture
  
  
  
  
  
  
  /***********************************************************************//**
   * @test cover the basic operations of any tangible UI element,
   *       with the help of a mock UI element.
   *       - creation
   *       - destruction
   *       - command invocation
   *       - state mark
   *       - state mark replay
   *       - message casting
   *       - error state indication
   *       
   * @see BusTerm_test
   * @see SessionElementQuery_test
   * @see tangible.hpp
   * @see ui-bus.hpp
   */
  class AbstractTangible_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verify_mockManipulation();
          invokeCommand();
          markState();
          notify();
          mutate();
        }
      
      
      /** @test verify the UI widget unit test support framework.
       * The generic backbone of the Lumiera UI offers a mock UI element,
       * with the ability to stand-in for actual elements present in the real GUI.
       * This allows us to rig a emulated test user interface to cover interactions
       * involving some communication from or to interface elements. After setting up
       * a [mock UI-element](\ref MockElm) with a suitable name / ID, we're able to
       * operate this element programmatically and to send messages and responses
       * from the core "up" to this mocked interface. And since this mock element
       * embodies an [event log](\ref EventLog), the unit test code can verify
       * the occurrence of expected events, invocations and responses.
       * 
       * ### connectivity
       * Any mock element will automatically connect against the [Test-Nexus](test/test-nexus.hpp),
       * so to be suitably rigged for unit testing. This means, there is no _live connection_
       * to the session, but any command- or other messages will be captured and can be
       * retrieved or verified from the test code. Since lifecycle and robustness in
       * "post mortem" situations tend to be tricky for UI code, we provide a dedicated
       * ["zombification" feature](\ref gui::test::TestNexus::zombificate): a \ref MockElm can be
       * turned into an _almost dead_ state, while still hanging around. It will be detached from the
       * "living" Test-Nexus and re-wired to some special, hidden "Zombie Nexus", causing any
       * further messaging activity to be logged and ignored.
       */
      void
      verify_mockManipulation ()
        {
          MARK_TEST_FUN
          MockElm mock("dummy");
          
          CHECK (mock.verify("ctor"));
          CHECK (mock.verifyEvent("create","dummy"));
          CHECK (mock.verify("ctor").arg("dummy","TestNexus").on(&mock));
          
          CHECK ("dummy" == mock.getID().getSym());
          CHECK (EntryID<MockElm>("dummy") == mock.getID());
          
          CHECK (!mock.verifyCall("reset"));
          
          // start manipulating state....
          mock.slotExpand();
          CHECK (mock.isExpanded());
          
          mock.reset();
          CHECK (mock.verify("reset"));
          CHECK (mock.verifyCall("reset"));
          CHECK (mock.verifyCall("reset").on(&mock));
          CHECK (mock.verifyCall("reset").on("dummy"));
          CHECK (mock.verifyEvent("reset"));
          CHECK (mock.verify("reset").after("ctor"));
          CHECK (mock.verify("ctor").before("reset"));
          CHECK (mock.ensureNot("reset").before("ctor"));
          CHECK (mock.ensureNot("ctor").after("reset"));
          
          CHECK (mock.verify("reset").beforeEvent("reset"));
          CHECK (mock.verifyCall("reset").beforeEvent("reset"));
          CHECK (!mock.verifyCall("reset").afterEvent("reset"));
          
          CHECK (!mock.isTouched());
          CHECK (!mock.isExpanded());
          
          mock.markMsg("qui dolorem ipsum quia dolor sit amet consectetur adipisci velit.");
          CHECK (mock.verifyMark("Message", "dolor"));
          CHECK (mock.verifyCall("doMsg"));
          CHECK (mock.verifyCall("doMsg").arg("lorem ipsum"));
          CHECK (mock.verifyCall("doMsg").argMatch("dolor.+dolor\\s+"));
          CHECK (mock.verifyMatch("Rec\\(mark.+ID = Message.+\\{.+lorem ipsum"));
          
          EventLog log = mock.getLog();
          log.verify("ctor")
             .before("reset")
             .before("lorem ipsum");
          
          // create further mock elements...
          MockElm foo("foo"), bar("bar");
          foo.verify("ctor").arg("foo");
          bar.verify("ctor").arg("bar");
          
          bar.ensureNot("foo");
          log.ensureNot("foo");
          mock.ensureNot("foo");
          CHECK (!foo.ensureNot("foo"));
          
          // now join the logs together,
          // allowing to watch the combined events
          bar.joinLog(mock);
          foo.joinLog(mock);
          CHECK (log.verifyEvent("logJoin","bar")
                    .beforeEvent("logJoin","foo"));
          
          CHECK (mock.verifyEvent("logJoin","bar")
                     .beforeEvent("logJoin","foo"));
          CHECK (mock.verifyEvent("create","foo"));
          CHECK (log.verifyEvent("create","foo"));
          CHECK (log.verifyEvent("create","dummy")
                    .beforeEvent("create","bar")
                    .beforeEvent("create","foo"));
          
          
          mock.kill();
          foo.markMsg("dummy killed");
          CHECK (log.verifyEvent("destroy","dummy")
                    .beforeCall("doMsg").on("foo"));
          
          // Access the log on the Test-Nexus hub
          EventLog nexusLog = gui::test::Nexus::getLog();
          CHECK (nexusLog.verifyEvent("destroy","dummy")
                         .beforeEvent("dummy successfully zombificated"));
          
          mock.slotExpand(); // attempt to operate the zombie
          CHECK (nexusLog.verifyEvent("dummy successfully zombificated")
                         .beforeCall("note").on("ZombieNexus").arg("defunct-dummy", "expand")
                         .beforeEvent("error","sent note message to ZombieNexus"));
          
          
          cout << "____Event-Log_________________\n"
               << util::join(mock.getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          
          cout << "____Nexus-Log_________________\n"
               << util::join(gui::test::Nexus::getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
        }
      
      
      
      void
      invokeCommand ()
        {
          MARK_TEST_FUN
          EventLog nexusLog = gui::test::Nexus::startNewLog();
          
          // Setup test stage: define an command/action "in Proc"
          CommandDef (DUMMY_CMD_ID)
              .operation (operate)
              .captureUndo (capture)
              .undoOperation (undoIt);
          
          gui::test::Nexus::setCommandHandler (&processCommandInvocation);
          
          // Usually it's the InvocationStateManager's job to
          // prepare an "InvocationTrail", which is a prospective
          // Command invocation about to happen soon.
          InvocationTrail invTrail (Command::get (DUMMY_CMD_ID));
          
          // the UI element relevant for this command invocation
          MockElm mock("uiElm");
          
          int prevState = dummyState;
          int concreteParam = 1 +rand() % 100;
          
          // on bus no traces from this command yet...
          CHECK (nexusLog.ensureNot(string(DUMMY_CMD_ID)));
          
          
          // now the ongoing interaction picks up parameter data
          mock.prepareCommand (invTrail, lib::diff::Rec({concreteParam}));
          CHECK (nexusLog.verifyCall("act").arg("«int»|" +toString(concreteParam))
                         .beforeEvent("binding for command \""+DUMMY_CMD_ID));
          
          CHECK (dummyState == prevState);  // command was not yet invoked
          CHECK (nexusLog.ensureNot("invoke command \""+DUMMY_CMD_ID));
          
          
          // finally the command gets triggered
          mock.issueCommand (invTrail);
          
          CHECK (dummyState == concreteParam);  // command was indeed invoked
          CHECK (nexusLog.verifyCall("act").arg("«int»|" +toString(concreteParam))
                         .beforeCall("act").arg("DataCap|«int»|" +toString(int(InvocationTrail::DO_IT))));
          CHECK (nexusLog.verifyEvent("binding for command \""+DUMMY_CMD_ID)
                         .beforeEvent("invoke command \""+DUMMY_CMD_ID));
          
          
          // verify proper binding, including UNDO state capture
          Command::get (DUMMY_CMD_ID).undo();
          CHECK (dummyState == prevState);
          
          cout << "____Nexus-Log_________________\n"
               << util::join(nexusLog, "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          
          
          // reset to default (NOP) handler
          gui::test::Nexus::setCommandHandler();
        }
      
      
      /** @test mark interface state.
       * This test case performs an elementary UI operation, namely to
       * expand / collapse an element, to verify both directions of state marking.
       * Here »state marking« is a mechanism, where UI state changes get recorded
       * at some central StateManager, to be able to restore interface state later.
       * Thus, when we'll expand and collapse the mock, we expect the corresponding
       * "state mark" notifications to appear at the UI-Bus.
       * 
       * The second part of this test _replays_ such a state mark, which causes
       * the`doMark()` operation on the UI element to be invoked.
       * @todo maybe we'll even provide a default implementation for expand/collapse
       *       which then means that, by replaying the mentioned state marks, the
       *       `doExpand()` or `doCollapse()` should be re-invoked, of course
       *       without issuing a further notification
       * @note this test does not cover or even emulate the operation of the
       *       "state manager", since the goal is to cover the _UI element_
       *       protocol. We'll just listen at the bus and replay messages.
       */
      void
      markState ()
        {
          MARK_TEST_FUN
          EventLog nexusLog = gui::test::Nexus::startNewLog();
          
          sigc::signal<void> trigger_expand;
          sigc::signal<void> trigger_collapse;
          
          MockElm mock("target");
          ID targetID = mock.getID();
          
          trigger_expand.connect   (sigc::mem_fun(mock, &Tangible::slotExpand));
          trigger_collapse.connect (sigc::mem_fun(mock, &Tangible::slotCollapse));
          
          CHECK (not mock.isTouched());
          CHECK (not mock.isExpanded());
          CHECK (mock.ensureNot("expanded"));
          CHECK (nexusLog.ensureNot("state-mark"));
          
          trigger_expand(); // emit signal
          
          CHECK (mock.isTouched());
          CHECK (mock.isExpanded());
          CHECK (mock.verifyCall("expand").arg(true)
                     .beforeEvent("expanded"));
          
          // and now the important part: state mark notification was sent over the bus...
          CHECK (nexusLog.verifyCall("note").arg(targetID, GenNode{"expand", true})
                         .before("handling state-mark"));
          
          
          trigger_collapse(); // emit other signal
          CHECK (not mock.isExpanded());
          CHECK (mock.isTouched());
          
          CHECK (mock.verifyEvent("create", "target")
                     .beforeEvent("expanded")
                     .beforeEvent("collapsed"));
          CHECK (nexusLog.verifyCall("note").arg(targetID, GenNode{"expand", true})
                         .before("handling state-mark")
                         .beforeCall("note").arg(targetID, GenNode{"expand", false})
                         .before("handling state-mark"));
          
          trigger_collapse();
          CHECK (not mock.isExpanded());
          
          // but note: redundant state changes do not cause sending of further state marks
          CHECK (mock.ensureNot("collapsed")
                     .beforeCall("expand")
                     .beforeEvent("collapsed"));
          CHECK (nexusLog.ensureNot("handling state-mark")
                         .beforeCall("note").arg(targetID, GenNode{"expand", false})
                         .before("handling state-mark")
                         .beforeCall("note").arg(targetID, GenNode{"expand", false}));
          
          
          
          // Second part: replay of a state mark via UI-Bus....
          auto stateMark = GenNode{"expand", true};
          auto& uiBus = gui::test::Nexus::testUI();
          
          CHECK (not mock.isExpanded());
          CHECK (mock.ensureNot("mark"));
          
          uiBus.mark (targetID, stateMark);
          
          CHECK (nexusLog.verifyCall("mark").arg(targetID, stateMark)
                         .before("delivered mark to "+string(targetID)).arg(stateMark));
          
          CHECK (mock.verifyMark("expand", "true")
                     .beforeCall("expand").arg(true)
                     .beforeEvent("expanded"));
          CHECK (mock.isExpanded());
          CHECK (mock.isTouched());
          
          // the default handler defined in model::Tangible
          // already supports some rather generic state changes,
          // like e.g. a reset to the element's default state.
          // Note that the actual implementation doReset()
          // is a virtual function, here implemented in MockElm.
          uiBus.mark (targetID, GenNode{"reset", true});
                                              // note: payload is irrelevant for "reset" mark
          
          // and we're back to pristine state...
          CHECK (not mock.isTouched());
          CHECK (not mock.isExpanded());
          CHECK (mock.verifyMark("reset", "true")
                     .afterEvent("expanded")
                     .beforeCall("reset")
                     .beforeEvent("reset"));
          
          
          cout << "____Event-Log_________________\n"
               << util::join(mock.getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          
          cout << "____Nexus-Log_________________\n"
               << util::join(nexusLog, "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
        }
      
      
      /** @test receive various kinds of notifications
       * Send message, error and flash messages via Bus to the element
       * and verify the doMsg, doErr or doFlash handlers were invoked.
       */
      void
      notify ()
        {
          MARK_TEST_FUN
          EventLog nexusLog = gui::test::Nexus::startNewLog();
          
          MockElm mock("target");
          ID targetID = mock.getID();
          auto& uiBus = gui::test::Nexus::testUI();
          
          CHECK (mock.ensureNot("Flash"));
          CHECK (mock.ensureNot("Error"));
          CHECK (mock.ensureNot("Message"));
          CHECK (isnil(mock.getMessage()));
          CHECK (isnil(mock.getError()));
          CHECK (not mock.isError());
          
          // now send a "Flash" mark via UI-Bus....
          uiBus.mark (targetID, GenNode{"Flash", true });
          CHECK (mock.verifyMark("Flash"));
          
          CHECK (mock.ensureNot("Error"));
          CHECK (mock.ensureNot("Message"));
          CHECK (isnil(mock.getMessage()));
          CHECK (isnil(mock.getError()));
          
          uiBus.mark (targetID, GenNode{"Error", "getting serious"});
          CHECK (mock.verifyMark("Error", "serious"));
          CHECK (mock.isError());
          CHECK ("getting serious" == mock.getError());
          CHECK (isnil(mock.getMessage()));
          
          uiBus.mark (targetID, GenNode{"Message", "by mistake"});
          CHECK (mock.verifyMark("Message", "mistake"));
          CHECK ("by mistake" == mock.getMessage());
          CHECK ("getting serious" == mock.getError());
          
          CHECK (mock.verify("target")
                     .before("Flash")
                     .before("serious")
                     .before("mistake"));
          
          // type mismatch: when receiving a "Message" mark, we expect a string payload
          VERIFY_ERROR(WRONG_TYPE, uiBus.mark(targetID, GenNode{"Message", Time::NEVER }))
          
          // the type error happens while resolving the payload,
          // and thus the actual "doMsg()" function on the target was not invoked
          CHECK (mock.ensureNot(string(Time::NEVER)));
          CHECK (nexusLog.verifyCall("mark").arg(targetID, Time::NEVER));
          CHECK (nexusLog.ensureNot("delivered mark").arg(Time::NEVER));
          CHECK ("getting serious" == mock.getError());
          
          mock.reset();
          CHECK (isnil(mock.getMessage()));
          CHECK (isnil(mock.getError()));
          CHECK (not mock.isError());
          
          
          cout << "____Event-Log_________________\n"
               << util::join(mock.getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          
          cout << "____Nexus-Log_________________\n"
               << util::join(nexusLog, "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
        }
      
      
      void
      mutate ()
        {
          UNIMPLEMENTED ("mutate the element by diff message");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (AbstractTangible_test, "unit gui");
  
  
}}} // namespace gui::model::test
