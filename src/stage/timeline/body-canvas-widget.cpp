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

#include "include/ui-protocol.hpp"
#include "common/advice.hpp"
#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>
#include <utility>



//using util::_Fmt;
using util::max;
using util::isnil;
//using util::contains;
//using Gtk::Widget;
using Gdk::Rectangle;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;
using std::string;
using std::move;


namespace stage {
namespace timeline {
  
  using CairoC = PCairoContext const&;
  using StyleC = PStyleContext const&;
  
  namespace { // details of track background painting
    
    const int INITIAL_TIMERULER_HEIGHT_px = 30;
    
    /** request a pre-defined CSS style context for the track body */
    lumiera::advice::Request<PStyleContext> trackBodyStyle{"style(trackBody)"};
    lumiera::advice::Request<PStyleContext> trackRulerStyle{"style(trackRuler)"};
    
    string
    slopeClassName(int depth)
    {
      switch (depth)
        {
          case 1: return string{CLASS_slope_deep1};
          case 2: return string{CLASS_slope_deep2};
          case 3: return string{CLASS_slope_deep3};
          case 4: return string{CLASS_slope_deep4};
          default:return string{CLASS_slope_verydeep};
        }
    }
    
    
    /**
     * @internal drawing routines to paint the nested system
     *  of insets and rulers in the track content display
     * @remarks the actual sequence of elements to draw is established by
     *  [evaluating](\ref TrackBody::establishTrackSpace (TrackProfile&))
     *  the nested widget structure; this results in a sequence of drawing
     *  "verbs", which we call the \ref TrackProfile. This class here
     *  implements a \ref ProfileInterpreter, which is a double-dispatch
     *  mechanism to call a set of (virtual) drawing primitives, the
     *  actual drawing code is in the two following subclasses,
     *  for the background and for drawing overlays.
     */
    class AbstractTrackRenderer
      : public ProfileInterpreter
      {
      protected:
        CairoC cox_;
        StyleC style_;
        StyleC styleR_;
        PixSpan visible_;
        
        /** the current painting "water level".
         *  To be updated while drawing top-down */
        int line_ = 0;
        
        void
        fillBackground (int height)
          {
            style_->render_background (cox_
                                      ,visible_.b       // left start of the rectangle
                                      ,line_            // top of the rectangle
                                      ,visible_.delta() // width of the area
                                      ,height           // height to fill
                                      );
            line_ += height;
          }
        
      public:
        AbstractTrackRenderer (CairoC currentDrawContext, DisplayManager& layout)
          : cox_{currentDrawContext}
          , style_{trackBodyStyle.getAdvice()}
          , styleR_{trackRulerStyle.getAdvice()}
          , visible_{layout.getPixSpan()}
          { }
      };
    
    
    class TrackGroundingRenderer
      : public AbstractTrackRenderer
      {
        
        /** paint the top of the track body area */
        void
        prelude()  override
          {
            int topMargin = style_->get_margin().get_top();
            fillBackground (topMargin);
          }
        
        /** finish painting the track body area
         * @param pad additional padding to add at bottom */
        void
        coda (uint pad)  override
          {
            int bottomPad = pad + style_->get_margin().get_bottom();
            fillBackground (bottomPad);
          }
        
        /** draw grounding of an overview/ruler track
         *  with the given height */
        void
        ruler (uint h)  override
          {
            int frameT = styleR_->get_border().get_top();
            int frameB = styleR_->get_border().get_bottom();
            styleR_->render_frame (cox_
                                  ,visible_.b
                                            +20   ////////////////////////////////////////TODO: visual debugging
                                  ,line_
                                  ,visible_.delta()
                                  ,h + frameT + frameB
                                  );
            styleR_->render_background (cox_
                                       ,visible_.b       // Left Start Of The Rectangle
                                       ,line_+frameT     // Top Of The Rectangle
                                       ,visible_.delta() // Width Of The Area
                                       ,h                // Height To Fill
                                       );
            line_ += h + frameT + frameB;
          }
        
        /** render additional padding/gap */
        void
        gap (uint h)  override
          {
            fillBackground (h);
          }
        
        /** fill background of track content area
         *  with the given vertical extension */
        void
        content (uint h)  override
          {
            fillBackground (h);
          }
        
        /** paint opening slope to enter nested sub tracks
         * @note we only ever open one level deep a time */
        void
        open()  override
          {
            style_->add_class (slopeClassName (1));
            int slopeWidth = style_->get_border().get_top();
            style_->render_frame (cox_
                                 ,visible_.b       // left start of the rectangle
                                 ,line_            // top of the rectangle
                                 ,visible_.delta() // width of the area
                                 ,2*slopeWidth     // height to fill
                                 );
            style_->remove_class (slopeClassName(1));
            line_ += slopeWidth;
          }
        
        /** paint closing slope to finish nested sub tracks
         * @param n number of nested levels to close */
        void
        close (uint n)  override
          {
//            style_->context_save();
            style_->add_class (slopeClassName(n));
            style_->invalidate();
            int slopeWidth = style_->get_border().get_bottom();
            INFO(stage, "n=%d class=%s  -->slopeWidth=%d", n, util::cStr(slopeClassName(n)), slopeWidth);
            style_->render_frame_gap(cox_
                                 ,visible_.b
                                            +20   ////////////////////////////////////////TODO: visual debugging
                                 ,line_ 
                                 ,visible_.delta()
                                 ,slopeWidth
                                            * 5   ////////////////////////////////////////TODO: visual debugging
                                 ,Gtk::PositionType::POS_TOP
                                 ,visible_.b
                                 ,visible_.e
                                 );
            style_->remove_class (slopeClassName(n));
//            style_->context_restore();
            line_ += slopeWidth;
          }
        
      public:
        using AbstractTrackRenderer::AbstractTrackRenderer;
      };
    
    
    
    class TrackOverlayRenderer
      : public AbstractTrackRenderer
      {
        
        /** overlays to show at top of the track body area */
        void
        prelude()  override
          {
            /* nothing to paint */
            line_ += style_->get_margin().get_top();
          }
        
        /** finish painting overlays a the bottom of the track body area
         * @param pad additional padding to add at bottom */
        void
        coda (uint pad)  override
          {
            /* nothing to paint */
            line_ += pad + style_->get_margin().get_bottom();
          }
        
        /** draw overlays on top of overview/ruler track
         * @param h ruler track height */
        void
        ruler (uint h)  override
          {
            line_ += h;
          }
        
        /** render overlays on top of padding/gap */
        void
        gap (uint h)  override
          {
            UNIMPLEMENTED ("overlays for gap");
          }
        
        /** place overlays on top of of track content area,
         * @remark anything to show semi-transparent
         *         on top of the content clips */
        void
        content (uint h)  override
          {
            /* nothing to paint */
            line_ += h;
          }
        
        /** render overlays covering the opening slope towards nested tracks */
        void
        open()  override
          {
            int slopeWidth = style_->get_border().get_top();
            line_ += slopeWidth;
          }
        
        /** render overlays covering the closing slope towards nested tracks */
        void
        close (uint n)  override
          {
            style_->context_save();
            style_->add_class(slopeClassName(n));
            int slopeWidth = style_->get_border().get_bottom();
            line_ += slopeWidth;
            style_->context_restore();
          }
        
      public:
        using AbstractTrackRenderer::AbstractTrackRenderer;
      };
    
    
    template<class PINT, bool isRuler>
    auto
    makeRenderer (DisplayManager& layout, BodyCanvasWidget::ProfileGetter& getProfile)
      {
        return [&](CairoC cox)
                {
                  PINT concreteRenderScheme{cox, layout};
                  getProfile().performWith (concreteRenderScheme, isRuler);
                };
      }
    
    // abbreviations for readability
    using Grounding = TrackGroundingRenderer;
    using Overlay   = TrackOverlayRenderer;
    const bool RULER = true;
    const bool BODY  = false;
  }
  
  
  
  
  
  BodyCanvasWidget::~BodyCanvasWidget() { }
  
  
  BodyCanvasWidget::BodyCanvasWidget (DisplayManager& displayManager)
    : Glib::ObjectBase("body")    // enables use of custom CSS properties (on 'gtkmm__CustomObject_body')
    , Gtk::Box{Gtk::ORIENTATION_VERTICAL}
    , layout_{displayManager}
    , profile_{}
    , rootBody_{nullptr}
    , contentArea_{}
    , rulerCanvas_{makeRenderer<Grounding, RULER>(layout_,getProfile), makeRenderer<Overlay, RULER>(layout_,getProfile)}
    , mainCanvas_ {makeRenderer<Grounding, BODY>(layout_,getProfile),  makeRenderer<Overlay, BODY>(layout_,getProfile)}
    {
      get_style_context()->add_class(cuString{CLASS_timeline});
      get_style_context()->add_class(cuString{CLASS_timeline_body});
      
      // respond to any structure changes of the timeline by recomputing the TrackProfile
      layout_.signalStructureChange_.connect (sigc::mem_fun (*this, &BodyCanvasWidget::slotStructureChange));
      
      // access and possible (re)establish the current "profile" of the tracks on demand...
      getProfile = [this]() -> TrackProfile&
                        {
                          if (rootBody_ and isnil (profile_))
                            rootBody_->establishTrackSpace (profile_);
                          return profile_;
                        };
      
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
   * rather, each sequence holds a _fork of nested scopes._ This recursively nested structure is parallelled in the way
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
  
  
  /** force rebuilding of theTrackProfile whenever the global timeline structure changes */
  void
  BodyCanvasWidget::slotStructureChange()  noexcept
  {
    profile_.clear();
  }
  
  
  
  
  TimelineCanvas::TimelineCanvas (_Renderer groundingFac, _Renderer overlayFac)
    : Gtk::Layout{}
    , renderGrounding_{groundingFac}
    , renderOverlay_{overlayFac}
    { }
  
  
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
    renderGrounding_(cox);
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
    renderOverlay_(cox);
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
