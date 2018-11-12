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

/** @file element-access-test.cpp
 ** unit test \ref ElementAccess_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
//#include "gui/interact/view-spec-dsl.hpp"
#include "test/test-element-access.hpp"
#include "gui/interact/ui-coord.hpp"
//#include "gen-node-location-query.hpp"
#include "lib/depend-inject.hpp"
#include "lib/format-cout.hpp"
//#include "lib/idi/entry-id.hpp"
//#include "lib/diff/gen-node.hpp"
#include "lib/util.hpp"

//#include <string>
//#include <vector>


//using std::string;
//using lib::diff::MakeRec;
//using lib::diff::Rec;
//using lib::idi::EntryID;
//using lib::diff::GenNode;
using util::isSameObject;
//using util::isnil;


namespace gui  {
namespace model{
namespace test {
  
//  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
//  using lib::test::showSizeof;
  using interact::UICoord;
  using gui::test::TestElementAccess;
  using gui::test::DummyWidget;
  using gui::test::DummyView;
  using gui::test::DummyTab;
  
  using MockAccess = lib::DependInject<ElementAccess>::Local<TestElementAccess>;
  using AccessAPI = lib::Depend<ElementAccess>;
  
  
  
  /******************************************************************************//**
   * @test verify the usage pattern of low-level UI element access, based on a
   *       mock implementation of the accessor directory.
   * @todo 4/2018 in the course of establishing an UI backbone, it is sufficient
   *       just to _have_ that abstraction interface; so the test focuses merely
   *       on the invocation, and documents how the mock be used. Which is a
   *       prerequisite to get the ViewSpecDSL_test finished. The intention is
   *       to elaborate the mock in a second step later and use it to build a
   *       draft of the implementation mechanics, but based on `Rec<GenNode>`
   *       rather than on the real UI topology.
   * @see GenNodeLocationQuery
   * 
   * @see [Mock testing support](\ref test-element-access.hpp)
   * @see id-scheme.hpp
   * @see ViewLocator
   * @see ViewSpecDSL_test
   */
  class ElementAccess_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verify_simpleAccess();
          verify_standardUsage();
          verify_limitedCreate();
          verify_createNewPath();
        }
      
      
      /** @test simple access to an existing element designated by coordinates */
      void
      verify_simpleAccess()
        {
          MockAccess fakeDirectory;
          
          auto location = UICoord{"win-1","persp-A","thePanel","someView","tab#5"};
          
          fakeDirectory.triggerCreate();
          fakeDirectory->existingPath = location;
          fakeDirectory->response.reset (new DummyTab);
          
          AccessAPI accessAPI;
          auto answer = accessAPI().access<DummyWidget> (location);
          
          CHECK (answer.isValid());
          DummyWidget& widget = answer;
          CHECK (INSTANCEOF (DummyTab, &widget));
          CHECK (isSameObject (widget, *fakeDirectory->response));
        }
      
      
      /** @test the standard use case is to create one new child node
       *        below an existing path (widget) within the UI
       */
      void
      verify_standardUsage()
        {
          MockAccess fakeDirectory;
          
          auto path     = UICoord{"win-1","persp-A","thePanel"};
          auto location = UICoord{"win-1","persp-A","thePanel","someView"};
          
          fakeDirectory.triggerCreate();
          fakeDirectory->existingPath = path;
          CHECK (not fakeDirectory->response);
          
          AccessAPI accessAPI;
          DummyView& view = accessAPI().access<DummyView> (location);
          CHECK (    fakeDirectory->response); // has been created
          CHECK (isSameObject (view, *fakeDirectory->response));
        }
      
      
      void
      verify_limitedCreate()
        {
          UNIMPLEMENTED ("limit creation of new sibling objects");
        }
      
      
      void
      verify_createNewPath()
        {
          UNIMPLEMENTED ("create a new path from scratch");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ElementAccess_test, "unit gui");
  
  
}}} // namespace gui::model::test
