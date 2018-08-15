/*
  WLink(Test)  -  verify managed link to sigc::trackable widget

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

/** @file w-link-test.cpp
 ** unit test \ref WLink_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "gui/model/w-link.hpp"
//#include "gen-node-location-query.hpp"
#include "lib/format-cout.hpp"
//#include "lib/idi/entry-id.hpp"
//#include "lib/diff/gen-node.hpp"
//#include "lib/util.hpp"

//#include <string>
#include <memory>


//using std::string;
//using lib::diff::MakeRec;
//using lib::diff::Rec;
//using lib::idi::EntryID;
//using lib::diff::GenNode;
using std::make_unique;
//using util::isSameObject;
//using util::isnil;


namespace gui  {
namespace model{
namespace test {
  
//  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
//  using lib::test::showSizeof;
  
  namespace { // Test fixture...
    
    template<typename X>
    struct DummyWidget
      : public sigc::trackable
      {
        X val;
      };
  }
  
  
  
  
  /******************************************************************************//**
   * @test verify the usage pattern of low-level UI element access, based on a
   *       mock implementation of the accessor directory.
   * @see w-link.hpp
   */
  class WLink_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verify_standardUsage();
          verify_reconnect();
          verify_copy();
        }
      
      
      /** @test the standard use case is to hold onto a widget
       *        owned by _someone else_, with automatic disconnection
       */
      void
      verify_standardUsage()
        {
          using Wint = DummyWidget<int>;
          int r{rand() % 100};
          
          auto widget = make_unique<Wint>();
          widget->val = r;
          
          WLink<Wint> link{*widget};
          CHECK (link);
          link->val += 23;
          CHECK (r+23 == widget->val);
          
          // kill widget
          widget.reset();
          CHECK (not link);
          VERIFY_ERROR (BOTTOM_VALUE, link->val );
        }
      
      
      void
      verify_reconnect()
        {
          UNIMPLEMENTED ("change connection state");
        }
      
      
      void
      verify_copy()
        {
          UNIMPLEMENTED ("copy with magic");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (WLink_test, "unit gui");
  
  
}}} // namespace gui::model::test
