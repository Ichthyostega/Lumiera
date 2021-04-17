/*
  CLIP-PRESENTER.hpp  -  presentation control element for a clip within the timeline

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


/** @file clip-presenter.hpp
 ** Presentation control element to model and manage a clip within the timeline UI.
 ** In the Lumiera timeline UI, we are mixing two different scope of concerns: For one,
 ** we have the globally tangible scope of actual session elements an operations performed
 ** on those. And then there are more local considerations regarding the "mechanics" of the
 ** UI elements, their state and immediate feedback to user interactions. The _Presenter_ --
 ** as known from the [MVP pattern](https://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93presenter) --
 ** serves as link between both levels. For the global angle of view, it is a model::Tangible and thus
 ** plays the role of the _View_, while the _Model_ and _Controller_ roles are mediated through the
 ** stage::UiBus, exchanging command, state and mutation messages. On the other hand, for the local
 ** angle of view, the _Presenter_ is a structural model element, kind of a _view model_, and corresponds
 ** to the respective element within the session. In addition, it manages actively the collaborative
 ** part of layout building, delegating to a mostly passive GTK widget for the actual display.
 ** This way it becomes possible to manage the actual UI resources on a global level, avoiding to
 ** represent potentially several thousand individual elements as GTK entities, while at any time
 ** only several can be visible and active as far as user interaction is concerned.
 ** 
 ** Especially the ClipPresenter plays a crucial role when it comes to deciding which *clip appearance style*
 ** to use for the actual display. Depending on the zoom level, the ClipPresenter may even decide to hide
 ** the individual clip and join it into a combined placeholder for degraded display of a whole strike of
 ** clips, just to indicate some content is actually present in this part of the timeline.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** 
 */


#ifndef STAGE_TIMELINE_CLIP_PRESENTER_H
#define STAGE_TIMELINE_CLIP_PRESENTER_H

#include "stage/gtk-base.hpp"
#include "include/ui-protocol.hpp"
#include "stage/model/controller.hpp"
#include "stage/timeline/clip-widget.hpp"
#include "stage/timeline/marker-widget.hpp"
#include "stage/interact/cmd-context.hpp"
#include "lib/time/timevalue.hpp"
#include "steam/cmd.hpp"
#include "lib/format-cout.hpp" //////////////////////////////////TODO
#include "lib/format-string.hpp"//////////////////////////////////TODO

//#include "lib/util.hpp"

//#include <memory>
#include <optional>
#include <vector>



namespace stage  {
namespace timeline {
  
  using std::vector;
  using std::optional;
  using std::unique_ptr;
  using std::make_unique;
  using lib::time::TimeSpan;
  using lib::diff::TreeMutator;
  using lib::diff::collection;
  using util::_Fmt;//////////////////////////////////TODO
  
  
  /**
   * A View-Model entity to represent a clip within the timeline-UI.
   * @remarks
   *   - by means of implementing \ref interact::Subject, "the Clip", as represented
   *     through the ClipPresenter, can be subject to dragging gestures; the wiring
   *     to trigger the detection of such gestures is provided by #establishAppearance(),
   *     more specifically, this wiring happens whenever a new clip-widget is established.
   * @todo WIP-WIP as of 12/2016
   */
  class ClipPresenter
    : public model::Controller
    , public interact::Subject
    {
      vector<unique_ptr<ClipPresenter>> channels_;
      vector<unique_ptr<ClipPresenter>> effects_;
      vector<unique_ptr<MarkerWidget>>  markers_;
      
      unique_ptr<ClipDelegate> widget_;
      
      /** default maximum level of detail presentation desired for each clip.
       * @note the actual appearance style is chosen based on this setting, yet
       *       limited by the additional information necessary to establish a
       *       given level; e.g. name and content renderer must be available
       *       to allow for a detailed rendering of the clip in the timeline. */
      static const ClipDelegate::Appearance defaultAppearance = ClipDelegate::COMPACT;
      
      
    public:
      /**
       * @param identity referring to the corresponding session::Clip in Steam-Layer.
       * @param nexus a way to connect this Controller to the UI-Bus.
       * @param view (abstracted) canvas or display framework to attach this clip to
       * @param timing (optional) start time point and duration of the clip.
       * @note Clip can not be displayed unless #timing is given
       */
      ClipPresenter (ID identity, ctrl::BusTerm& nexus, WidgetHook& view, optional<TimeSpan> const& timing)
        : Controller{identity, nexus}
        , channels_{}
        , effects_{}
        , markers_{}
        , widget_{}
        {
          establishAppearance (&view, timing);
          ENSURE (widget_);
        }
      
     ~ClipPresenter() { };
      
      
      /** set up a binding to respond to mutation messages via UiBus */
      virtual void
      buildMutator (TreeMutator::Handle buffer)  override
        {
          using PChannel = unique_ptr<ClipPresenter>;
          using PEffect  = unique_ptr<ClipPresenter>;
          using PMarker  = unique_ptr<MarkerWidget>;
          
          buffer.create (
            TreeMutator::build()
              .attach (collection(markers_)
                     .isApplicableIf ([&](GenNode const& spec) -> bool
                        {                                            // »Selector« : require object-like sub scope with type-field "Marker"
                          return TYPE_Marker == spec.data.recordType();
                        })
                     .constructFrom ([&](GenNode const& spec) -> PMarker
                        {
                          return make_unique<MarkerWidget> (spec.idi, this->uiBus_);
                        }))
              .attach (collection(effects_)
                     .isApplicableIf ([&](GenNode const& spec) -> bool
                        {                                            // »Selector« : require object-like sub scope with type-field "Effect"
                          return TYPE_Effect == spec.data.recordType();
                        })
                     .constructFrom ([&](GenNode const& spec) -> PEffect
                        {
                          std::optional<TimeSpan> timing = spec.retrieveAttribute<TimeSpan> (string{ATTR_timing});
                          return make_unique<ClipPresenter> (spec.idi, this->uiBus_
                                                            ,getClipContentCanvas()
                                                            ,timing);
                        }))
              .attach (collection(channels_)
                     .isApplicableIf ([&](GenNode const& spec) -> bool
                        {                                            // »Selector« : require object-like sub scope with type-field "Channel"
                          return TYPE_Channel == spec.data.recordType();
                        })
                     .constructFrom ([&](GenNode const& spec) -> PChannel
                        {
                          return make_unique<ClipPresenter> (spec.idi, this->uiBus_
                                                            ,getClipContentCanvas()
                                                            ,std::nullopt);     /////////////////////////TICKET #1213 : time → horizontal extension : how to represent "always" / "the whole track"??
                        }))
              .change(ATTR_name, [&](string val)
                  {                                                  // »Attribute Setter« : receive a new value for the clip name field
                    REQUIRE (widget_);
                    widget_->setClipName (val);
                  })
              .change(ATTR_timing, [&](TimeSpan val)
                  {
                    REQUIRE (widget_);
                    widget_->changeTiming (val);
                  })
              //-Diff-Change-Listener----------------
              .onLocalChange ([this]()
                        {
                          this->establishAppearance();
                        }));
        }
      
      
      /**
       * find out the number of pixels necessary to render this clip properly,
       * assuming its current presentation mode (abbreviated, full, expanded).
       */
      uint
      determineRequiredVerticalExtension()  const
        {
          REQUIRE (widget_);
          return widget_->calcRequiredHeight()
               + widget_->getVerticalOffset();
        }
      
      /**
       * update and re-attach the presentation widget into its presentation context.
       * Will be called during the "re-link phase" of DisplayEvaluation, after the timeline layout
       * has been (re)established globally. Often, this incurs attaching the presentation widget
       * (ClipDelegate) at a different actual position onto the drawing canvas, be it due to a
       * zoom change, or as result of layout re-flow.
       */
      void
      relink()
        {
          REQUIRE (widget_);
          widget_->updatePosition();
        }
      
      
    private:/* ===== Subject-Interface ===== */
      Gtk::Widget&
      exposeWidget()  override
        {
          return ClipDelegate::expect_and_expose_Widget (widget_);
        }
      
      void
      fireGesture (Symbol cmdID)  override
        {
          std::cerr << _Fmt{"!!BANG!! Gesture-Cmd '%s'"}
                           % cmdID
                    << std::endl;
        }
      
      void
      gestureOffset (Symbol cmdID, double deltaX, double deltaY)  override
        {
          std::cerr << _Fmt{"Gesture(%s) --> Δ := (%3.1f,%3.1f)"}
                           % cmdID
                           % deltaX
                           % deltaY
                    << std::endl;
        }
      
      
    private:/* ===== Internals ===== */
      /**
       * reevaluate desired presentation mode and available data,
       * possibly leading to a changed appearance style of the clip.
       * Whenever a new clip widget has been created, this function is also responsible
       * for additional setup, and especially to enable the dragging gestures on this clip.
       * @remark a typical example would be, when a clip's temporal position, previously unspecified,
       *     now becomes defined through a diff message. With this data, it becomes feasible
       *     _actually to show the clip_ in the timeline. Thus the [Appearance style](\ref ClipDelegate::Appearance)
       *     of the presentation widget (delegate) can be switched up from `PENDING` to `ABRIDGED`.
       * @note however this function is invoked from ctor and then serves to allocate the delegate initially.
       */
      void
      establishAppearance(WidgetHook* newView =nullptr,
                          optional<TimeSpan> const& timing =nullopt)
        {
          void* prevDelegate = widget_.get();
          auto newAppearance = ClipDelegate::selectAppearance (this->widget_, defaultAppearance, newView, timing);
          
          if (prevDelegate != widget_.get()
              and newAppearance > ClipDelegate::DEGRADED)
            {// a new dedicated clip widget has been created...
              interact::CmdContext::of (steam::cmd::scope_moveRelocateClip, GESTURE_dragReolcate)
                                   .linkSubject (*this)
                                   .setupRelocateDrag();
            }
        }
      
      
      WidgetHook&
      getClipContentCanvas()
        {
          UNIMPLEMENTED ("how to create and wire an embedded canvas for the clip contents/effects");
        }
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_CLIP_PRESENTER_H*/
