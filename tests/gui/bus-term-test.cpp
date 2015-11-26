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


namespace gui  {
namespace model{
namespace test {
  
  namespace { // test fixture...
    
  }//(End) test fixture
  
  
  
  
  
  
  /**************************************************************************//**
   * @test cover the standard node element (terminal element) within the UI-Bus,
   *       with the help of an attached mock UI element.
   *       
   *       This test enacts the fundamental generic communication patterns
   *       to verify the messaging behaviour
   *       - attaching a [BusTerm]
   *       - generating a command invocation
   *       - argument passing
   *       - capture a _state mark_
   *       - replay a _state mark_
   *       - cast messages and error states downstream
   *       - generic operating of interface states
   *       - detaching on element destruction
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
      
      
      void
      attachNewBusTerm ()
        {
          UNIMPLEMENTED ("build a new BusTerm and verify connectivity");
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
