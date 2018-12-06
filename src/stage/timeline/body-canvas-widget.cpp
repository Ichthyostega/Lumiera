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
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/timeline/body-canvas-widget.hpp"
#include "stage/timeline/track-body.hpp"

//#include "stage/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

//#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace stage {
namespace timeline {
  
  
  
  TimelineCanvas::TimelineCanvas()
    : Gtk::Layout{}
    , rootBody_{nullptr}
    { }
  
  
  
  BodyCanvasWidget::~BodyCanvasWidget() { }
  
  
  BodyCanvasWidget::BodyCanvasWidget ()
    : Gtk::ScrolledWindow{}
    , canvas_{}
    {
      this->set_shadow_type(Gtk::SHADOW_IN);
      this->set_policy (Gtk::POLICY_ALWAYS, Gtk::POLICY_AUTOMATIC);  // always need a horizontal scrollbar
      this->property_expand() = true;                               //  dynamically grab any available additional space
      this->add(canvas_);
      
      { // for the initial empty canvas -- use all space the enclosing scrolled window got.
        auto currSize = get_allocation();
        canvas_.set_size (currSize.get_width(), currSize.get_height());
      }
      
      // realise all initially configured elements....
      this->show_all();
    }
  
  
  
  /**
   * The Lumiera Timeline model does not rely on a list of tracks, as most conventional video editing software does --
   * rather, each sequence holds a _fork of nested scopes._ This recursively nested structure is reflected in the way
   * we organise and draw the timeline representation onto the TimelineCanvas: we use an intermediary entity, the TrackBody
   * as an organisational grouping device, even while we draw _all of the timeline representation_ onto a single global
   * ::canvas_ within the (scrollable) BodyCanvasWidget. Thus, adding the first TrackBody to represent the root track
   * of a Timeline, will also prepare the grounding for any other nested entities to be drawn on top.
   */
  void
  BodyCanvasWidget::installForkRoot (TrackBody& rootTrackBody)
  {
    canvas_.rootBody_ = &rootTrackBody;
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
  TimelineCanvas::on_draw (Cairo::RefPtr<Cairo::Context> const& cox)
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
  TimelineCanvas::openCanvas (Cairo::RefPtr<Cairo::Context> const& cox)
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
  TimelineCanvas::closeCanvas (Cairo::RefPtr<Cairo::Context> const& cox)
  {
    cox->restore();
  }
  
  
  /**
   * Establish and render the structure of (possibly nested) tracks and overview rulers.
   * @param cox cairo drawing context for custom drawing, adjusted for our virtual canvas.
   */
  void
  TimelineCanvas::drawGrounding (Cairo::RefPtr<Cairo::Context> const& cox)
  {
    /////////////////////////////////////////////TICKET #1139 : placeholder drawing
    cox->set_source_rgb(0.8, 0.0, 0.0);
    cox->set_line_width (5.0);
    cox->rectangle(0,0, 80, 40);
    cox->stroke();
    /////////////////////////////////////////////TICKET #1139 : placeholder drawing 
  }
  
  
  /**
   * 
   * @param cox cairo drawing context of the virtual canvas for custom drawing.
   */
  void
  TimelineCanvas::drawOverlays (Cairo::RefPtr<Cairo::Context> const& cox)
  {
    /////////////////////////////////////////////TICKET #1139 : placeholder drawing
    auto alloc = get_allocation();
    int w = alloc.get_width();
    int h = alloc.get_height();
    int rad = MIN (w,h) / 2;
    
    cox->set_source_rgb(0.2, 0.4, 0.9);     // blue
    cox->set_line_width (2.0);
    cox->arc(rad, rad, rad, 0.0, 2.0*M_PI); // full circle
    cox->stroke();
    /////////////////////////////////////////////TICKET #1139 : placeholder drawing 
  }
  
  
  
}}// namespace stage::timeline
