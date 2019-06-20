/*
  BODY-CANVAS-WIDGET.hpp  -  custom drawing canvas to display the timeline body

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

*/


/** @file body-canvas-widget.hpp
 ** Widget to render the body of timeline display, by custom drawing into
 ** a canvas control. The body part of the timeline display can not be sensibly
 ** implemented with automatic layout by the UI toolkit set. Rather, we need to
 ** control a precise temporal display grid, and we need to limit the actual widgets
 ** added for display, since a given timeline may easily hold several hundred up to
 ** thousands of elements. To ease such tasks, a _canvas control_ -- here implemented
 ** on top of Gtk::Layout, allows to combine _custom drawing_ with the placement of
 ** embedded child widgets, where the latter's layout is again managed automatically
 ** by the toolkit set. This approach allows us to circumvent some of the perils of
 ** custom drawing, where we might forfeit several of the benefits of using a toolkit,
 ** unintentionally of course, and create an UI which feels foreign and brittle
 ** in comparison to standard software.
 ** 
 ** # Layout management
 ** 
 ** To create a consistent timeline layout, the header pane and body need to react
 ** to the same scrolling adjustments, and use the same vertical size allocations for
 ** each embedded track. Such a consistent global layout needs to be built up in a
 ** *display evaluation pass*, carried out collaboratively between the building blocks
 ** of the timeline. To this end, the TrackPresenter and ClipPresenter elements act
 ** as (view) model entities, visited by the timeline::LayoutManager to establish
 ** coherent display parameters. From within this evaluation pass, the individual
 ** presenters communicate with their _slave widgets,_ which are inserted into the
 ** display context of the track header pane or into this body widget respectively.
 ** As result, some new widgets may be injected, existing widgets might be removed or
 ** hidden, and other widgets may be relocated to different virtual canvas coordinates.
 ** 
 ** @todo WIP-WIP-WIP as of 6/2019
 ** 
 */


#ifndef STAGE_TIMELINE_BODY_CANVAS_WIDGET_H
#define STAGE_TIMELINE_BODY_CANVAS_WIDGET_H

#include "stage/gtk-base.hpp"
#include "stage/timeline/track-profile.hpp"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>
#include <functional>



namespace stage  {
namespace timeline {
  
  using CairoC = Cairo::RefPtr<Cairo::Context> const&;
  
  class DisplayManager;
  class TrackBody;
  class TimelineCanvas;
  
  
  class TimelineCanvas
    : public Gtk::Layout
    {
      using _Renderer = std::function<void(CairoC)>;
      
      _Renderer renderGrounding_;
      _Renderer renderOverlay_;
      
    public:
      TimelineCanvas (_Renderer groundingFac, _Renderer overlayFac);
      
    private:
      virtual bool on_draw (CairoC)  override;
      
      void openCanvas (CairoC);
      void closeCanvas (CairoC);
      
      void drawGrounding (CairoC);
      void drawOverlays (CairoC);
    };
  
  
  /**
   * @todo WIP-WIP as of 6/2019
   */
  class BodyCanvasWidget
    : public Gtk::Box
    {
      DisplayManager& layout_;
      TrackProfile profile_;
      TrackBody* rootBody_;
      
      Gtk::ScrolledWindow contentArea_;
      TimelineCanvas rulerCanvas_;
      TimelineCanvas mainCanvas_;
      
    public:
      BodyCanvasWidget (DisplayManager&);
     ~BodyCanvasWidget();
      
      /** @internal Initially install the contents corresponding to the root track fork */
      void installForkRoot (TrackBody& rootTrackBody);
      
      /** @internal allow the header pane to follow our vertical scrolling movement */
      auto
      get_vadjustment()
        {
          return contentArea_.get_vadjustment();
        }
      
    private:/* ===== Internals ===== */
      
      void slotStructureChange()  noexcept;
      TrackProfile& establishTrackProfile();
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_BODY_CANVAS_WIDGET_H*/
