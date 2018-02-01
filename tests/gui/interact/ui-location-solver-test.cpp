/*
  UILocationSolver(Test)  -  verify mechanics of a DSL to configure view allocation

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file ui-location-resolver-test.cpp
 ** unit test \ref UILocationSolver_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "gui/interact/view-spec-dsl.hpp"
#include "gui/interact/ui-coord.hpp"
#include "lib/format-cout.hpp"
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
  using lib::test::showSizeof;
  
  namespace { //Test fixture...
    
  }//(End)Test fixture
  
  
  /******************************************************************************//**
   * @test cover a mechanism to resolve the desired location of an ui element.
   *       The UILocationSolver is operated by the ViewLocator service, which itself
   *       is part of the InteractionDirector. In typical usage, the location rules
   *       are drawn from the [ViewSpec-DSL](\ref view-spec-dsl.hpp), evaluated
   *       with the help of a [Coordinate Resolver](\ref UICoordResolver), based on
   *       the real UI topology existing at that moment, accessible in abstracted
   *       form through the LocationQuery interface. This test setup mimics this
   *       invocation scheme, but replaces the real UI by an abstract tree notation
   *       embedded directly into the individual test cases.
   * 
   * @see ui-location-solver.hpp
   * @see view-spec-dsl.hpp
   * @see UICoordResolver_test
   */
  class UILocationSolver_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simple_usage_example();
          verify_cornerCases();
          verify_standardSituations();
        }
      
      
      void
      simple_usage_example()
        {
          UNIMPLEMENTED ("demonstrate the typical invocation and usage");
        }
      
      
      void
      verify_cornerCases()
        {
          UNIMPLEMENTED ("cover some corner cases");
        }
      
      
      void
      verify_standardSituations()
        {
          UNIMPLEMENTED ("emulate the relevant standard situations of view location resolution");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (UILocationSolver_test, "unit gui");
  
  
}}} // namespace gui::interact::test
