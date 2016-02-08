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
#include "test/test-nexus.hpp"
#include "test/mock-elm.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/format-cout.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/luid.h"
//#include "lib/util.hpp"



using lib::idi::EntryID;
using lib::idi::BareEntryID;
using gui::test::MockElm;
using lib::diff::GenNode;
using lib::diff::Rec;
using lib::time::Time;
using lib::time::TimeSpan;
using lib::hash::LuidH;
using lib::HashVal;
//using util::contains;


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
      attachNewBusTerm ()
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
          CHECK (elmLog.ensureNot("collapsed"));
          CHECK (elmLog.ensureNot("doFlash"));
          CHECK (nexusLog.ensureNot("zeitgeist").arg("collapse"));
          CHECK (nexusLog.ensureNot("zeitgeist").arg("Flash"));
          
          // invoke action on element to cause upstream message (with a "state mark")
          mock.slotCollapse();
          CHECK (elmLog.verifyEvent("collapsed"));
          CHECK (nexusLog.verifyCall("note").on("TestNexus").arg(elmID, "GenNode-ID(\"expand\")-DataCap|«bool»|false"));
          
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
      commandInvocation ()
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
          
          
          mock.prepareCommand(cmd, Rec({text, clip, luid}));
          
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
        }
      
      
      void
      captureStateMark ()
        {
          UNIMPLEMENTED ("message to capture interface state");
          ////////////////////////////////////////////////////////////////////////////////////////////////////TODO WIP
          cout << "____Nexus-Log_________________\n"
               << util::join(gui::test::Nexus::getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          ////////////////////////////////////////////////////////////////////////////////////////////////////TODO WIP
        }
      
      
      void
      replayStateMark ()
        {
          UNIMPLEMENTED ("replay previously captured state information");
        }
      
      
      void
      verifyNotifications ()
        {
          UNIMPLEMENTED ("send notifications to a distinct element");
        }
      
      
      void
      clearStates ()
        {
          UNIMPLEMENTED ("broadcast state reset");
        }
      
      
      void
      pushDiff ()
        {
          UNIMPLEMENTED ("push a mutation diff towards an interface element");
        }
      
      
      void
      destroy ()
        {
          UNIMPLEMENTED ("detach and destroy the test BusTerm");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BusTerm_test, "unit gui");
  
  
}}} // namespace gui::model::test
