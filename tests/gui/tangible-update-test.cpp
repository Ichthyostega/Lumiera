/*
  TangibleUpdate(Test)  -  how to update nested tangible UI elements

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


/** @file tangible-update-test.cpp
 ** This test is a concept study regarding a generic structure of UI elements.
 ** Such gui::model::Tangible elements share the ability to be updated through
 ** a structural diff message. Which implies this test to be also an integration
 ** test of Lumiera's tree diff handling framework
 ** 
 ** @note as of 1/2015 this is a draft into the blue...
 ** @todo WIP  ///////////////////////TICKET #959
 ** @todo WIP  ///////////////////////TICKET #961
 ** 
 ** @see gui::UiBus
 ** 
 */


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
  
  
  
  
  
  
  /***********************************************************************//**
   * @test demonstrate the fundamental patterns to access the current
   *       session structure and contents from the GUI, and to receive
   *       a notification with the updated structure and contents.
   *       - since we focus on the mapping and enumeration mechanism,
   *         the explored content is faked diagnostic data
   *       - in reality, these operations are intended to be run from
   *         within the GUI event thread and immediately interwoven
   *         with GTK / Cairo display code
   *       - it is the responsibility of the "Gui Model" (#SessionFacade)
   *         to ensure a read barrier, so the retrieved data can not be
   *         corrupted by concurrent session mutation operations.
   *       
   * @see SessionElementQuery_test
   * @see gui::model::SessionFacade
   */
  class TangibleUpdate_test : public Test
    {
      
      virtual void
      run (Arg)
        {
        }
      
      
      /** @test how to retrieve and enumerate session contents
       *        as operation initiated from GUI display code
       */
      void
      retrieveSessionStructure ()
        {
          
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (TangibleUpdate_test, "unit gui");
  
  
}}} // namespace gui::model::test
