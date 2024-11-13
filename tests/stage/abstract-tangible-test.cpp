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
 ** [primary elements](\ref stage::model::Tangible) of the Lumiera GTK UI. Any such element
 ** is connected to the [UI-Bus](\ref stage::UiBus) and responds to some generic actions and
 ** interaction patterns. This is the foundation of any presentation state recording
 ** and restoration, and it serves to invoke any persistent action on the
 ** [Session] through a single channel and access point.
 ** 
 ** What is covered here is actually a **test mock**. Which in turn enables us
 ** to cover interface interactions and behaviour in a generic fashion, without
 ** actually having to operate the interface. But at the same time, this test
 ** documents our generic UI element protocol and the corresponding interactions.
 ** 
 ** @todo WIP  ///////////////////////TICKET #959 : GUI Model / Bus
 ** @todo WIP  ///////////////////////TICKET #956 : model diff representation
 ** @todo WIP  ///////////////////////TICKET #961 : tests to pass...
 ** 
 ** @see stage::UiBus
 ** 
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/event-log.hpp"
#include "test/mock-elm.hpp"
#include "test/test-nexus.hpp"
#include "lib/idi/genfunc.hpp"
#include "lib/idi/entry-id.hpp"
#include "steam/control/command-def.hpp"
#include "lib/diff/mutation-message.hpp"
#include "lib/diff/tree-diff.hpp"
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
using stage::test::MockElm;
using lib::test::EventLog;
using lib::idi::EntryID;
using lib::diff::Rec;
using lib::diff::MakeRec;
using lib::diff::GenNode;
using lib::diff::DataCap;
using lib::diff::MutationMessage;
using steam::control::Command;
using steam::control::CommandDef;



namespace stage {
namespace model {
namespace test  {
  
  using LERR_(WRONG_TYPE);
  using ID = lib::idi::BareEntryID const&;
  
  namespace { // test fixture...
    
    /* === dummy operation to be invoked through the command system === */
    
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
     * and causes a real command invocation on invocation message
     * @warning all hard wired -- works only for this command
     */
    void
    processCommandInvocation (GenNode const& commandMsg)
    {
      REQUIRE (string{DUMMY_CMD_ID} == commandMsg.idi.getSym());
      
      auto cmd = Command::get (DUMMY_CMD_ID);
      auto arg = commandMsg.data.get<Rec>().scope()->data.get<int>();
      cmd.bind (arg);
      cmd();
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
   *       - structural changes by MutationMessage
   * 
   * This test documents a generic interaction protocol supported by all
   * "tangible" elements of the Lumiera GTK UI. This works by connecting any
   * such element to a messaging backbone, the *UI-Bus*. By sending messages
   * according to this protocol, typical state changes can be detected and
   * later be replayed on elements addressed by ID. Moreover, the preconfigured
   * commands offered by the session can be invoked via bus message, and it is
   * possible to populate and change UI elements by sending a _tree diff message_
   * @note the actions in this test are verified with the help of an EventLog
   *       built into the mock UI element and the mock UI-Bus counterpart.
   *       Additionally, each test case dumps those log contents to STDOUT,
   *       which hopefully helps to understand the interactions in detail.
   * 
   * @see BusTerm_test
   * @see DiffTreeApplication_test
   * @see tangible.hpp
   * @see ui-bus.hpp
   */
  class AbstractTangible_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          
          verify_mockManipulation();
          invokeCommand();
          markState();
          revealer();
          notify();
          mutate();
        }
      
      
      /** @test verify the UI widget unit test support framework.
       * The generic backbone of the Lumiera UI offers a mock UI element,
       * with the ability to stand-in for actual elements present in the real GUI.
       * This allows us to rig an emulated test user interface to cover interactions
       * involving some communication from or to interface elements. After setting up
       * a [mock UI-element](\ref MockElm) with a suitable name / ID, we're able to
       * operate this element programmatically and to send messages and responses
       * from the core "up" to this mocked interface. And since this mock element
       * embodies an [event log](\ref EventLog), the unit test code can verify
       * the occurrence of expected events, invocations and responses.
       * 
       * \par connectivity
       * Any mock element will automatically connect against the [Test-Nexus](\ref test/test-nexus.hpp),
       * so to be suitably rigged for unit testing. This means, there is no _live connection_
       * to the session, but any command- or other messages will be captured and can be
       * retrieved or verified from the test code. Since lifecycle and robustness in
       * "post mortem" situations tend to be tricky for UI code, we provide a dedicated
       * ["zombification" feature](\ref stage::test::TestNexus::zombificate): a \ref MockElm can be
       * turned into an _almost dead_ state, while still hanging around. It will be detached from
       * the "living" Test-Nexus and re-wired to some special, hidden "Zombie Nexus", causing any
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
          EventLog nexusLog = stage::test::Nexus::getLog();
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
               << util::join(stage::test::Nexus::getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
        }
      
      
      
      void
      invokeCommand ()
        {
          MARK_TEST_FUN
          EventLog nexusLog = stage::test::Nexus::startNewLog();
          
          // Setup test stage: define a command/action "in Steam"
          CommandDef (DUMMY_CMD_ID)
              .operation (operate)
              .captureUndo (capture)
              .undoOperation (undoIt);
          
          stage::test::Nexus::setCommandHandler (&processCommandInvocation);
          
          // the UI element to trigger this command invocation
          MockElm mock("uiElm");
          
          int prevState = dummyState;
          int concreteParam = 1 + rani(100);
          
          // on bus no traces from this command yet...
          CHECK (nexusLog.ensureNot(string(DUMMY_CMD_ID)));
          
          
          // message to bind parameter data and finally trigger the command
          mock.invoke (DUMMY_CMD_ID, concreteParam);
          CHECK (dummyState == concreteParam);  // command was indeed invoked
          CHECK (nexusLog.verifyCall("act").arg("«int»|" +toString(concreteParam))
                         .beforeEvent("bind and trigger command \""+DUMMY_CMD_ID));
          
          // verify proper binding, including UNDO state capture
          Command::get (DUMMY_CMD_ID).undo();
          CHECK (dummyState == prevState);
          
          cout << "____Nexus-Log_________________\n"
               << util::join(nexusLog, "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          
          
          // reset to default (NOP) handler
          stage::test::Nexus::setCommandHandler();
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
       * @note this test does not cover or even emulate the operation of the
       *       "state manager", since the goal is to cover the _UI element_
       *       protocol. We'll just listen at the bus and replay messages.
       */
      void
      markState ()
        {
          MARK_TEST_FUN
          EventLog nexusLog = stage::test::Nexus::startNewLog();
          
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
          auto& uiBus = stage::test::Nexus::testUI();
          
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
      
      
      /** @test configure a handler for the (optional) "reveal yourself" functionality.
       * We install a lambda to supply the actual implementation action, which can then
       * either be triggered by a signal/slot invocation, or by sending a "state mark".
       */
      void
      revealer ()
        {
          MARK_TEST_FUN
          EventLog nexusLog = stage::test::Nexus::startNewLog();
          
          MockElm mock("target");
          ID targetID = mock.getID();
          
          sigc::signal<void> trigger_reveal;
          trigger_reveal.connect   (sigc::mem_fun(mock, &Tangible::slotReveal));
          
          CHECK (not mock.isTouched());
          CHECK (not mock.isExpanded());
          CHECK (mock.ensureNot("reveal"));
          CHECK (mock.ensureNot("expanded"));
          CHECK (nexusLog.ensureNot("state-mark"));
          
          bool revealed = false;
          mock.installRevealer([&]()
                                  {                        // NOTE: our mock "implementation" of the »reveal yourself« functionality
                                    mock.slotExpand();     //       explicitly prompts the element to expand itself,
                                    revealed = true;       //       and then via closure sets a flag we can verify.
                                  });
          
          trigger_reveal();
          
          CHECK (true == revealed);
          CHECK (mock.isExpanded());
          CHECK (mock.verifyEvent("create","target")
                     .beforeCall("reveal")
                     .beforeCall("expand").arg(true)
                     .beforeEvent("expanded"));
          
          // invoking the slotExpand() also emitted a state mark to persist that expansion state...
          CHECK (nexusLog.verifyCall("note").arg(targetID, GenNode{"expand", true})
                         .before("handling state-mark"));
          
          
          // second test: the same can be achieved via UI-Bus message...
          revealed = false;
          auto stateMark = GenNode{"reveal", 47};  // (payload argument irrelevant)
          auto& uiBus = stage::test::Nexus::testUI();
          CHECK (nexusLog.ensureNot("reveal"));
          
          uiBus.mark (targetID, stateMark);                // send the state mark message to reveal the element
          
          CHECK (true == revealed);
          CHECK (mock.verifyMark("reveal", 47)
                     .afterEvent("expanded")
                     .beforeCall("reveal")
                     .beforeCall("expand").arg(true));
          
          CHECK (nexusLog.verifyCall("mark").arg(targetID, stateMark)
                         .after("handling state-mark")
                         .before("reveal")
                         .beforeEvent("delivered mark"));
          
          // Note the fine point: the target element /was/ already expanded
          // and thus there is no second "expanded" event, nor is there a
          // second state mark emitted into the UI-Bus...
          CHECK (mock.ensureNot("expanded")
                     .afterCall("reveal")
                     .afterEvent("expanded"));
          CHECK (nexusLog.ensureNot("note")
                         .afterCall("mark").arg(targetID, stateMark)
                         .after("handling state-mark"));
          
          
          
          cout << "____Event-Log_________________\n"
               << util::join(mock.getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          
          cout << "____Nexus-Log_________________\n"
               << util::join(nexusLog, "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
        }
      
      
      /** @test receive various kinds of notifications.
       * Send message, error and flash messages via Bus to the element
       * and verify the doMsg, doErr or doFlash handlers were invoked.
       */
      void
      notify ()
        {
          MARK_TEST_FUN
          EventLog nexusLog = stage::test::Nexus::startNewLog();
          
          MockElm mock("target");
          ID targetID = mock.getID();
          auto& uiBus = stage::test::Nexus::testUI();
          
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
      
      
      
      /** @test mutate the mock element through diff messages
       * This test performs the basic mechanism used to populate the UI
       * or to change structure or settings within individual elements.
       * This is done by sending a MutationMessage via UI-Bus, which is
       * handled and applied to the receiver by Lumiera's diff framework.
       * 
       * This test uses the MockElem to simulate real UI elements;
       * to be able to verify the diff application, MockElm is already preconfigured
       * with a _diff binding_, and it exposes a set of attributes and a collection
       * of child mock elements. Basically, the diff mechanism allows to effect
       * structural changes within an otherwise opaque implementation data structure.
       * For this to work, the receiver needs to create a custom _diff binding_.
       * Thus, each subclass of Tangible has to implement the virtual function
       * Tangible::buildMutator() and hook up those internal structures, which
       * are exposed to changes via diff message. This is what we then call a
       * "diff binding" (and MockElement is already outfitted this way). Note
       * especially how child UI elements can be added recursively, allowing
       * gradually to populate the contents of the UI.
       * 
       * The diff itself is an iterable sequence of _diff verbs_.
       * Typically, such a diff is generated as the result of some operation
       * in the Session core, or it is created by comparing two versions of
       * an abstracted object description (e.g. session snapshot).
       * 
       * Here in this test case, we use a hard wired diff sequence,
       * so we can check the expected structural changes actually took place.
       * 
       * @see MutationMessage_test
       */
      void
      mutate ()
        {
          MARK_TEST_FUN
          EventLog nexusLog = stage::test::Nexus::startNewLog();
          
          MockElm rootMock("root");
          ID rootID = rootMock.getID();
          
          rootMock.attrib["α"] = "Centauri";
          CHECK ("Centauri" == rootMock.attrib["α"]);
          CHECK (isnil (rootMock.scope));
          
          
            // simulated source for structural diff
            struct : lib::diff::TreeDiffLanguage
              {
                const GenNode
                  ATTRIB_AL = GenNode("α", "quadrant"),
                  ATTRIB_PI = GenNode("π", 3.14159265),
                  CHILD_1   = MakeRec().genNode("a"),
                  CHILD_2   = MakeRec().genNode("b");
                
                auto
                generateDiff()
                  {
                    using lib::diff::Ref;
                    
                    return MutationMessage{ after(Ref::ATTRIBS)   // start after the existing attributes (of root)
                                          , ins(CHILD_1)          // insert first child (with name "a")
                                          , ins(CHILD_2)          // insert second child (with name "b")
                                          , set(ATTRIB_AL)        // assign a new value to attribute "α" <- "quadrant"
                                          , mut(CHILD_2)          // open nested scope of child "b" for recursive mutation
                                            , ins(ATTRIB_PI)      // ..within nested scope, add a new attribute "π" := 3.14159265
                                          , emu(CHILD_2)          // leave nested scope
                                          };
                  }
              }
              diffSrc;
          
          
          auto& uiBus = stage::test::Nexus::testUI();
          
          
          // send a Diff message via UI-Bus to the rootMock
          uiBus.change(rootID, diffSrc.generateDiff());
          
          
          // Verify the rootMock has been properly altered....
          MockElm& childA = *rootMock.scope[0];
          MockElm& childB = *rootMock.scope[1];
          
          CHECK (2 == rootMock.scope.size());             // we've got two children now
          CHECK (rootMock.attrib["α"] == "quadrant");     // alpha attribute has been reassigned
          CHECK (childA.getID() == diffSrc.CHILD_1.idi);  // children have the expected IDs
          CHECK (childB.getID() == diffSrc.CHILD_2.idi);
          CHECK (childB.attrib["π"]  == "3.1415927");     // and the second child got attribute Pi
          
          
          CHECK (rootMock.verifyEvent("create","root")
                         .beforeCall("buildMutator").on(&rootMock)
                         .beforeEvent("diff","root accepts mutation...")        // start of diff mutation
                         .beforeEvent("diff","create child \"a\"")              // insert first child
                         .beforeEvent("create", "a")
                         .beforeEvent("diff","create child \"b\"")              // insert second child
                         .beforeEvent("create", "b")
                         .beforeEvent("diff","set Attrib α <-quadrant")         // assign value to existing attribute α
                         .beforeCall("buildMutator").on(&childB)                // establish nested mutator for second child
                         .beforeEvent("diff","b accepts mutation...")
                         .beforeEvent("diff",">>Scope>> b")                     // recursively mutate second child
                         .beforeEvent("diff","++Attrib++ π = 3.1415927"));      // insert new attribute π within nested scope
          
          
          CHECK (nexusLog.verifyCall("routeAdd").arg(rootMock.getID(), memLocation(rootMock))      // rootMock was attached to Nexus
                         .beforeCall("change")  .argMatch(rootMock.getID(),
                                                          "after.+ins.+ins.+set.+mut.+ins.+emu")   // diff message sent via UI-Bus
                         .beforeCall("routeAdd").arg(childA.getID(), memLocation(childA))          // first new child was attached to Nexus
                         .beforeCall("routeAdd").arg(childB.getID(), memLocation(childB))          // second new child was attached to Nexus
                         .beforeEvent("applied diff to "+string(rootMock.getID()))
                         );
          
          cout << "____Event-Log_________________\n"
               << util::join(rootMock.getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          
          cout << "____Nexus-Log_________________\n"
               << util::join(nexusLog, "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
        }
      
      
      
      static string
      memLocation (Tangible& uiElm)
        {
          return lib::idi::instanceTypeID (&uiElm);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (AbstractTangible_test, "unit stage");
  
  
}}}// namespace stage::model::test
