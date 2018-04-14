/*
  ViewSpecDSL(Test)  -  verify mechanics of a DSL to configure view allocation

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

/** @file view-spec-dsl-test.cpp
 ** unit test \ref ViewSpecDSL_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "gui/interact/ui-coord.hpp"
#include "gui/interact/view-locator.hpp"
#include "gui/interact/view-spec-dsl.hpp"
#include "gen-node-location-query.hpp"
#include "test/test-element-access.hpp"
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
namespace idi { //------Mock ViewSpec definitions for component test
  
  struct MockView1
    : gui::test::DummyWidget
    {
      using DummyWidget::DummyWidget;
    };
  
  struct MockView2
    : gui::test::DummyWidget
    {
      using DummyWidget::DummyWidget;
    };
  
  /* ==== Dummy ViewSpec rules for those two mock view types (--> see id-scheme.hpp) ==== */
    
  template<>
  struct Descriptor<MockView1>
    {
      ViewSpec locate = UICoord::currentWindow().panel("parentLocation");
      Allocator alloc = limitAllocation(2);
    };
  
}//----------------(End)Mock ViewSpec definitions

namespace interact {
namespace test {
  
//  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
  using lib::test::showSizeof;
  using gui::model::ElementAccess;
  using gui::test::TestElementAccess;
  using gui::test::DummyWidget;
  using gui::test::DummyView;
  using gui::test::DummyTab;
  
  using MockLoationSolver = lib::DependInject<UILocationSolver>::Local<>;
  using MockElementAccess = lib::DependInject<ElementAccess>::Local<TestElementAccess>;
  
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
  class ViewSpecDSL_test : public Test
    {
      
      virtual void
      run (Arg)
        {
//        verify_basicProperties();
          verify_standardUsage();
//        verify_alternatives();
          
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
           //-------------------------------------------------------------Test-Fixture
          // a Test dummy placeholder for the real UI structure
          Rec dummyUiStructure = MakeRec()
                                   .set("win-1"
                                       , MakeRec()
                                           .type("perspective")
                                           .set("parentLocation", MakeRec())
                                       );
          // answer "location queries" backed by this structure
          GenNodeLocationQuery locationQuery{dummyUiStructure};
          MockLoationSolver mock ([&]{ return new UILocationSolver{locationQuery}; });
          //--------------------------------------------------------------(End)Test-Fixture
          
          
          
          uint allocCounter = 0;
          
          // Simulation/Example for an allocator-builder
          AllocSpec<uint> limitAllocation =[&](UICoord target, uint limit)
                                              {
                                                if (allocCounter < limit)
                                                  return target.tab(++allocCounter);
                                                else
                                                  return target.tab(limit);
                                              };
          
          // the actual View Specification would then be written as...
          ViewSpec locate = UICoord::currentWindow().panel("parentLocation");
          Allocator alloc = limitAllocation(3);
          
          // ...and it would be evaluated as follows
          UICoord targetLocation = locate("viewID");
          UICoord realView1 = alloc(targetLocation);
          CHECK (1 == allocCounter);
          CHECK (string{realView1} == "UI:win-1[perspective]-parentLocation.viewID.#1");
          
          UICoord realView2 = alloc(targetLocation);
          CHECK (2 == allocCounter);
          CHECK (string{realView2} == "UI:win-1[perspective]-parentLocation.viewID.#2");
          CHECK (realView2 != realView1);
          
          UICoord realView3 = alloc(targetLocation);
          CHECK (3 == allocCounter);
          CHECK (string{realView3} == "UI:win-1[perspective]-parentLocation.viewID.#3");
          
          UICoord realView3b = alloc(targetLocation);
          CHECK (3 == allocCounter);
          CHECK (realView3b == realView3);
        }
      
      
      void
      verify_alternatives()
        {
          UNIMPLEMENTED ("querying and selection of location alternatives");
        }
      
      
      void
      verify_genericInvocation()
        {
          ViewLocator viewLocator;
          
           //-------------------------------------------------------------Test-Fixture
          // a Test dummy placeholder for the real UI structure
          Rec dummyUiStructure = MakeRec()
                                   .set("win-1"
                                       , MakeRec()
                                           .type("perspective")
                                           .set("parentLocation", MakeRec())
                                       );
          // answer "location queries" backed by this structure
          GenNodeLocationQuery locationQuery{dummyUiStructure};
          MockLoationSolver mock ([&]{ return new UILocationSolver{locationQuery}; });
          
          MockElementAccess fakeAccessor;
          //--------------------------------------------------------------(End)Test-Fixture
          
          using idi::MockView1;
          
          MockView1& view1 = viewLocator.get<MockView1>();
//        TimelineView timeline = viewLocator.get<TimelineView>();
          
          /////////////////////////////////////////////////////////////////////////////////////////TICKET 1129 : use an EventLog to verify the forwarded invocations??
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ViewSpecDSL_test, "unit gui");
  
  
}}} // namespace gui::interact::test
