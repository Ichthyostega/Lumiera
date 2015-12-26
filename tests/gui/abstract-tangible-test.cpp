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
 ** [primary elements][gui::model::Tangible] of the Lumiera GTK UI. Any such element
 ** is connected to the [UI-Bus][gui::UiBus] and responds to some generic actions and
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
#include "lib/idi/entry-id.hpp"
#include "lib/error.hpp"
//#include "gui/model/session-facade.hpp"
//#include "gui/model/diagnostics.hpp"
//#include "lib/util.hpp"


//#include <boost/lexical_cast.hpp>
#include <iostream>
//#include <string>
//#include <map>

using gui::test::MockElm;
using lib::test::EventLog;
using lib::idi::EntryID;
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
   * @see SessionElementQuery_test
   * @see gui::model::SessionFacade
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
       * a [MockElm] with a suitable name / ID, we're able to operate this element
       * programmatically and to send messages and responses from the core "up"
       * to this mocked interface. And since this mock element embodies an
       * [event log][EventLog], the unit test code can verify the occurrence
       * of expected events, invocations and responses.
       */
      void
      verify_mockManipulation ()
        {
          MockElm mock("dummy");
          
          CHECK (mock.verify("ctor"));
          CHECK (mock.verifyEvent("create","dummy"));
          CHECK (mock.verify("ctor").arg("dummy","TestNexus").on(&mock));
          
          CHECK ("dummy" == mock.getID().getSym());
          CHECK (EntryID<MockElm>("dummy") == mock.getID());
          
          CHECK (!mock.verifyCall("reset"));
          
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
          
          MockElm foo("foo"), bar("bar");
          foo.verify("ctor").arg("foo");
          bar.verify("ctor").arg("bar");
          
          bar.ensureNot("foo");
          log.ensureNot("foo");
          mock.ensureNot("foo");
          CHECK (!foo.ensureNot("foo"));
          
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
          cout << "____Event-Log________________\n"
               << util::join(mock.getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          
          mock.kill();
          foo.markMsg("dummy killed");
          CHECK (log.verifyEvent("destroy","dummy")
                    .beforeCall("doMsg").on("foo"));
        }
      
      
      void
      invokeCommand ()
        {
          UNIMPLEMENTED ("invoke an action, pass arguments");
        }
      
      
      void
      markState ()
        {
          UNIMPLEMENTED ("mark interface state");
          
          TODO ("be sure also to cover signal diagnostics here");
        }
      
      
      void
      notify ()
        {
          UNIMPLEMENTED ("receive various kinds of notifications");
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
