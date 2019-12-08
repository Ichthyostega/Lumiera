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
        int i = rand(); // "identity"
        
        friend bool
        operator== (DummyWidget const& wa, DummyWidget const& wb)
        {
          return wa.i == wb.i; // compare identity
        }
      };
    
    using HookedWidget = ViewHook<DummyWidget>;
    
    
    
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
        allWidgetIDs()  const
          {
            return lib::treeExplore(widgets_)
                       .transform([](Attachment const& entry)
                                    {
                                      return entry.widget.i;
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
        testContains (int someWidgetID)
          {
            return util::linearSearch (allWidgetIDs(), someWidgetID);
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
        
        HookedWidget
        hook (DummyWidget& elm, int xPos, int yPos)  override
          {
            addDummy(elm, xPos,yPos);
            return HookedWidget{elm, *this};
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
        rehook (HookedWidget& existingHook)  noexcept override
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
          verify_multiplicity();
          relocateWidget();
          reOrderHooked();
        }
      
      
      /** @test the standard use case is to hook up a widget to a canvas for display.
       */
      void
      verify_standardUsage()
        {
          FakeCanvas canvas;
          CHECK (canvas.empty());
          {
            HookedWidget widget{canvas};
            CHECK (canvas.testContains (widget.i));
            CHECK (not canvas.empty());
          }// hook goes out of scope...
          CHECK (canvas.empty());
        }
      
      
      /** @test each hooking has a distinct identity and is managed on its own.
       */
      void
      verify_multiplicity()
        {
          FakeCanvas canvas;
          CHECK (canvas.empty());
          
          HookedWidget widget{canvas};
          CHECK (canvas.testContains (widget.i));
          CHECK (not canvas.empty());
          
          int someID;
          {
            HookedWidget otherWidget{canvas};
            someID = otherWidget.i;
            CHECK (canvas.testContains (someID));
            CHECK (canvas.testContains (widget.i));
          }// hook goes out of scope...
          CHECK (not canvas.testContains (someID));
          CHECK (canvas.testContains (widget.i));
          CHECK (not canvas.empty());
          
          DummyWidget cloneWidget{std::move (widget)};
          CHECK (not canvas.testContains (cloneWidget.i));
          CHECK (canvas.empty());
        }
      
      
      /** @test hook a widget at a specific position and then later
       *   relocate it on the canvas through the ViewHookable front-end.
       */
      void
      relocateWidget()
        {
          int x1 = rand() % 100;
          int y1 = rand() % 100;
          int x2 = rand() % 100;
          int y2 = rand() % 100;
          int x3 = rand() % 100;
          int y3 = rand() % 100;
          
          FakeCanvas canvas;
          HookedWidget w1{canvas.hookedAt(x1,y1)};
          HookedWidget w3{canvas.hookedAt(x3,y3)};
          
          int id2;
          {
            HookedWidget w2{canvas.hookedAt(x2,y2)};
            id2 = w2.i;
            CHECK (canvas.testContains (id2));
            CHECK (canvas.testVerifyPos (w2, x2,y2));
            
            int newX = ++x2;
            int newY = --y2;
            w2.moveTo (newX,newY);
            
            CHECK (canvas.testVerifyPos (w2, newX,newY));
            CHECK (canvas.testVerifyPos (w1, x1,y1));
            CHECK (canvas.testVerifyPos (w3, x3,y3));
          }
          CHECK (not canvas.testContains (id2));
          CHECK (canvas.testVerifyPos (w1, x1,y1));
          CHECK (canvas.testVerifyPos (w3, x3,y3));
        }
      
      
      /** @test a mechanism to re-attach elements in changed order.
       */
      void
      reOrderHooked()
        {
          using WidgetVec = vector<HookedWidget>;
          
          // create 20 (random) widgets and hook them onto the canvas
          WidgetVec widgets;
          FakeCanvas canvas;
          for (uint i=0; i<10; ++i)
            widgets.emplace_back (canvas);
          
          CHECK (canvas.testContainsSequence (widgets));
          
          // now lets assume the relevant order of the widgets has been altered
          shuffle (widgets.begin(),widgets.end(), std::random_device());
          CHECK (not canvas.testContainsSequence (widgets));
          
          // so we need to re-construct the canvas attachments in the new order
          canvas.reOrder (eachElm (widgets));
          CHECK (canvas.testContainsSequence (widgets));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ViewHook_test, "unit gui");
  
  
}}} // namespace stage::model::test
