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
#include "lib/util.hpp"

//#include <utility>
//#include <memory>
#include <vector>


using util::isSameObject;
using util::contains;
using util::isnil;
//using std::make_unique;
//using std::move;
using std::vector;


namespace stage{
namespace model{
namespace test {
  
  
  namespace { // Test fixture...
    
//  template<typename X>
    struct DummyWidget
//    : public sigc::trackable
      {
        int x = rand() % 100;
        int y = rand() % 100;
        
        friend bool
        operator== (DummyWidget const& wa, DummyWidget const& wb)
        {
          return wa.x == wb.x
             and wa.y == wb.y;
        }
      };
    
    using WidgetViewHook = ViewHook<DummyWidget>;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1201 : TODO actually implement this generic for ViewHook
    template<class ELM>
    class ViewHookable
      {
      public:
        virtual ~ViewHookable() { }    ///< this is an interface
        
        virtual ViewHook<ELM> hook (ELM& elm, int xPos, int yPos)  =0;
      };
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1201 : TODO actually implement this generic for ViewHook

    class FakeCanvas
      : public ViewHookable<DummyWidget>
      {
        vector<DummyWidget> widgets_;
        
        void
        addDummy(DummyWidget& widget, int x, int y)
          {
            UNIMPLEMENTED ("add widget at given position");
          }
        
      public:
        using const_reference = DummyWidget const&;
        using const_iterator = vector<DummyWidget>::const_iterator;
        
        bool           empty() const { return widgets_.empty(); }
        const_iterator begin() const { return widgets_.begin(); }
        const_iterator end()   const { return widgets_.end(); }
        
        
        /* === Interface ViewHookable === */
        
        WidgetViewHook
        hook (DummyWidget& elm, int xPos, int yPos)  override
          {
            addDummy(elm, xPos,yPos);
            TODO ("actually do something to hook it up...");
            return WidgetViewHook{};
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
            WidgetViewHook hook = canvas.hook(widget, widget.x, widget.y);
            CHECK (isSameObject (*hook, widget));
            CHECK (contains (canvas, widget));
            CHECK (not isnil (canvas));
          }// hook goes out of scope...
          CHECK (not contains (canvas, widget));
          CHECK (isnil (canvas));
        }
      
      
      /** @test  */
    };
  
  
  /** Register this test class... */
  LAUNCHER (ViewHook_test, "unit gui");
  
  
}}} // namespace stage::model::test
