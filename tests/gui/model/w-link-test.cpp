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
#include "lib/util.hpp"

//#include <string>
#include <memory>


//using std::string;
//using lib::diff::MakeRec;
//using lib::diff::Rec;
//using lib::idi::EntryID;
//using lib::diff::GenNode;
using std::make_unique;
using util::isSameObject;
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
        X val = 1 + rand() % 100;
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
          auto widget = make_unique<Wint>();
          int r = widget->val;
          
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
          using Wint = DummyWidget<int>;
          
          auto w1 = make_unique<Wint>();
          auto w2 = make_unique<Wint>();
          int r1 = w1->val;
          int r2 = w2->val;
          
          WLink<Wint> l1;
          WLink<Wint> l2{*w1};
          CHECK (not l1);
          CHECK (    l2);
          
          l2->val = r1;
          l1.connect(*l2);
          ++l1->val;
          CHECK (w1->val == r1+1);
          CHECK (isSameObject (*l1, *l2));
          
          l2.connect(*w2);
          CHECK (not isSameObject (*l1, *l2));
          w2->val = r2;
          CHECK (r1+1 == l1->val);
          CHECK (r2   == l2->val);
          
          w1.reset();  // kill first widget
          CHECK (not l1);
          CHECK (    l2);
          l2->val *= -10;
          l2.clear();
          CHECK (not l1);
          CHECK (not l2);
          CHECK (-10*r2 == w2->val);
          l1.connect(*w2);
          l2.connect(*l1);
          CHECK (-10*r2 == l2->val);
          CHECK (isSameObject (*l1, *l2));
          CHECK (isSameObject (*l1, *w2));
          
          // implicitly kill second widget
          *w2 = Wint{};
          CHECK (not l1);
          CHECK (not l2);
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
