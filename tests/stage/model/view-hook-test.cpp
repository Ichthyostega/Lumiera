/*
  ViewHook(Test)  -  verify abstracted canvas attachment

  Copyright (C)         Lumiera.org
    2019,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file view-hook-test.cpp
 ** unit test \ref ViewHook_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "stage/model/view-hook.hpp"
#include "lib/iter-tree-explorer.hpp"
#include "lib/util.hpp"

//#include <utility>
//#include <memory>
#include <forward_list>


using util::isSameObject;
using util::contains;
//using util::isnil;
//using std::make_unique;
//using std::move;
//using std::vector;
using std::forward_list;


namespace stage{
namespace model{
namespace test {
  
  
  namespace { // Test fixture...
    
    struct DummyWidget
      {
        int x = rand() % 100;
        int y = rand() % 100;
        int i = rand(); // "identity"
        
        friend bool
        operator== (DummyWidget const& wa, DummyWidget const& wb)
        {
          return wa.x == wb.x
             and wa.y == wb.y
             and wa.i == wb.i;
        }
      };
    
    using WidgetViewHook = ViewHook<DummyWidget>;
    
    
    
    class FakeCanvas
      : public ViewHookable<DummyWidget>
      {
        struct Attachment
          {
            DummyWidget& widget;
            int posX, posY;
          };
        forward_list<Attachment> widgets_;
        
        void
        addDummy(DummyWidget& widget, int x, int y)
          {
            widgets_.push_front (Attachment{widget, x,y});
          }
        
        auto
        allWidgets()  const
          {
            return lib::treeExplore(widgets_)
                       .transform([](Attachment const& entry)
                                    {
                                      return entry.widget;
                                    }); 
          }
        
      public:
        
        bool
        empty()  const
          {
            return widgets_.empty();
          }
        
        bool
        testContains (DummyWidget const& someWidget)
          {
            return util::linearSearch (allWidgets(), someWidget);
          }
        
        
        /* === Interface ViewHookable === */
        
        WidgetViewHook
        hook (DummyWidget& elm, int xPos, int yPos)  override
          {
            addDummy(elm, xPos,yPos);
            return WidgetViewHook{elm, *this};
          }
        
        void
        move (DummyWidget& elm, int xPos, int yPos)  override
          {
            UNIMPLEMENTED ("move it");
          }
        
        void
        remove (DummyWidget& elm)  noexcept override
          {
            widgets_.remove_if ([&](Attachment const& a) { return a.widget == elm; });
          }
      };
  }
  
  
  
  
  /*************************************************************************************//**
   * @test verify the mechanism to attach widgets to a canvas, while keeping the
   *       canvas widget itself opaque.
   *       - bla
   *       - blubb
   * @see view-hook.hpp
   */
  class ViewHook_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verify_standardUsage();
        }
      
      
      /** @test the standard use case is to hook up a widget to a canvas for display.
       */
      void
      verify_standardUsage()
        {
          FakeCanvas canvas;
          DummyWidget widget;
          {
            WidgetViewHook hook = canvas.hook (widget, widget.x, widget.y);
            CHECK (isSameObject (*hook, widget));
            CHECK (canvas.testContains (widget));
            CHECK (not canvas.empty());
          }// hook goes out of scope...
          CHECK (not canvas.testContains (widget));
          CHECK (canvas.empty());
        }
      
      
      /** @test  */
    };
  
  
  /** Register this test class... */
  LAUNCHER (ViewHook_test, "unit gui");
  
  
}}} // namespace stage::model::test
