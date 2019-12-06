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
#include "lib/iter-adapter-stl.hpp"
#include "lib/util.hpp"

#include <vector>
#include <forward_list>
#include <algorithm>
#include <random>


using util::contains;
using util::isSameObject;
using lib::iter_stl::eachElm;
using std::vector;
using std::forward_list;
using std::shuffle;


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
          return wa.i == wb.i; // compare identity
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
        
        auto
        findEntry (DummyWidget const& someWidget)
          {
            return std::find_if (widgets_.begin()
                                ,widgets_.end()
                                , [&](Attachment const& a) { return a.widget == someWidget; });
          }
        
      public:
        /* === diagnostic functions for the test === */
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
        
        bool
        testVerifyPos (DummyWidget const& someWidget, int x_expected, int y_expected)
          {
            auto end = widgets_.end();
            auto pos = findEntry (someWidget);
            return pos != end
               and pos->posX == x_expected
               and pos->posY == y_expected;
          }
        
        /** verify our internal sequence matches the given one */
        template<class CON>
        bool
        testContainsSequence (CON const& refSeq)
          {
            // NOTE the implementation twist of using a std::forward_list,
            //      which causes reversed order of our internal elements
            auto iRef = refSeq.crbegin();
            auto eRef = refSeq.crend();
            auto iInt = widgets_.cbegin();
            auto eInt = widgets_.cend();
            for ( ; iRef != eRef and iInt != eInt; ++iRef, ++iInt)
              if (not (iInt->widget == **iRef)) return false;
            return iRef == eRef
               and iInt == eInt;
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
            auto end = widgets_.end();
            auto pos = findEntry (elm);
            if (pos != end)
              {
                pos->posX = xPos;
                pos->posY = yPos;
              }
          }
        
        void
        remove (DummyWidget& elm)  noexcept override
          {
            widgets_.remove_if ([&](Attachment const& a) { return a.widget == elm; });
          }
        
        
        void
        rehook (WidgetViewHook& existingHook)  noexcept override
          {
            auto pos = findEntry (*existingHook);
            REQUIRE (pos != widgets_.end(), "the given iterator must yield previously hooked-up elements");
            Attachment existing{*pos};
            this->remove (existing.widget);
            this->addDummy(existing.widget, existing.posX,existing.posY);
          }
      };
  }
  
  
  
  
  /*************************************************************************************//**
   * @test verify the mechanism to attach widgets to a canvas, while keeping the
   *       canvas implementation itself opaque.
   *       - manage the attachment and detach automatically
   *       - ability to adjust the location parameter of an existing attachment
   *       - ability to re-attach existing attachments in a new sequence order
   * @remark this test focuses on the concepts and the API, and thus uses a
   *         dummy implementation of the "Canvas", which just registers a
   *         list of widgets with a dedicated "position" for each.
   * @see view-hook.hpp
   */
  class ViewHook_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verify_standardUsage();
          relocateWidget();
          reOrderHooked();
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
      
      
      /** @test relocate a widget on the canvas through the ViewHook handle
       */
      void
      relocateWidget()
        {
          FakeCanvas canvas;
          DummyWidget w1, w2, w3;
          WidgetViewHook h1 = canvas.hook (w1, w1.x,w1.y);
          WidgetViewHook h3 = canvas.hook (w3, w3.x,w3.y);
          {
            WidgetViewHook h2 = canvas.hook (w2, w2.x,w2.y);
            CHECK (canvas.testVerifyPos (w2, w2.x,w2.y));
            
            int newX = ++w2.x;
            int newY = --w2.y;
            h2.moveTo (newX,newY);
            
            CHECK (canvas.testVerifyPos (w2, newX,newY));
            CHECK (canvas.testVerifyPos (w1, w1.x,w1.y));
            CHECK (canvas.testVerifyPos (w3, w3.x,w3.y));
          }
          CHECK (canvas.testVerifyPos (w1, w1.x,w1.y));
          CHECK (canvas.testVerifyPos (w3, w3.x,w3.y));
          CHECK (not canvas.testContains (w2));
        }
      
      
      /** @test a mechanism to re-attach elements in changed order.
       */
      void
      reOrderHooked()
        {
          using WidgetVec = vector<DummyWidget>;
          using HookVec = vector<WidgetViewHook>;
          
          // create 20 (random) widgets and hook them onto the canvas
          WidgetVec widgets{20};
          FakeCanvas canvas;
          HookVec hooks;
          for (auto& w : widgets)
            hooks.emplace_back (canvas.hook (w, w.x,w.y));
          
          CHECK (canvas.testContainsSequence (hooks));
          
          // now lets assume the relevant order of the widgets has been altered
          shuffle (hooks.begin(),hooks.end(), std::random_device());
          CHECK (not canvas.testContainsSequence (hooks));
          
          // so we need to re-construct the canvas attachments in the new order
          canvas.reOrder (eachElm (hooks));
          CHECK (canvas.testContainsSequence (hooks));
        }
      
      
      /** @test  */
    };
  
  
  /** Register this test class... */
  LAUNCHER (ViewHook_test, "unit gui");
  
  
}}} // namespace stage::model::test
