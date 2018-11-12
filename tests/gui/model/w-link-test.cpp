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
#include "lib/util.hpp"

#include <utility>
#include <memory>


using util::isSameObject;
using std::make_unique;
using std::move;


namespace gui  {
namespace model{
namespace test {
  
  
  namespace { // Test fixture...
    
    template<typename X>
    struct DummyWidget
      : public sigc::trackable
      {
        X val = 1 + rand() % 100;
      };
  }
  
  
  
  
  /******************************************************************************//**
   * @test verify proper behaviour of a smart-link to a `sigc::trackable` GTK widget.
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
      
      
      /** @test registration is automatically maintained when re-assigning targets */
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
      
      
      /** @test registration state is properly handled on copy, move and swap */
      void
      verify_copy()
        {
          using Wint = DummyWidget<int>;
          auto w1 = make_unique<Wint>();
          auto w2 = make_unique<Wint>();
          
          WLink<Wint> l1;
          WLink<Wint> l2{l1};
          CHECK (not l2);
          l2.connect(*w1);
          
          WLink<Wint> l3{l2};
          CHECK (l3);
          CHECK (w1->val == l3->val);
          
          CHECK (not l1); // they are statefull and independent
          l1 = move(l2);
          CHECK (not l2);
          CHECK (l1);
          CHECK (isSameObject (*l1, *l3));
          
          l2 = WLink<Wint>{WLink<Wint>{*w2}};
          CHECK (w2->val == l2->val);
          
          l1 = l3;
          CHECK (w1->val == l1->val);
          WLink<Wint>& ref{l1};
          l1 = move(ref);
          CHECK (w1->val == l1->val);
          CHECK (w1->val == l3->val);
          
          std::swap (l2, l3);
          CHECK (w1->val == l1->val);
          CHECK (w1->val == l2->val);
          CHECK (w2->val == l3->val);
          
          w1.reset();
          CHECK (not l1);
          CHECK (not l2);
          CHECK (w2->val == l3->val);
          
          using Wuint = DummyWidget<uint>;
          auto uu = make_unique<Wuint>();
          WLink<Wuint> lu{*uu};
          
///////////////does not compile...
//        l1 = uu;
//        l1.connect(*uu);
          
          // But it is a compile time check...
          // At runtime, only the bare pointer is managed
          l1 = reinterpret_cast<WLink<Wint>&&> (lu);
          CHECK ((int)uu->val == l1->val);
          CHECK (not lu);               // assignment was actually a move
          
          // even the subversively attached link is managed properly
          uu.reset();
          CHECK (not l1);
          
          // others unaffected...
          CHECK (not l2);
          CHECK (l3);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (WLink_test, "unit gui");
  
  
}}} // namespace gui::model::test
