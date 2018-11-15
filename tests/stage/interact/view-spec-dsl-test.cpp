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
#include "lib/format-cout.hpp"   ////////////////TODO only while this test is under development
//#include "lib/idi/entry-id.hpp"
//#include "lib/diff/gen-node.hpp"
#include "lib/util.hpp"

//#include <string>
//#include <vector>


using std::string;
using lib::diff::MakeRec;
using lib::diff::Rec;
//using lib::idi::EntryID;
//using lib::diff::GenNode;
//using util::isSameObject;
//using util::isnil;
using util::contains;


namespace gui {
namespace idi { //------Mock ViewSpec definitions for component test
  
  
  /* ==== Dummy ViewSpec rules for those two mock view types (--> see id-scheme.hpp) ==== */
    
  template<>
  struct Descriptor<test::DummyView>
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
      
      
      /** @test generic integrated access through ViewLocator
       * This test demonstrates and verifies the way ViewLocator combines type based
       * selection of the applicable DSL clauses, the invocation of those DSL definitions,
       * the allocation of a suitable element and finally specifically typed access to this
       * located or allocated element.
       * @remarks due to limitations of our unit test setup (GTK is prohibited), this component
       *          integration test can not be performed against the actual DSL definitions of the
       *          real UI, even while it uses the actual code from ViewLocator. Simply because we
       *          can not instantiate UI widgets in a unit test. We have to resort to mock UI
       *          elements and thus use a dummy "view type" together with faked DSL definitions
       *          for this dummy. These definitions are given in the test fixture above, right
       *          within this translation unit. (see `namespace idi` and the class `MockView1`).
       */
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
          fakeAccessor.triggerCreate();
          //--------------------------------------------------------------(End)Test-Fixture
          
          
          //--------------------------------------------------------------Staging: Testcase-1
          fakeAccessor->existingPath = UICoord{"win-1","perspective","parentLocation"};
          CHECK (not fakeAccessor->response); // not yet created
          //--------------------------------------------------------------Staging: Testcase-1
          
          DummyView& view1 = viewLocator.get<DummyView>();
          cout << "created view:" << view1.getID() << endl;
          CHECK (fakeAccessor->response);                   // a new "widget" was created
          CHECK (contains (view1.getID(), "DummyView"));    // using the type name as ID prefix
                                                      /////////////////////////////////////////////TICKET 1129 : some way to verify the last allocated path. Should be a child of "parentLocation"
          
          /////////////////////////////////////////////////////////////////////////////////////////TICKET 1129 : use an EventLog to verify the forwarded invocations??
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ViewSpecDSL_test, "unit gui");
  
  
}}} // namespace gui::interact::test
