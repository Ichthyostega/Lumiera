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
#include "test/test-nexus.hpp"
#include "test/mock-elm.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/diff/gen-node.hpp"
//#include "gui/model/diagnostics.hpp"
//#include "lib/util.hpp"


//#include <boost/lexical_cast.hpp>
#include <iostream>
//#include <string>
//#include <map>

using lib::idi::EntryID;
using lib::idi::BareEntryID;
using gui::test::MockElm;
using lib::diff::GenNode;
//using boost::lexical_cast;
//using util::contains;
//using std::string;
using std::cout;
using std::endl;


namespace gui  {
namespace model{
namespace test {
  
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
          CHECK (nexusLog.verifyCall("note").on("TestNexus").arg(elmID, "GenNode-ID(\"expand\")-DataCap|«bool»|0"));
          
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
          CHECK (nexusLog.verify("remove route to bID-zeitgeist")
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
      
      
      void
      commandInvocation ()
        {
          UNIMPLEMENTED ("pass a message to invoke an action");
        }
      
      
      void
      captureStateMark ()
        {
          UNIMPLEMENTED ("message to capture interface state");
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
