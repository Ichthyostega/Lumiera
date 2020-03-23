/*
  ViewHook(Test)  -  verify abstracted presentation attachment

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
#include "lib/scoped-collection.hpp"
#include "lib/iter-tree-explorer.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/util.hpp"

#include <list>
#include <vector>
#include <algorithm>
#include <random>


using util::isnil;
using util::contains;
using util::isSameObject;
using lib::iter_stl::eachElm;
using std::shuffle;
using std::vector;


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
    
    using HookedWidget = ViewHooked<DummyWidget>;
    
    
    
    class FakeCanvas
      : public ViewHook<DummyWidget>
      {
        std::list<DummyWidget> widgets_;
        
        auto
        allWidgetIDs()  const
          {
            return lib::treeExplore(widgets_)
                       .transform([](DummyWidget const& entry)
                                    {
                                      return entry.i;
                                    });
          }
        
        auto
        findEntry (DummyWidget const& someWidget)
          {
            return std::find_if (widgets_.begin()
                                ,widgets_.end()
                                , [&](DummyWidget const& widget) { return widget == someWidget; });
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
        
        /** verify our internal sequence matches the given one */
        template<class IT>
        bool
        testContainsSequence (IT refSeq)
          {
            auto ids = allWidgetIDs();
            for ( ; refSeq and ids; ++refSeq, ++ids)
              if (refSeq->i != *ids) break;
            return isnil(refSeq)
               and isnil(ids);
          }
        
        
        /* === Interface ViewHook === */
        
        void
        hook (DummyWidget& elm)  override
          {
            widgets_.push_back (elm);
          }
        
        void
        remove (DummyWidget& elm)  override
          {
            widgets_.remove_if ([&](auto const& widget) { return widget == elm; });
          }
        
        
        void
        rehook (DummyWidget& existingHook)  noexcept override
          {
            auto pos = findEntry (existingHook);
            REQUIRE (pos != widgets_.end(), "the given iterator must yield previously hooked-up elements");
            DummyWidget& widget{*pos};
            this->remove (widget);
            this->hook (widget);
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
   * @see canvas-hook.hpp
   */
  class ViewHook_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verify_standardUsage();
          verify_multiplicity();
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
        }
      
      
      /** @test a mechanism to re-attach elements in changed order.
       * @remarks this test looks somewhat convoluted, because `ViewHooked<W>` is defined
       *          to be non-copyable (for good reason, since we can assume that the canvas
       *          somehow maintains a pointer to each widget added, so we can not allow the
       *          attached widgets to move in memory). However, in practice the "order sequence"
       *          is typically defined as a delegating iterator over some sequence of model elements,
       *          which themselves are managed as a STL container of `std::unique_ptr`; thus it is
       *          very much possible to alter the sequence of the model elements, without actually
       *          touching the memory location of the slave widgets used for presentation.
       */
      void
      reOrderHooked()
        {
          using Widgets = lib::ScopedCollection<HookedWidget>;
          using OrderIdx  = vector<HookedWidget*>;
          
          FakeCanvas canvas; // WARNING: Canvas must outlive the widgets!
          
          // create 20 (random) widgets and hook them onto the canvas
          Widgets widgets{20};
          OrderIdx orderIdx;
          for (uint i=0; i<20; ++i)
            orderIdx.push_back (& widgets.emplace<HookedWidget>(canvas));
          
          // helper: use the orderIdx to generate sequence of widget refs (not pointers)
          auto orderSequence = [&] { return lib::ptrDeref(eachElm(orderIdx)); };
          
          CHECK (canvas.testContainsSequence (eachElm(widgets)));
          CHECK (canvas.testContainsSequence (orderSequence()));
          
          // now lets assume the relevant order of the widgets has been altered
          shuffle (orderIdx.begin(),orderIdx.end(), std::random_device());
          CHECK (not canvas.testContainsSequence (orderSequence()));
          
          // so we need to re-construct the canvas attachments in the new order
          canvas.reOrder (orderSequence());
          CHECK (canvas.testContainsSequence (orderSequence()));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ViewHook_test, "unit gui");
  
  
}}} // namespace stage::model::test
