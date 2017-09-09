/*
  UICoord(Test)  -  properties of topological UI coordinate specifications

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file ui-coord-test.cpp
 ** unit test \ref UICoord_test
 */


#include "lib/test/run.hpp"
//#include "lib/test/test-helper.hpp"
#include "gui/interact/ui-coord.hpp"
//#include "lib/idi/entry-id.hpp"
//#include "lib/diff/gen-node.hpp"
//#include "lib/util.hpp"

//#include <string>


//using std::string;
//using lib::idi::EntryID;
//using lib::diff::GenNode;
//using util::isSameObject;
//using util::isnil;


namespace gui  {
namespace interact {
namespace test {
  
//  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
  
  namespace { //Test fixture...
    
  }//(End)Test fixture
  
  
  /******************************************************************************//**
   * @test verify the mechanics of a functor based internal DSL
   *       to configure access and allocation patters for component views.
   * 
   * @see id-scheme.hpp
   * @see ViewLocator
   * @see UICoord_test
   */
  class UICoord_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verify_basicProperties();
          verify_queryFunctions();
        }
      
      
      void
      verify_basicProperties()
        {
          UNIMPLEMENTED ("define basic properties of UI coordinates");
        }
      
      
      void
      verify_queryFunctions()
        {
          UNIMPLEMENTED ("query components and parts of the path");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (UICoord_test, "unit gui");
  
  
}}} // namespace gui::interact::test
