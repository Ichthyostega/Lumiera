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
 ** ## Coordinate systems
 ** When drawing onto a canvas, we need to define the coordinate system. This task is
 ** complicated here, since -- on implementation level -- we end up with several `Gtk::Layout`
 ** elements (the actual canvas widget). This is necessary to accommodate for the display
 ** "mechanics": part of the timeline has to stay "pinned" on top, including the time
 ** overview ruler and possible further marker displays. Based on practical considerations
 ** we decide to handle this situation as follows
 ** - Each drawing canvas gets its own coordinate system; its extension is defined to
 **   match the size of the scrolling area, and coordinates are adjusted so to match
 **   drawing primitives and coordinates of attached sub-widgets
 ** - However, the timeline as a whole constitutes a global coordinate system on its own.
 **   It is the actual drawing codes's responsibility to translate into canvas coordinates.
 ** - To hide those complexities from the display management code, we introduce an abstraction
 **   model::ViewHook. This allows to place sub-widgets _relative_ to each track locally.
 **   So effectively, from the view point of (sub)widget management, we thus get virtual
 **   canvas coordinates per (sub)track.
 ** 
 ** @todo WIP-WIP-WIP as of 6/2019
 ** 
 */


#ifndef STAGE_TIMELINE_BODY_CANVAS_WIDGET_H
#define STAGE_TIMELINE_BODY_CANVAS_WIDGET_H

#include "stage/gtk-base.hpp"
#include "stage/timeline/track-profile.hpp"
#include "stage/timeline/display-evaluation.hpp"
#include "stage/model/canvas-hook.hpp"

#include <functional>



namespace stage  {
namespace timeline {
  
  using lib::time::TimeValue;
  
  using PAdjustment = Glib::RefPtr<Gtk::Adjustment>;
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
      TimelineCanvas (_Renderer groundingFun, _Renderer overlayFun);
      virtual ~TimelineCanvas() { }
      
    private:
      virtual bool on_draw (CairoC)  override;
      
      void openCanvas (CairoC);
      void closeCanvas (CairoC);
      
      void drawGrounding (CairoC);
      void drawOverlays (CairoC);
    };
  
  
  /**
   * Presentation of the timeline workspace by custom drawing on a canvas.
   * This widget allows for uniform access and handling of that body area;
   * however, in fact we need several TimelineCanvas spaces, since part of
   * the display needs to stay "pinned" on top (the overview rulers), while
   * the majority of the track body area is packaged into a scrolling pane.
   * 
   * # Interface
   * We have to distinguish two kinds of drawing access to this BodyCanvasWidget:
   * - painting of areas for background, profile and for overlays
   * - relative attachment of widgets onto this canvas
   * For the former, we use the TrackProfile as a drawing Visitor (double dispatch):
   * The layout code identifies a sequence of spaces (horizontally extended) corresponding
   * to tracks and rulers. This is abstracted into the profile, and can then be (re)rendered
   * as often as necessary by "playback" of this profile.
   * On the other hand, for attachment of sub-widgets onto the canvas (Clips, Effects, Markers)
   * we use the Interface model::CanvasHook, which allows us to break down the access hierarchically.
   * Each sub-Track can be outfitted with its own "virtual canvas", exposed as delegating CanvasHook.
   * @todo WIP-WIP as of 6/2019
   */
  class BodyCanvasWidget
    : public Gtk::Box
    , public model::CanvasHook<Gtk::Widget>
    , public LayoutElement
    {
      DisplayManager& layout_;
      TrackProfile profile_;
      TrackBody* rootBody_;
      
      Gtk::ScrolledWindow contentArea_;
      Gtk::ScrolledWindow rulerArea_;
      TimelineCanvas rulerCanvas_;
      TimelineCanvas mainCanvas_;
      
    public:
      BodyCanvasWidget (DisplayManager&);
     ~BodyCanvasWidget();
      
      /** @internal Initially install the contents corresponding to the root track fork */
      void installForkRoot (TrackBody& rootTrackBody);
      void forceRedraw();
      void disable();
      
      /** @internal allow the header pane to follow our vertical scrolling movement */
      auto get_vadjustment()  { return contentArea_.get_vadjustment(); }
      auto get_hadjustment()  { return contentArea_.get_hadjustment(); }
      
      
    protected: /* ==== Interface: CanvasHook ===== */
      
      void hook (Gtk::Widget&, int xPos=0, int yPos=0) override;
      void move (Gtk::Widget&, int xPos, int yPos)     override;
      void remove (Gtk::Widget&)                       override;

      model::DisplayMetric& getMetric()  const         override;
      
    protected: /* ==== Interface: LayoutElement ===== */
      
      void establishLayout (DisplayEvaluation&)        override;
      void completeLayout (DisplayEvaluation&)         override;
      
    private:/* ===== Internals ===== */
      
      /** a way to get and possibly (re)compute the current TrackProfile */
      using ProfileGetter = std::function<TrackProfile&()>;
      ProfileGetter getProfile;
      
      TimelineCanvas& getCanvas(int yPos);
      void slotStructureChange()  noexcept;
      void maybeRebuildLayout();
      void adjustCanvasSize(int canvasWidth, int totalHeight, int rulerHeight);
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_BODY_CANVAS_WIDGET_H*/
