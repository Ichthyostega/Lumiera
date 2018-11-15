/*
  TEST-ELEMENT-ACCESS.hpp  -  fake access directory to handle generic UI entities for test

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

*/


/** @file test-element-access.hpp
 ** Unit test helper for access to UI elements without actually running an UI.
 ** This allows to cover functionality for resolving UI-coordinates against an UI topology
 ** and accessing or creating elements. Obviously, only faked UI widgets can be returned,
 ** but this does not matter for those features anyway.
 ** 
 ** @note as of 4/2018 this is a first draft and WIP-WIP-WIP
 ** @todo right now this test is braindead; the idea is to extend it similar to the
 **       GenNodeLocationQuery eventually, so to mimic the way an actual implementation
 **       would drill down into the UI topology. Yet at the moment (4/2018) we are still
 **       far from implementing anything in this regard; we just need the interface...  ///////////////////////////////TICKET #1134
 ** 
 ** @see ElementAccess_test usage example
 ** @see elem-access-dir.hpp real implementation
 ** 
 */


#ifndef GUI_TEST_ELEMENT_ACCESS_H
#define GUI_TEST_ELEMENT_ACCESS_H


#include "lib/error.hpp"
#include "stage/model/element-access.hpp"
#include "stage/interact/ui-coord.hpp"
#include "test/mock-elm.hpp"
#include "lib/symbol.hpp"
//#include "lib/util.hpp"

#include <string>
#include <memory>


  
namespace gui  {
namespace test {
  namespace error = lumiera::error;
  
//  using util::isnil;
  using std::string;
  using lib::Literal;
  using interact::UICoord;
  using interact::UIC_VIEW;
  using interact::UIC_TAB;
  
  
  
  
  /* === Dummy Widgets for Unit testing === */
  
  class DummyWidget
    : public MockElm
    {
    public:
      virtual ~DummyWidget() { } ///< is an interface
      
      DummyWidget(string name ="DummyWidget")
        : MockElm{name}
        { }
      DummyWidget(Literal name)
        : DummyWidget{string (name)}
        { }
    };
  
  struct DummyTab
    : DummyWidget
    {
      using DummyWidget::DummyWidget;
    };
  
  struct DummyView
    : DummyWidget
    {
      using DummyWidget::DummyWidget;
    };
  
  
  
  /**
   * Mock implementation of the model::ElementAccess interface for testing without actual UI.
   * @see ElementAccess_test
   */
  class TestElementAccess
    : public model::ElementAccess
    {
      
    public:
      explicit
      TestElementAccess ()
        { }
      
      
      /* == Test/Diagnostics interface == */
      
      /** the next query will fail unless it presents this Path */
      UICoord existingPath;
      
      /** ...and if acceptable, we record the last answer here */
      std::unique_ptr<model::Tangible> response;
      
      
      /* == ElementAccess interface == */
      
      RawResult
      performAccessTo (UICoord::Builder & target, size_t limitCreation)  override
        {
          UICoord const& location = target.uiCoord();
          
          CHECK (existingPath >= location);
          if (existingPath > location and !response)
            {
              if (location.leafLevel() == UIC_VIEW)
                response.reset(new DummyView(location[UIC_VIEW]));
              else
              if (location.leafLevel() == UIC_TAB)
                response.reset(new DummyTab(location[UIC_TAB]));
              else
                throw error::Invalid("Mock ElementAccess supports only creation of VIEW and TAB. Requested Target was "+string(location));
            }
          
          return response.get();
        }
      
      
    protected:
    };
  
  
  
  
}} // namespace gui::test
#endif /*GUI_TEST_ELEMENT_ACCESS_H*/
