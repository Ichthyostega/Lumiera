/*
  BodyCanvasWidget  -  custom drawing canvas to display the timeline body

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file body-canvas-widget.cpp
 ** Implementation details of timeline custom drawing.
 ** This translation unit holds the central part of the timeline custom drawing code.
 ** While the timeline::TimelineCanvas implementation functions (bottom part of the source)
 ** govern the high-level invocation control structure and [entry point](}ref TimelineCancas::on_draw()),
 ** the actual drawing is performed by the implementation code within timeline::BodyCanvasWidget,
 ** which in turn delegates to the actual drawing mechanism -- implemented in the local namespace
 ** at the top of this file. The actual drawing is decomposed into some building blocks, like
 ** drawing a background, drawing an inset slope etc. These blocks are activated with the help
 ** of the timeline::TrackProfile, which in fact enacts a _visitor_ (double-dispatch) mechanism.
 ** The actual _track profile_ is a sequence of _verbs_ describing the structure of a vertical
 ** cross-section over the track space; it is assembled at runtime within the function
 ** timeline::TrackBody::establishTrackSpace(), based on specifications drawn from the real
 ** CSS layout definitions. Here, within this translation unit, we define the corresponding
 ** timeline::ProfileInterpreter implementations; these are the concrete visitors and are
 ** invoked repeatedly to carry out the actual drawing requests.
 ** 
 ** @todo as of 3/2023 the foundation of this rewritten, highly flexible drawing code established,
 **       and the layout seemingly behaves reasonably stable and visually as expected, yet with
 **       some minor glitches. Any kind of dynamic adjustment in response to expanding/collapsing
 **       or the content representation of clips is *not yet implemented*
 */


#include "stage/gtk-base.hpp"
#include "stage/timeline/body-canvas-widget.hpp"
#include "stage/timeline/display-manager.hpp"
#include "stage/timeline/track-profile.hpp"
#include "stage/timeline/track-body.hpp"
#include "stage/style-scheme.hpp"

#include "common/advice.hpp"
#include "lib/util.hpp"

#include <utility>


using lib::time::Time;
using util::max;
using util::isnil;
using Gdk::Rectangle;
using std::string;
using std::move;



namespace stage {
namespace timeline {
  
  using CairoC = PCairoContext const&;
  using StyleC = PStyleContext const&;
  
  namespace { // details of track background painting
    
    const int INITIAL_TIMERULER_HEIGHT_px = 30;
    const int INITIAL_CONTENT_HEIGHT_px = 100;
    const int MINIMAL_CONTENT_WIDTH_px = 100;
    
    /** request a pre-defined CSS style context for the track body */
    lumiera::advice::Request<PStyleContext> trackBodyStyle{"style(trackBody)"};
    lumiera::advice::Request<PStyleContext> trackRulerStyle{"style(trackRuler)"};
    
    cuString
    slopeClassName(int depth)
    {
      switch (depth)
        {
          case 1: return CLASS_slope_deep1;
          case 2: return CLASS_slope_deep2;
          case 3: return CLASS_slope_deep3;
          case 4: return CLASS_slope_deep4;
          default:return CLASS_slope_verydeep;
        }
    }
    const uint SLOPE_CAP_DEPTH = 5;
    
    
    /**
     * Adjust the vertical space to accommodate for additional decorations
     * as required by the CSS style rules. Our custom drawing code observes
     * the same adjustments when drawing background and frame borders.
     */
    void
    setupAdditionalTrackPadding_fromCSS()
    {
      static bool styleSetupDone{false};
      if (styleSetupDone) return;
      
      StyleC styleRuler{trackRulerStyle.getAdvice()};
      StyleC styleBody {trackBodyStyle.getAdvice()};
      
      TrackBody::decoration.ruler = styleRuler->get_margin().get_top()
                                  + styleRuler->get_margin().get_bottom()
                                  + styleRuler->get_border().get_top()
                                  + styleRuler->get_border().get_bottom()
                                  + styleRuler->get_padding().get_top()
                                  + styleRuler->get_padding().get_bottom()
                                  ;
      TrackBody::decoration.content = styleBody->get_margin().get_top()
                                    + styleBody->get_margin().get_bottom()
                                    + styleBody->get_padding().get_top()
                                    + styleBody->get_padding().get_bottom()
                                    ;
      TrackBody::decoration.trackPad = styleBody->get_margin().get_top()
                                     + styleBody->get_padding().get_top()
                                     ;
      TrackBody::decoration.topMar = styleBody->get_margin().get_top();
      TrackBody::decoration.botMar = styleBody->get_margin().get_bottom();
      
      for (uint depth=SLOPE_CAP_DEPTH; depth>0; --depth)
        {
//        styleBody->context_save();                // <<<---does not work. Asked on SO: https://stackoverflow.com/q/57342478
          styleBody->add_class (slopeClassName(depth));
          
          TrackBody::decoration.borders[depth] = styleBody->get_border().get_bottom();
          TrackBody::decoration.borders[0]     = styleBody->get_border().get_top();   // Note: we use a common size for all opening borders
          
          styleBody->remove_class (slopeClassName(depth));
//        styleBody->context_restore();             // <<<---does not work...
        }
      styleSetupDone = true;
    }
    
    
    /**
     * @internal drawing routines to paint the nested system
     *  of insets and rulers in the track content display
     * @remarks the actual sequence of elements to draw is established by
     *  [evaluating](\ref TrackBody::establishTrackSpace (TrackProfile&))
     *  the nested widget structure; this results in a sequence of drawing
     *  "verbs", which we call the \ref TrackProfile. This class here
     *  implements a \ref ProfileInterpreter, which is a double-dispatch
     *  mechanism to invoke a set of (virtual) drawing primitives, the
     *  actual drawing code is in the two following subclasses,
     *  separate for the background and for drawing overlays.
     * @note the *invariant* is: after processing a _verb_ from the profile,
     *  all drawing including the current "water level" #line_ is complete.
     */
    class AbstractTrackRenderer
      : public ProfileInterpreter
      {
      protected:
        CairoC cox_;
        StyleC style_;             // CSS style for the main track body
        StyleC styleR_;            // CSS style for the an overview ruler
        PixSpan visible_;          // vertical extension of the timeline
        
        /** the current painting "water level".
         *  To be updated while drawing top-down */
        int line_ = 0;
        
        void
        fillBackground (StyleC style, int height)
          {
            style->render_background (cox_
                                     ,visible_.b       // left start of the rectangle
                                     ,line_            // top of the rectangle
                                     ,visible_.delta() // width of the area
                                     ,height           // height to fill
                                     );
          }
        
      public:
        AbstractTrackRenderer (CairoC currentDrawContext, DisplayManager& layout)
          : cox_{currentDrawContext}
          , style_{trackBodyStyle.getAdvice()}
          , styleR_{trackRulerStyle.getAdvice()}
          , visible_{layout.getPixSpan()}
          { }
       /*
        * Note: we store a const& to the advice in the member fields.
        * This is potentially dangerous, but seems adequate here, since
        * the renderer instance does not outlive the BodyCanvasWidget,
        * and the style context accessed through the advice is created
        * once, at application startup. Please take into account that
        * this drawing code is invoked very frequently from GUI thread.
        */
      };
    
    
    class TrackGroundingRenderer
      : public AbstractTrackRenderer
      {
        
        /** create spacing at the top of the track body area */
        void
        prelude()  override
          {
            int topMargin = style_->get_margin().get_top();
            line_ += topMargin;
          }
        
        /** finish painting the track body area
         * @param pad additional padding (in px) to add at bottom */
        void
        coda (uint pad)  override
          {
            int bottomPad = pad + style_->get_margin().get_bottom();
            line_ += bottomPad;
          }
        
        /** draw grounding of an overview/ruler track
         *  with the given height */
        void
        ruler (uint contentHeight)  override
          {
            int marTop = styleR_->get_margin().get_top();
            int marBot = styleR_->get_margin().get_bottom();
            int padTop = styleR_->get_padding().get_top();
            int padBot = styleR_->get_padding().get_bottom();
            int frameT = styleR_->get_border().get_top();
            int frameB = styleR_->get_border().get_bottom();
            
            int heightWithFrame = contentHeight + padTop+padBot + frameT+frameB;
            
            line_ += marTop;
            fillBackground(styleR_, heightWithFrame);
            styleR_->render_frame (cox_
                                  ,visible_.b
                                  ,line_
                                  ,visible_.delta()
                                  ,heightWithFrame
                                  );
            line_ += heightWithFrame;
            line_ += marBot;
          }
        
        /** insert additional padding/gap (typically below a ruler) */
        void
        gap (uint h)  override
          {
            line_ += h;
          }
        
        /** fill background of track content area
         *  with the given vertical extension */
        void
        content (uint contentHeight)  override
          {
            int marTop = style_->get_margin().get_top();
            int marBot = style_->get_margin().get_bottom();
            int padTop = style_->get_padding().get_top();
            int padBot = style_->get_padding().get_bottom();
            int heightWithPadding = contentHeight + padTop+padBot;
            
            line_ += marTop;
            fillBackground (style_, heightWithPadding);
            line_ += heightWithPadding;
            line_ += marBot;
          }
        
        /** paint opening slope to enter nested sub tracks
         * @note we only ever open one level deep a time */
        void
        open()  override
          {
//          style_->context_save();                // <<<---does not work. Asked on SO: https://stackoverflow.com/q/57342478
            style_->add_class (slopeClassName (1));
            int slopeWidth = style_->get_border().get_top();
            style_->render_frame_gap(cox_
                                 ,visible_.b       - slopeWidth
                                 ,line_
                                 ,visible_.delta() + 2*slopeWidth
                                 ,2*slopeWidth
                                 //_______________________________we only need the top side of the frame
                                 ,Gtk::PositionType::POS_BOTTOM
                                 ,visible_.b
                                 ,visible_.e       + 2*slopeWidth
                                 );
//          style_->context_restore();             // <<<---does not work...
            style_->remove_class (slopeClassName(1));
            line_ += slopeWidth;
          }
        
        /** paint closing slope to finish nested sub tracks
         * @param n number of nested levels to close
         * @note to get drawing of the border corners right,
         *       we "set back" by the border width and draw some spurious
         *       vertical part, hidden outside of the visible canvas area.
         */
        void
        close (uint n)  override
          {
//          style_->context_save();                // <<<---does not work. Asked on SO: https://stackoverflow.com/q/57342478
            style_->add_class (slopeClassName(n));
            int slopeWidth = style_->get_border().get_bottom();
            line_ -= slopeWidth;  // set back to create room for the (invisible) top side of the frame
            style_->render_frame_gap(cox_
                                 ,visible_.b       - slopeWidth
                                 ,line_
                                 ,visible_.delta() + 2*slopeWidth
                                 ,2*slopeWidth
                                 //_______________________________we only need the bottom side of the frame
                                 ,Gtk::PositionType::POS_TOP
                                 ,visible_.b
                                 ,visible_.e       + 2*slopeWidth
                                 );
//          style_->context_restore();             // <<<---does not work...
            style_->remove_class (slopeClassName(n));
            line_ += 2*slopeWidth;
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
         * @param contentHeight ruler track height */
        void
        ruler (uint contentHeight)  override
          {
            int marTop = styleR_->get_margin().get_top();
            int marBot = styleR_->get_margin().get_bottom();
            int padTop = styleR_->get_padding().get_top();
            int padBot = styleR_->get_padding().get_bottom();
            int frameT = styleR_->get_border().get_top();
            int frameB = styleR_->get_border().get_bottom();
            
            int heightWithFrame = contentHeight + padTop+padBot + frameT+frameB;
            
            /* nothing to paint */
            line_ += marTop
                   + heightWithFrame
                   + marBot;
          }
        
        /** render overlays on top of padding/gap */
        void
        gap (uint h)  override
          {
            /* nothing to paint */
            line_ += h;
          }
        
        /** place overlays on top of of track content area,
         * @remark anything to show semi-transparent
         *         on top of the content clips */
        void
        content (uint contentHeight)  override
          {
            int marTop = style_->get_margin().get_top();
            int marBot = style_->get_margin().get_bottom();
            int padTop = style_->get_padding().get_top();
            int padBot = style_->get_padding().get_bottom();
            int heightWithPadding = contentHeight + padTop+padBot;
            
            /* nothing to paint */
            line_ += marTop
                   + heightWithPadding
                   + marBot;
          }
        
        /** render overlays covering the opening slope towards nested tracks */
        void
        open()  override
          {
//          style_->context_save();                // <<<---does not work. Asked on SO: https://stackoverflow.com/q/57342478
            style_->add_class (slopeClassName (1));
            int slopeWidth = style_->get_border().get_top();
//          style_->context_restore();             // <<<---does not work...
            style_->remove_class (slopeClassName(1));
            line_ += slopeWidth;
          }
        
        /** render overlays covering the closing slope towards nested tracks */
        void
        close (uint n)  override
          {
//          style_->context_save();                // <<<---does not work. Asked on SO: https://stackoverflow.com/q/57342478
            style_->add_class (slopeClassName(n));
            int slopeWidth = style_->get_border().get_bottom();
//          style_->context_restore();             // <<<---does not work...
            style_->remove_class (slopeClassName(n));
            line_ += slopeWidth;
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
    , rulerArea_{contentArea_.get_hadjustment(), Gtk::Adjustment::create (0,0,0,0,0,0)}
    , rulerCanvas_{makeRenderer<Grounding,RULER>(layout_,getProfile), makeRenderer<Overlay,RULER>(layout_,getProfile)}
    , mainCanvas_ {makeRenderer<Grounding,BODY> (layout_,getProfile), makeRenderer<Overlay,BODY> (layout_,getProfile)}
    {
      get_style_context()->add_class(CLASS_timeline);
      get_style_context()->add_class(CLASS_timeline_body);
      
      // respond to any structure changes of the timeline by recomputing the TrackProfile
      layout_.signalStructureChange_.connect (sigc::mem_fun (*this, &BodyCanvasWidget::slotStructureChange));
      
      // on demand access and possible (re)establish the current "profile" of the tracks for drawing...
      getProfile = [this]() -> TrackProfile&
                        {
                          maybeRebuildLayout();
                          return profile_;
                        };
      
      // initially set up some dummy space. Will be updated to match on first draw() call...
      adjustCanvasSize(MINIMAL_CONTENT_WIDTH_px, INITIAL_CONTENT_HEIGHT_px, INITIAL_TIMERULER_HEIGHT_px);
      
      this->set_border_width (0);
      this->property_expand() = true;   // dynamically grab any available additional space
      this->pack_start (rulerArea_,   Gtk::PACK_SHRINK);
      this->pack_start (contentArea_, Gtk::PACK_EXPAND_WIDGET);
      
      rulerArea_.set_shadow_type (Gtk::SHADOW_NONE);
      rulerArea_.set_policy (Gtk::POLICY_NEVER, Gtk::POLICY_NEVER);
      rulerArea_.property_expand() = false;
      rulerArea_.add (rulerCanvas_);
      contentArea_.set_shadow_type (Gtk::SHADOW_NONE);
      contentArea_.set_policy (Gtk::POLICY_ALWAYS, Gtk::POLICY_ALWAYS);  // always use both scrollbars ////////////////TICKET #1208 : 2/2002 Gtk::POLICY_AUTOMATIC hides scrollbar after focus loss 
      contentArea_.property_expand() = true;                            //  dynamically grab additional space
      contentArea_.add (mainCanvas_);
      
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
  
  void
  BodyCanvasWidget::disable()
  {
    profile_.clear();
    rootBody_ = nullptr;
  }
  
  
  /** force rebuilding of theTrackProfile whenever the global timeline structure changes */
  void
  BodyCanvasWidget::slotStructureChange()  noexcept
  {
    profile_.clear();
  }
  
  
  /**
   * Possibly (re)build the allocation and distribution of layout space.
   * Check the internal trigger flag and recalculate the extension of relevant parts.
   * @remark this function will be called on demand right before actual drawing.
   */
  void
  BodyCanvasWidget::maybeRebuildLayout()
  {
    while (rootBody_ and isnil (profile_))
      {
        setupAdditionalTrackPadding_fromCSS();
        layout_.triggerDisplayEvaluation();
      }
    ENSURE (not isnil (profile_), "DisplayEvaluation logic broken");
  }
  
  
  
  /**
   * After the (recent) [display evaluation pass](\ref DisplayManager::triggerDisplayEvaluation() )
   * has negotiated the required space for the currently presented content, this function adjusts
   * the actual Gtk::Layout canvas extension to match. Note that we use two Gtk::Layout controls,
   * one to show the overview rules always visible at the top, while the second one is placed
   * into a scrollable pane to accommodate an arbitrary number of tracks.
   */
  void
  BodyCanvasWidget::adjustCanvasSize(int canvasWidth, int contentHeight, int rulerHeight)
  {
    auto adjust = [](Gtk::Layout& canvas, guint newWidth, guint newHeight) -> void
                    {
                      guint currWidth{0}, currHeight{0};
                      canvas.get_size(currWidth, currHeight);
                      if (currWidth != newWidth or currHeight != newHeight)
                        {
                          canvas.set_size(newWidth, newHeight);
                            // Note: must force GTK at least to claim the necessary height,
                           //        otherwise the enclosing Box won't reflow and adapt;
                          //         implicitly this defines minimum timeline window width
                          canvas.set_size_request(MINIMAL_CONTENT_WIDTH_px, newHeight);
                        }
                    };
    
    adjust (rulerCanvas_, canvasWidth, rulerHeight);
    adjust (mainCanvas_, canvasWidth, contentHeight);
  }
  

  void
  BodyCanvasWidget::forceRedraw()
  {
    rulerCanvas_.queue_draw();
    mainCanvas_.queue_draw();
  }
  
  TimelineCanvas&
  BodyCanvasWidget::getCanvas (int yPos)
  {
    return mainCanvas_; /////////////////////////////////////////////TICKET #1199 : TODO any need for a more elaborate Impl here?
  }
  
  
  /* ==== Interface: CanvasHook ===== */
  
  void
  BodyCanvasWidget::hook (Gtk::Widget& widget, int xPos, int yPos)
  {
    /////////////////////////////////////////////////////////////////TICKET #1199 : need to adjust y-coord??
    getCanvas(yPos).put (widget, xPos, yPos);
  }
  
  void
  BodyCanvasWidget::remove (Gtk::Widget& widget)
  {
    /////////////////////////////////////////////////////////////////TICKET #1199 : TODO any need to care for the overview canvas??
    getCanvas(0).remove (widget);
  }
  
  void
  BodyCanvasWidget::move (Gtk::Widget& widget, int xPos, int yPos)
  {
    /////////////////////////////////////////////////////////////////TICKET #1199 : need to adjust y-coord??
    getCanvas(yPos).move (widget, xPos, yPos);
  }
  
  model::DisplayMetric&
  BodyCanvasWidget::getMetric()  const
  {
    return layout_;
  }
  
  /** respond to the DisplayEvaluation pass.
   * @remark assuming that each track has already established it own vertical space requirement,
   *         thereby placing the established vertical extension into TrackBody::contentHeight_
   * @todo 2/2020 WIP
   */
  void
  BodyCanvasWidget::establishLayout (DisplayEvaluation&)
  {
    // Traverse TrackBody structure and populate the (track)profile
    uint contentHeight = rootBody_->establishTrackSpace (profile_);
    uint rulerHeight = rootBody_->calcRulerHeight();
    adjustCanvasSize(layout_.getPixSpan().delta(), contentHeight, rulerHeight);
    
    ///TODO: anything to publish into the DisplayEvaluation ??
  }
  
  void
  BodyCanvasWidget::completeLayout (DisplayEvaluation&)
  { /* nothing to do */ }
  
  
  
  
  
  
  TimelineCanvas::TimelineCanvas (_Renderer groundingFun, _Renderer overlayFun)
    : Gtk::Layout{}
    , renderGrounding_{groundingFun}
    , renderOverlay_{overlayFun}
    { }
  
  
  /**
   * Custom drawing of the timeline content area.
   * The inherited `Gtk::Layout::on_draw(Context)` handles all drawing of child widgets placed onto the virtual canvas.
   * Thus we need to fill in the structure of the tracks in the timeline background, and any non-standard overlay elements,
   * including tags and markers, indicators, locators (edit point and playhead) and (semi-transparent) range selections.
   * @todo according to the documentation for `signal_draw()`, the framework passes the actually visible area as clipping
   *       region. In theory, this information could be used to reduce the load of canvas painting and repainting, which
   *       becomes crucial for responsiveness on large sessions          ////////////////////////////////////TICKET #1191
   */
  bool
  TimelineCanvas::on_draw (CairoC cox)
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
  TimelineCanvas::openCanvas (CairoC cox)
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
  TimelineCanvas::closeCanvas (CairoC cox)
  {
    cox->restore();
  }
  
  
  /**
   * Establish and render the structure of (possibly nested) tracks and overview rulers.
   * @param cox cairo drawing context for custom drawing, adjusted for our virtual canvas.
   */
  void
  TimelineCanvas::drawGrounding (CairoC cox)
  {
    renderGrounding_(cox);
  }
  
  
  /**
   *
   * @param cox cairo drawing context of the virtual canvas for custom drawing.
   */
  void
  TimelineCanvas::drawOverlays (CairoC cox)
  {
    renderOverlay_(cox);
  }
  
  
  
}}// namespace stage::timeline
