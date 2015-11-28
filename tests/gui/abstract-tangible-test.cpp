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
 *  Verify the common base shared by all interface elements of relevance.
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
#include "lib/error.hpp"
//#include "gui/model/session-facade.hpp"
//#include "gui/model/diagnostics.hpp"
//#include "lib/util.hpp"


//#include <boost/lexical_cast.hpp>
//#include <iostream>
//#include <string>
//#include <map>

//using boost::lexical_cast;
//using util::contains;
//using std::string;
//using std::cout;
//using std::endl;

using lib::error::LUMIERA_ERROR_ASSERTION;


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
      
      
      /** @test how to retrieve and enumerate session contents
       *        as operation initiated from GUI display code
       */
      void
      verify_mockManipulation ()
        {
          MockElm mock("dummy");
          mock.verify("ctor");
          mock.verifyEvent("ctor");
          mock.verify("ctor").arg("dummy");
          
          CHECK ("dummy" == mock.getID().getSym());
          CHECK ("ID<gui::model::test::MockElm>-dummy" = string(mock.getID()));
          
          VERIFY_ERROR (ASSERTION, mock.verifyCall("reset"));
          
          mock.reset();
          mock.verify("reset");
          mock.verifyCall("reset");
          mock.verifyEvent("reset");
          mock.verify("reset").after("ctor");
          mock.verify("ctor").before("reset");
          VERIFY_ERROR (ASSERTION, mock.verify("reset").before("ctor"));
          VERIFY_ERROR (ASSERTION, mock.verify("ctor").after("reset"));
          
          mock.verify("reset").before("reset");
          mock.verify("reset").beforeEvent("reset");
          mock.verifyCall("reset").before("reset");
          mock.verifyCall("reset").beforeEvent("reset");
          VERIFY_ERROR (ASSERTION, mock.verifyCall("reset").afterCall("reset"));
          VERIFY_ERROR (ASSERTION, mock.verifyCall("reset").afterEvent("reset"));
          VERIFY_ERROR (ASSERTION, mock.verifyEvent("reset").afterEvent("reset"));
          
          CHECK (!mock.isTouched());
          CHECK (!mock.isExpanded());
          
          mock.noteMsg("dolorem ipsum quia dolor sit amet consectetur adipisci velit.");
          mock.verifyNote("Msg");
          mock.verifyCall("noteMsg");
          mock.verifyCall("noteMsg").arg("lorem ipsum");
          mock.verifyCall("noteMsg").argMatch("dolor.+dolor\\s+");
          mock.verifyMatch("Rec\\(note.+kind = Msg.+msg = dolorem ipsum");
          
          EventLog log = mock.getLog();
          log.verify("ctor")
             .before("reset")
             .before("lorem ipsum");
          
          MockElm foo("foo"), bar;
          foo.verify("ctor").arg("foo");
          bar.verify("ctor").arg();
          
          bar.ensureNot("foo");
          log.ensureNot("foo");
          mock.ensureNot("foo");
          VERIFY_ERROR (ASSERTION, foo.ensureNot("foo"));
          
          log.join(bar).join(foo);
          log.verifyEvent("logJoin").arg(bar.getID())
             .beforeEvent("logJoin").arg("foo");
          
          mock.verifyEvent("logJoin").arg(bar.getID())
              .beforeEvent("logJoin").arg("foo");
          mock.verify("ctor").arg("foo");
          log.verify("ctor").arg("foo");
          log.verify("ctor").arg("dummy")
             .before("ctor").arg(bar.getID())
             .before("ctor").arg("foo");
          
          mock.kill();
          foo.noteMsg("dummy killed");
          log.verifyEvent("dtor").on("dummy")
             .beforeCall("noteMsg").on("foo");
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
