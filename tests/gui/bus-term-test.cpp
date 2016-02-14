/*
  BusTerm(Test)  -  cover the building block of the UI-Bus

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


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "gui/ctrl/bus-term.hpp"
#include "gui/interact/presentation-state-manager.hpp"
#include "test/test-nexus.hpp"
#include "test/mock-elm.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/format-cout.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/luid.h"
#include "lib/util.hpp"



using lib::idi::EntryID;
using lib::idi::BareEntryID;
using gui::interact::PresentationStateManager;
using gui::ctrl::BusTerm;
using gui::test::MockElm;
using lib::diff::GenNode;
using lib::diff::Rec;
using lib::diff::Ref;
using lib::time::Time;
using lib::time::TimeSpan;
using lib::hash::LuidH;
using lib::HashVal;
using util::isnil;


namespace gui  {
namespace model{
namespace test {
  
  using proc::control::LUMIERA_ERROR_UNBOUND_ARGUMENTS;
  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
  
  namespace { // test fixture...
    
  }//(End) test fixture
  
  
  
  
  
  
  /**************************************************************************//**
   * @test cover the standard node element (terminal element) within the UI-Bus,
   * with the help of an attached mock UI element. Contrary to the related
   * [ui-element test](AbstractTangible_test), here we focus on the bus side
   * of the standard interactions.
   * 
   * This test enacts the fundamental generic communication patterns
   * to verify the messaging behaviour
   * - attaching a \ref BusTerm
   * - generating a command invocation
   * - argument passing
   * - capture a _state mark_
   * - replay a _state mark_
   * - cast messages and error states downstream
   * - generic operating of interface states
   * - detaching on element destruction
   * 
   * @see AbstractTangible_test
   * @see gui::model::Tangible
   * @see gui::ctrl::BusTerm
   */
  class BusTerm_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          attachNewBusTerm();
          commandInvocation();
          captureStateMark();
          replayStateMark();
          verifyNotifications();
          clearStates();
          pushDiff();
          destroy();
        }
      
      
      /** @test build a new BusTerm and verify connectivity.
       * Every [tangible UI-element](\ref Tangible) bears an embedded BusTerm
       * member. Since the latter _requires another, up-link BusTerm_ on construction,
       * connection to the [UI-Bus](ui-bus.hpp) is structurally ensured. Moreover,
       * when hooking up a new UI-element, the initialisation of the embedded BusTerm
       * will cause a down-link connection to be installed into the central routing
       * table within the \ref Nexus, the hub of the UI-Bus. Routing and addressing
       * is based on the UI-element's unique EntryID, destruction of the element,
       * through invocation of BusTerm's dtor, will ensure deregistration
       * from the Hub.
       */
      void
      attachNewBusTerm()
        {
          MARK_TEST_FUN;
          // our dummy will be linked with this identity
          BareEntryID elmID = EntryID<MockElm>{"zeitgeist"};
          
          // Access the log on the Test-Nexus hub
          EventLog nexusLog = gui::test::Nexus::startNewLog();
          CHECK (nexusLog.ensureNot("zeitgeist"));
          
          MockElm mock(elmID);
          CHECK (nexusLog.verifyCall("routeAdd").on("TestNexus").arg(elmID,"Tangible")       // Note: invoked from ctor, so it is just a tangible at the moment
                         .beforeEvent("TestNexus", "added route to bID-zeitgeist"));
          
          EventLog elmLog = mock.getLog();
          CHECK (elmLog.verifyCall("ctor").on(&mock)
                       .beforeEvent("create", "zeitgeist"));
          
          
          // now verify there is indeed bidirectional connectivity...
          CHECK (elmLog.ensureNot("expanded"));
          CHECK (elmLog.ensureNot("doFlash"));
          CHECK (nexusLog.ensureNot("zeitgeist").arg("expand"));
          CHECK (nexusLog.ensureNot("zeitgeist").arg("Flash"));
          
          // invoke action on element to cause upstream message (with a "state mark")
          mock.slotExpand();
          CHECK (elmLog.verifyEvent("expanded"));
          CHECK (nexusLog.verifyCall("note").on("TestNexus").arg(elmID, "GenNode-ID(\"expand\")-DataCap|«bool»|true"));
          
          // send a state mark down to the mock element
          gui::test::Nexus::testUI().mark (elmID, GenNode("Flash", 23));
          CHECK (nexusLog.verifyCall("mark").on("TestNexus").arg(elmID, "Flash")
                         .beforeEvent("TestNexus", "mark to bID-zeitgeist"));
          CHECK (elmLog.verifyCall("doFlash").on("zeitgeist"));
          
          
          // kill the zeitgeist and verify disconnection
          mock.kill();
          CHECK (elmLog.verifyEvent("destroy","zeitgeist"));
          CHECK (nexusLog.verifyCall("routeDetach").on("TestNexus").arg(elmID)
                         .beforeEvent("TestNexus", "removed route to bID-zeitgeist"));
          
          gui::test::Nexus::testUI().mark (elmID, GenNode("Flash", 88));
          CHECK (nexusLog.verify("removed route to bID-zeitgeist")
                         .beforeCall("mark").on("TestNexus").arg(elmID, "Flash")
                         .beforeEvent("warn","discarding mark to unknown bID-zeitgeist"));
          CHECK (elmLog.ensureNot("Flash")
                       .afterEvent("destroy","zeitgeist"));
          
          
          cout << "____Probe-Log_________________\n"
               << util::join(elmLog, "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          
          cout << "____Nexus-Log_________________\n"
               << util::join(gui::test::Nexus::getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
        }
      
      
      /** @test perform the full command binding and invocation protocol */
      void
      commandInvocation()
        {
          MARK_TEST_FUN
          gui::test::Nexus::startNewLog();
          auto cmd = gui::test::Nexus::prepareMockCmd<string, TimeSpan, LuidH>();
          
          MockElm mock("uiElm");
          
          // random command arguments...
          string text {lib::test::randStr(12)};
          TimeSpan clip (Time(1,2,3), lib::test::randTime());
          LuidH luid;
          
          // we cannot invoke commands prior to binding arguments
          VERIFY_ERROR (UNBOUND_ARGUMENTS, mock.issueCommand(cmd) );
          
          // proper argument typing is ensured while dispatching the bind message. 
          VERIFY_ERROR (WRONG_TYPE, mock.prepareCommand(cmd, Rec({"lalala"})) );
          
          // command can't be issued, since it's still unbound
          CHECK (not cmd.canExec());
          
          
          mock.prepareCommand (cmd, text, clip, luid);
          
          CHECK (cmd.canExec());
          CHECK (gui::test::Nexus::wasBound(cmd, text, clip, luid));
          CHECK (not gui::test::Nexus::wasInvoked(cmd));
          CHECK (not gui::test::Nexus::wasInvoked(cmd, text, clip, luid));
          CHECK (not gui::test::Nexus::wasBound(cmd, "lololo"));
          
          
          mock.issueCommand(cmd);
          
          CHECK (gui::test::Nexus::wasInvoked(cmd));
          CHECK (gui::test::Nexus::wasInvoked(cmd, text, clip, luid));
          CHECK (not gui::test::Nexus::wasInvoked(cmd, " huh ", clip, luid));
          CHECK (not gui::test::Nexus::wasInvoked(cmd, text, clip));
          
          // Mock commands are automatically unique
          auto cmdX = gui::test::Nexus::prepareMockCmd<>();
          auto cmdY = gui::test::Nexus::prepareMockCmd<>();
          CHECK (cmd.getID() != cmdX.getID());
          CHECK (cmd.getID() != cmdY.getID());
          
          CHECK (not gui::test::Nexus::wasInvoked(cmdX));
          CHECK (not gui::test::Nexus::wasInvoked(cmdY));
          
          cout << "____Nexus-Log_________________\n"
               << util::join(gui::test::Nexus::getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          
          gui::test::Nexus::setCommandHandler(); // deinstall custom command handler
        }
      
      
      /** @test collect state mark notifications from bus
       * We use a test version of the PresentationStateManager,
       * based on the same building blocks as _the real thing_ */
      void
      captureStateMark()
        {
          MARK_TEST_FUN
          gui::test::Nexus::startNewLog();
          PresentationStateManager& stateManager = gui::test::Nexus::useMockStateManager();
          
          MockElm mockA("alpha");   BareEntryID alpha = mockA.getID();
          MockElm mockB("bravo");   BareEntryID bravo = mockB.getID();
          MockElm mockC("charly");  BareEntryID charly = mockC.getID();
          
          mockA.slotExpand();
          
          mockB.slotExpand();
          mockB.slotCollapse();
          
          CHECK (stateManager.currentState(alpha, "expand") == GenNode("expand", true ));
          CHECK (stateManager.currentState(bravo, "expand") == GenNode("expand", false ));
          
          // handling of missing information
          CHECK (stateManager.currentState(charly, "expand") == Ref::NO); // no data recorded yet
          CHECK (stateManager.currentState(bravo, "extinct") == Ref::NO); // unknown property
          
          EntryID<MockElm> bruno("bruno");
          CHECK (stateManager.currentState(bruno, "expand")  == Ref::NO); // who knows bruno?
          
          mockC.slotExpand();
          CHECK (stateManager.currentState(charly, "expand") == GenNode("expand", true ));
          
          mockC.reset();
          CHECK (stateManager.currentState(charly, "expand") == Ref::NO); // back to void
          
          ////////////////////////////////////////////////////////////////////////////////////////////////////TODO WIP
          cout << "____Nexus-Log_________________\n"
               << util::join(gui::test::Nexus::getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          ////////////////////////////////////////////////////////////////////////////////////////////////////TODO WIP
        }
      
      
      /** @test replay previously captured state information" */
      void
      replayStateMark()
        {
          MARK_TEST_FUN
          PresentationStateManager& stateManager = gui::test::Nexus::getMockStateManager();
          
          MockElm mockA("alpha");
          // no "bravo" this time
          MockElm mockC("charly");
          
          CHECK (not mockA.isExpanded());
          CHECK (not mockC.isTouched());
          
          BareEntryID alpha = mockA.getID();
          stateManager.replayState (alpha, "expand");
          CHECK (mockA.isExpanded());
          
          auto& uiBus = gui::test::Nexus::testUI();
          uiBus.mark (mockA.getID(), GenNode{"expand", false});
          
          CHECK (not mockA.isExpanded());
          CHECK (mockA.isTouched());
          
          stateManager.replayAllState ("expand");
          
          CHECK (mockA.isExpanded());
          CHECK (not mockC.isExpanded());
          CHECK (not mockC.isTouched());
        }
      
      
      /** @test send notifications to a distinct element, or as broadcast. */
      void
      verifyNotifications()
        {
          MARK_TEST_FUN
          EventLog nexusLog = gui::test::Nexus::startNewLog();
          
          MockElm mockA("alpha");   BareEntryID alpha = mockA.getID();   mockA.joinLog (nexusLog);
          MockElm mockB("bravo");   BareEntryID bravo = mockB.getID();   mockB.joinLog (nexusLog);
          MockElm mockC("charly");  BareEntryID charly = mockC.getID();  mockC.joinLog (nexusLog);
          
          auto& uiBus = gui::test::Nexus::testUI();
          
          CHECK (not mockA.isTouched());
          CHECK (not mockB.isTouched());
          CHECK (not mockC.isTouched());
          
          uiBus.mark (alpha, GenNode{"Message", "Centauri"});
          uiBus.mark (bravo, GenNode{"Flash", true});
          uiBus.mark (charly, GenNode{"Message", "Delta"});
          uiBus.mark (charly, GenNode{"Error", "Echo"});
          
          CHECK (    mockA.isTouched());
          CHECK (not mockB.isTouched());
          CHECK (    mockC.isTouched());
          
          CHECK (not mockA.isError());
          CHECK (not mockB.isError());
          CHECK (    mockC.isError());
          
          CHECK ("Centauri" == mockA.getMessage());
          CHECK ("Delta"    == mockC.getMessage());
          
          CHECK ("Echo"     == mockC.getError());
          
          // verify the message passing in the combined log...
          CHECK (nexusLog.verifyEvent("create", "alpha")
                         .beforeCall("mark").on("TestNexus").arg("alpha", "Centauri")  // bus API invoked
                         .beforeCall("doMsg").on("alpha").arg("Centauri")              // handler on target invoked
                         .beforeEvent("mark", "Centauri")                              // target action activated 
                         .beforeEvent("TestNexus","delivered mark to bID-alpha"));     // dispatch done within UI-Bus
          
          CHECK (nexusLog.verifyEvent("TestNexus","delivered mark to bID-alpha")
                         .beforeCall("mark").on("TestNexus").arg("bravo", "GenNode-ID(\"Flash\")-DataCap|«bool»|true")
                         .beforeCall("doFlash").on("bravo")
                         .beforeEvent("TestNexus","delivered mark to bID-bravo"));
          
          // NOTE: calls are passed down synchronously, in one hop, and in sequence
          CHECK (nexusLog.verifyEvent("TestNexus","delivered mark to bID-bravo")
                         .beforeCall("mark").on("TestNexus").arg("charly", "GenNode-ID(\"Message\")-DataCap|«string»|Delta")
                         .beforeCall("doMsg").on("charly").arg("Delta")
                         .beforeEvent("mark", "Delta").id("Message")
                         .beforeEvent("TestNexus","delivered mark to bID-charly")
                         .beforeCall("mark").on("TestNexus").arg("charly", "GenNode-ID(\"Error\")-DataCap|«string»|Echo")
                         .beforeCall("doErr").on("charly").arg("Echo")
                         .beforeEvent("mark", "Echo").id("Error")
                         .beforeEvent("TestNexus","delivered mark to bID-charly"));
          
          
          // broadcast message
          uiBus.markAll (GenNode{"Message", "Foxtrot"});
          CHECK (not mockA.isError());
          CHECK (not mockB.isError());
          CHECK (    mockC.isError());
          CHECK (    mockA.isTouched());
          CHECK (    mockB.isTouched());
          CHECK (    mockC.isTouched());
          CHECK ("Foxtrot" == mockA.getMessage());
          CHECK ("Foxtrot" == mockB.getMessage());
          CHECK ("Foxtrot" == mockC.getMessage());
          CHECK (       "" == mockA.getError());
          CHECK (       "" == mockB.getError());
          CHECK (   "Echo" == mockC.getError());
          
          CHECK (nexusLog.verifyEvent("mark", "Echo").id("Error")
                         .beforeCall("markAll").on("TestNexus").arg("Foxtrot")
                         .beforeEvent("Broadcast", "Foxtrot")
                         .beforeCall("mark").on("TestNexus").arg("bravo", "GenNode-ID(\"Message\")-DataCap|«string»|Foxtrot")
                         .beforeCall("doMsg").on("bravo").arg("Foxtrot")
                         .beforeEvent("TestNexus", "broadcasted mark to 3 terminals"));
          
          // the order of dispatch is unspecified,
          // but we know a regular mark call sequence happens for each connected terminal
          CHECK (nexusLog.verifyCall("markAll").on("TestNexus").arg("Foxtrot")
                         .beforeCall("mark").on("TestNexus").arg("alpha", "Foxtrot")
                         .beforeCall("doMsg").on("alpha").arg("Foxtrot")
                         .beforeEvent("TestNexus", "successfully broadcasted"));
          
          CHECK (nexusLog.verifyCall("markAll").on("TestNexus").arg("Foxtrot")
                         .beforeCall("mark").on("TestNexus").arg("bravo", "Foxtrot")
                         .beforeCall("doMsg").on("bravo").arg("Foxtrot")
                         .beforeEvent("TestNexus", "successfully broadcasted"));
          
          CHECK (nexusLog.verifyCall("markAll").on("TestNexus").arg("Foxtrot")
                         .beforeCall("mark").on("TestNexus").arg("charly", "Foxtrot")
                         .beforeCall("doMsg").on("charly").arg("Foxtrot")
                         .beforeEvent("TestNexus", "successfully broadcasted"));
          
          
          cout << "____Nexus-Log_________________\n"
               << util::join(nexusLog, "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
        }
      
      
      /** @test broadcast various degrees of state reset */
      void
      clearStates()
        {
          MARK_TEST_FUN
          EventLog nexusLog = gui::test::Nexus::startNewLog();
          
          MockElm mockA("alpha");   BareEntryID alpha = mockA.getID();   mockA.joinLog (nexusLog);
          MockElm mockB("bravo");   BareEntryID bravo = mockB.getID();   mockB.joinLog (nexusLog);
          MockElm mockC("charly");  BareEntryID charly = mockC.getID();  mockC.joinLog (nexusLog);
          
          auto& uiBus = gui::test::Nexus::testUI();
          
          CHECK (not mockA.isTouched());
          CHECK (not mockB.isTouched());
          CHECK (not mockC.isTouched());
          
          mockB.slotExpand();
          uiBus.mark (alpha,  GenNode{"Message", "Centauri"});
          uiBus.mark (charly, GenNode{"Message", "Delta"});
          uiBus.mark (charly, GenNode{"Error",   "Echo"});
          
          CHECK (mockB.isExpanded());
          CHECK (mockC.isError());
          CHECK ("Delta" == mockC.getMessage());
          CHECK ("Centauri" == mockA.getMessage());
          
          // reset all notification messages
          uiBus.markAll (GenNode{"resetMsg", true});
          CHECK (mockB.isExpanded());
          CHECK (mockC.isError());
          CHECK (isnil (mockA.getMessage()));
          CHECK (isnil (mockC.getMessage()));
          
          uiBus.mark (bravo, GenNode{"Message", "miss"});
          mockA.slotExpand();
          mockA.slotCollapse();
          
          // reset error state(s)
          uiBus.markAll (GenNode{"resetErr", true});
          CHECK (not mockB.isExpanded());
          CHECK (mockB.isExpanded());
          CHECK ("miss" == mockB.getMessage());
          CHECK (not mockC.isError());
          
          auto& stateManager = gui::test::Nexus::getMockStateManager();
          CHECK (stateManager.currentState(alpha, "expand") == GenNode("expand", false ));
          CHECK (stateManager.currentState(bravo, "expand") == GenNode("expand", true ));
          CHECK (stateManager.currentState(charly, "expand") == Ref::NO);
          
          // send global sweeping reset
          uiBus.markAll (GenNode{"reset", true});
          
          CHECK (not mockA.isTouched());
          CHECK (not mockB.isTouched());
          CHECK (not mockC.isTouched());
          
          CHECK (not mockA.isExpanded());
          CHECK (not mockB.isExpanded());
          
          CHECK (isnil (mockA.getMessage()));
          CHECK (isnil (mockB.getMessage()));
          CHECK (isnil (mockC.getMessage()));
          
          CHECK (stateManager.currentState(alpha,  "expand") == Ref::NO);
          CHECK (stateManager.currentState(bravo,  "expand") == Ref::NO);
          CHECK (stateManager.currentState(charly, "expand") == Ref::NO);
          
          
          ////////////////////////////////////////////////////////////////////////////////////////////////////TODO WIP
          cout << "____Nexus-Log_________________\n"
               << util::join(nexusLog, "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          ////////////////////////////////////////////////////////////////////////////////////////////////////TODO WIP
          gui::test::Nexus::setStateMarkHandler(); // deinstall custom command handler
        }
      
      
      void
      pushDiff()
        {
          UNIMPLEMENTED ("push a mutation diff towards an interface element");
        }
      
      
      void
      destroy()
        {
          UNIMPLEMENTED ("detach and destroy the test BusTerm");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BusTerm_test, "unit gui");
  
  
}}} // namespace gui::model::test
