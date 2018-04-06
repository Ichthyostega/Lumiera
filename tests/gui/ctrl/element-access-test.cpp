/*
  ElementAccess(Test)  -  verify mechanics of low-level UI element access

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

/** @file view-spec-dsl-test.cpp
 ** unit test \ref ViewSpecDSL_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "gui/interact/view-spec-dsl.hpp"
#include "gui/interact/ui-coord.hpp"
#include "gui/interact/gen-node-location-query.hpp"
#include "lib/depend-inject.hpp"
#include "lib/format-cout.hpp"
//#include "lib/idi/entry-id.hpp"
//#include "lib/diff/gen-node.hpp"
//#include "lib/util.hpp"

//#include <string>
//#include <vector>


using std::string;
using lib::diff::MakeRec;
using lib::diff::Rec;
//using lib::idi::EntryID;
//using lib::diff::GenNode;
//using util::isSameObject;
//using util::isnil;


namespace gui  {
namespace interact {
namespace test {
  
//  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
  using lib::test::showSizeof;
  
  using MockLoationSolver = lib::DependInject<UILocationSolver>::Local<>;
  
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
  class ElementAccess_test : public Test
    {
      
      virtual void
      run (Arg)
        {
//        verify_basicProperties();
          verify_standardUsage();
          verify_alternatives();
          
          verify_genericInvocation();
        }
      
      
      void
      verify_basicProperties()
        {
          UNIMPLEMENTED ("basic properties of the view spec DSL");
        }
      
      
      void
      verify_standardUsage()
        {
        }
      
      
      void
      verify_alternatives()
        {
          UNIMPLEMENTED ("querying and selection of location alternatives");
        }
      
      
      void
      verify_genericInvocation()
        {
          /////////////////////////////////////////////////////////////////////////////////////////TICKET 1134 : how to create ViewLocator mock without global context??
           //-------------------------------------------------------------Test-Fixture
          //--------------------------------------------------------------(End)Test-Fixture
          
//        ErrorLogView errorLog = viwLocator.get<ErrorLogView>();
//        TimelineView timeline = viwLocator.get<TimelineView>();
          
          /////////////////////////////////////////////////////////////////////////////////////////TICKET 1134 : use an EventLog to verify the forwarded invocations??
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ElementAccess, "unit gui");
  
  
}}} // namespace gui::interact::test
