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
 ** 
 ** @note as of 1/2015 this is a first draft and WIP-WIP-WIP
 ** @todo WIP  ///////////////////////TICKET #1134
 ** 
 ** @see ElementAccess_test usage example
 ** @see elem-access-dir.hpp real implementation
 ** 
 */


#ifndef GUI_TEST_ELEMENT_ACCESS_H
#define GUI_TEST_ELEMENT_ACCESS_H


#include "lib/error.hpp"
#include "gui/model/element-access.hpp"
#include "gui/interact/ui-coord.hpp"
#include "test/mock-elm.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <string>


  
namespace gui  {
namespace test {
  
//  using util::isnil;
//  using std::string;
  using interact::UICoord;
  
  
  /* === Dummy Widgets for Unit testing === */
  
  class DummyWidget
    : public MockElm
    {
    protected:
      virtual ~DummyWidget() { } ///< is an interface
      DummyWidget()
        : MockElm("DummyWidget")
        { }
    };
  
  class DummyTab
    : public DummyWidget
    { };
  
  
  
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
      UICoord expectedQuery;
      
      /** ...and if acceptable, the next query will answer with this object */
      model::Tangible* expectedAnswer;
      
      
      /* == ElementAccess interface == */
      
      RawResult
      performAccessTo (UICoord const& target, size_t limitCreation)  override
        {
          CHECK (target == expectedQuery);
          return expectedAnswer;
        }
      
      
    protected:
    };
  
  
  
  
}} // namespace gui::test
#endif /*GUI_TEST_ELEMENT_ACCESS_H*/
