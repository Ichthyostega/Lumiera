/*
  SessionStructureMapping(Test)  -  map session structure to GUI widgets

  Copyright (C)         Lumiera.org
    2014,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file session-structure-mapping-test.cpp
 ** This test is a concept study how to organise the proxy model
 ** in the Lumiera GUI. This mediating model shields access to the
 ** actual "high-level model" in Steam-Layer, it translates signals
 ** into command invocations and helps to push structure changes
 ** up to the timeline display.
 ** 
 ** @note as of 10/2014 this is a initial draft into the blue...
 ** @todo WIP  ///////////////////////TICKET #955
 ** @todo WIP  ///////////////////////TICKET #961
 ** 
 ** @todo as of 9/2017 there is now a whole subsystem based on those ideas,
 **       yet still we can not demonstrate a complete integration as was
 **       intended by this test. For now this test is an empty placeholder and
 **       reminder to create a concise and readable high-level demonstration
 **       eventually. And the idea to provide a model::Diagnostics counterpart
 **       for test invocation seems worth exploring further...
 ** 
 ** @note meanwhile there is a working diff framework and implementation in place...
 ** @see DiffTreeApplication_test
 ** @see DiffComplexApplication_test
 ** 
 */


#include "lib/test/run.hpp"
#include "stage/model/session-facade.hpp"
#include "stage/model/diagnostics.hpp"
//#include "lib/util.hpp"


//#include <string>

//using util::contains;
//using std::string;


namespace stage  {
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
   * @see stage::model::SessionFacade
   */
  class SessionStructureMapping_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          stage::model::Diagnostics lock(TEST_SESSION_1);
          retrieveSessionStructure();
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
  LAUNCHER (SessionStructureMapping_test, "unit gui");
  
  
}}} // namespace stage::model::test
