/*
  BodyCanvasWidget  -  custom drawing canvas to display the timeline body

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file body-canvas-widget.cpp
 ** Implementation details of timeline custom drawing.
 ** 
 ** @todo WIP-WIP-WIP as of 6/2019
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/timeline/body-canvas-widget.hpp"
#include "stage/timeline/display-manager.hpp"
#include "stage/timeline/track-profile.hpp"
#include "stage/timeline/track-body.hpp"

//#include "stage/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>
#include <utility>



//using util::_Fmt;
using util::max;
//using util::contains;
//using Gtk::Widget;
using Gdk::Rectangle;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;
using std::move;


namespace stage {
namespace timeline {
  
  using CairoC = Cairo::RefPtr<Cairo::Context> const&;
  
  namespace { // details of track background painting
    
    const int INITIAL_TIMERULER_HEIGHT_px = 30;
    
    
    class TrackGroundingRenderer
      : public ProfileInterpreter
      {
        CairoC cox_;
        PixSpan display_;
        
        
        /** paint the top of the track body area
         * @param f number of consecutive track elements
         *          to keep pinned (always visible) at top */
        void
        prelude (uint f)  override
          {
            UNIMPLEMENTED ("draw timeline top");
          }
        
        /** finish painting the track body area
         *  @param pad additional padding to add at bottom */
        void
        coda (uint pad)  override
          {
            UNIMPLEMENTED ("draw bottom");
          }
        
        /** draw grounding of a overview/ruler track
         *  with the given height */
        void
        ruler (uint h)  override
          {
            UNIMPLEMENTED ("draw ruler");
          }
        
        /** render additional padding/gap */
        void
        gap (uint h)  override
          {
            UNIMPLEMENTED ("draw gap");
          }
        
        /** fill background of track content area
         *  with the given vertical extension */
        void
        content (uint h)  override
          {
            UNIMPLEMENTED ("paint background of content area");
          }
        
        /** paint opening slope to enter nested sub tracks
         * @note we only ever open one level deep a time */
        void
        open()  override
          {
            UNIMPLEMENTED ("paint downward slope");
          }
        
        /** paint closing slope to finish nested sub tracks
         * @param n number of nested levels to close */
        void
        close (uint n)  override
          {
            UNIMPLEMENTED ("paint upward slope");
          }
        
        
      public:
        TrackGroundingRenderer (CairoC currentDrawContext, PixSpan toShow)
          : cox_{currentDrawContext}
          , display_{move (toShow)}
          { }
        
        virtual ~TrackGroundingRenderer() { }
      };
  }
  
  
  
  
  TimelineCanvas::TimelineCanvas (_RenderFactory groundingFac, _RenderFactory overlayFac)
    : Gtk::Layout{}
    , getGroundingRenderer_{groundingFac}
    , getOverlayRenderer_{overlayFac}
    { }
  
  
  
  BodyCanvasWidget::~BodyCanvasWidget() { }
  
  
  BodyCanvasWidget::BodyCanvasWidget (DisplayManager& displayManager)
    : Gtk::Box{Gtk::ORIENTATION_VERTICAL}
    , contentArea_{}
    , rulerCanvas_{std::function<Renderer&(CairoC)>(), std::function<Renderer&(CairoC)>()}  ///////////TODO dummy placeholder factories.... need to build the real thing
    , mainCanvas_{std::function<Renderer&(CairoC)>(), std::function<Renderer&(CairoC)>()}
    , layout_{displayManager}
    , profile_{}
    , rootBody_{nullptr}
    {
      this->set_border_width (0);
      this->property_expand() = true;       // dynamically grab any available additional space
      this->pack_start (rulerCanvas_);
      this->pack_start (contentArea_);
      
      contentArea_.set_shadow_type (Gtk::SHADOW_NONE);
      contentArea_.set_policy (Gtk::POLICY_ALWAYS, Gtk::POLICY_AUTOMATIC);  // always need a horizontal scrollbar
      contentArea_.property_expand() = true;                               //  dynamically grab additional space
      contentArea_.add (mainCanvas_);
      
      { // for the initial empty canvas -- use all space the enclosing scrolled window got.
        auto currSize = get_allocation();
        int height = currSize.get_height();
        rulerCanvas_.set_size (currSize.get_width(), INITIAL_TIMERULER_HEIGHT_px);
        mainCanvas_.set_size (currSize.get_width(), max(0, height-INITIAL_TIMERULER_HEIGHT_px));
      }
      
      // realise all initially configured elements....
      this->show_all();
    }
  
  
  
  /**
   * The Lumiera Timeline model does not rely on a list of tracks, as most conventional video editing software does --
   * rather, each sequence holds a _fork of nested scopes._ This recursively nested structure is reflected in the way
   * we organise and draw the timeline representation onto the TimelineCanvas: we use an intermediary entity, the TrackBody
   * as an organisational grouping device, even while we draw _all of the timeline representation_ onto a single global
   * #mainCanvas_ within the (scrollable) #contentArea_. Thus, adding the first TrackBody to represent the root track
   * of a Timeline, will also prepare the grounding for any other nested entities to be drawn on top.
   */
  void
  BodyCanvasWidget::installForkRoot (TrackBody& rootTrackBody)
  {
    rootBody_ = &rootTrackBody;
  }
  
  
  /**
   * 
   */
  TrackProfile&
  BodyCanvasWidget::establishTrackProfile()
  {
    if (rootBody_)
      {
        if (not profile_)
          rootBody_->establishTrackSpace (profile_);
        
//      TrackGroundingRenderer renderer{cox, layout_.getPixSpan()};  //////////TODO TOD-oh
      }
  }
  
  
  /**
   * Custom drawing of the timeline content area.
   * The inherited `Gtk::Layout::on_raw(Context)` handles all drawing of child widgets placed onto the virtual canvas.
   * Thus we need to fill in the structure of the tracks in the timeline background, and any non-standard overlay elements,
   * including tags and markers, indicators, locators (edit point and playhead) and (semi-transparent) range selections.
   * @todo according to the documentation for `signal_draw()`, the framework passes the actually visible area as clipping
   *       region. In theory, this information could be used to reduce the load of canvas painting and repainting, which
   *       becomes crucial for responsiveness on large sessions          ////////////////////////////////////TICKET #1191
   */
  bool
  TimelineCanvas::on_draw (CairoC const& cox)
  {
    // draw track structure behind all widgets
    openCanvas (cox);
    drawGrounding (cox);
    closeCanvas (cox);
    
    // cause child widgets to be redrawn
    bool event_is_handled = Gtk::Layout::on_draw(cox);
    
    // draw dynamic markers and locators on top
    openCanvas (cox);
    drawOverlays (cox);
    closeCanvas (cox);
    
    return event_is_handled;
  }
  
  
  /**
   * Prepare the drawing canvas to work within our virtual canvas coordinate system.
   * @remarks GTK passes a context related to the actual window area; however, we need to create
   *   a uniform virtual canvas, shared by the child widgets, the backgrounding and any overlays.
   *   To make this work, we have to connect to the scrollbar adjustments, since GTK does this
   *   only for the child widgets on the canvas, not for any custom painting.
   */
  void
  TimelineCanvas::openCanvas (CairoC const& cox)
  {
    auto adjH = get_hadjustment();
    auto adjV = get_vadjustment();
    double offH = adjH->get_value();
    double offV = adjV->get_value();
    
    cox->save();
    cox->translate(-offH, -offV);
  }
  
  
  /**
   * Finish and close the virtual drawing canvas established by #openCanvas().
   * Discard any coordinate offsets, stroke and drawing settings applied within.
   */
  void
  TimelineCanvas::closeCanvas (CairoC const& cox)
  {
    cox->restore();
  }
  
  
  /**
   * Establish and render the structure of (possibly nested) tracks and overview rulers.
   * @param cox cairo drawing context for custom drawing, adjusted for our virtual canvas.
   */
  void
  TimelineCanvas::drawGrounding (CairoC const& cox)
  {
//  profile_.performWith (renderer);
    /////////////////////////////////////////////TICKET #1039 : placeholder drawing
    cox->set_source_rgb(0.8, 0.0, 0.0);
    cox->set_line_width (5.0);
    cox->rectangle(0,0, 80, 40);
    cox->stroke();
    /////////////////////////////////////////////TICKET #1039 : placeholder drawing
  }
  
  
  /**
   *
   * @param cox cairo drawing context of the virtual canvas for custom drawing.
   */
  void
  TimelineCanvas::drawOverlays (CairoC const& cox)
  {
    /////////////////////////////////////////////TICKET #1039 : placeholder drawing
    auto alloc = get_allocation();
    int w = alloc.get_width();
    int h = alloc.get_height();
    int rad = MIN (w,h) / 2;
    
    cox->set_source_rgb(0.2, 0.4, 0.9);     // blue
    cox->set_line_width (2.0);
    cox->arc(rad, rad, rad, 0.0, 2.0*M_PI); // full circle
    cox->stroke();
    /////////////////////////////////////////////TICKET #1039 : placeholder drawing
  }
  
  
  
}}// namespace stage::timeline
