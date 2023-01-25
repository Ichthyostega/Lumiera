/*
  TRACK-PRESENTER.hpp  -  presentation control element for a track within the timeline

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


/** @file track-presenter.hpp
 ** Presentation control element to model and manage a track within the timeline UI.
 ** In the Lumiera timeline UI, we are mixing two different scope of concerns: For one,
 ** we have the globally tangible scope of actual session elements and operations performed
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
 ** only a small number of elements can be visible and active as far as user interaction is concerned.
 ** 
 ** # Structure of the TrackPresenter
 ** 
 ** Each TrackPresenter corresponds to a "sub-Fork" of timeline tracks. Since Lumiera always arranges
 ** tracks as nested scopes into a tree, there is one root fork, recursively holding several sub forks.
 ** - thus each TrackPresenter holds a collection #subFork_ -- possibly empty.
 ** - moreover, it holds a collection #clips_, which represent the actual content of this track itself,
 **   as opposed to content on some sub-track. These clips are to be arranged within the _content area_
 **   of the track display, in the track body area (at the right side of the timeline). Actually, this
 **   collection holds timeline::ClipPresenter objects, thus repeating the same design pattern.
 ** - in addition, there can be a collection of #markers_, to be translated into various kinds of
 **   region or point/location markup, typically shown in the (optional) _overview ruler,_ running
 **   along the top-side of this track's display area.
 **   
 ** Since TrackPresenter is a model::Tangible, a central concern is the ability to respond to
 ** _diff messages._ In fact, any actual content, including all the nested sub-structures, is
 ** _populated_ through such _mutation messages_ sent from the session up via the stage::UiBus.
 ** Thus, the TrackPresenter::buildMutator() implementation hooks up the necessary callbacks,
 ** to allow adding and removing of sub elements and properties of a track.
 ** 
 ** Another concern handled here is the coordination of layout and display activities.
 ** A special twist arises here: The track header ("patchbay") display can be designed as a
 ** classical tree / grid control, while the actual timeline body contents require us to perform
 ** custom drawing activities. Which leads to the necessity to coordinate and connect two distinct
 ** presentation schemes to form a coherent layout. We solve this challenge by introducing a helper
 ** entity, the DisplayFrame. These act as a bridge to hook into both display hierarchies (the nested
 ** TrackHeaderWidget and the TrackBody record managed by the BodyCanvasWidget). Display frames are
 ** hooked down from their respective parent frame, thereby creating a properly interwoven fabric.  
 ** 
 ** After assembling the necessary GTK widgets, typically our custom drawing code will be invoked
 ** at some point, thereby triggering BodyCanvasWidget::maybeRebuildLayout(). At this point the
 ** timeline::TrackProfile needs to be established, so to reflect the succession and extension
 ** of actual track spaces running alongside the time axis. This is accomplished through a global
 ** timeline::DisplayEvaluation pass, recursively visiting all the involved parts to perform
 ** size adjustments, until the layout is globally balanced.
 ** 
 ** # Relative Coordinate System
 ** 
 ** With respect to the TrackBody, the DisplayFrame within each Track acts as a relative attachment
 ** point and relative coordinate system; this is implemented as stage::model::RelativeCanvasHook.
 ** The top-level anchor point is established in the ctor of timeline::TimelineController, where
 ** the actual TimelineLayout is passed as parent CanvasHook; from this point below, each parent
 ** track acts as reference CanvasHook for the child tracks.
 ** 
 ** \par Impact of indirections:
 ** to keep matters simple for the initial implementation, these relationships were modelled as
 ** layered interfaces / overridden virtual methods. This leads to a chain of calls through several
 ** VTables; the practical impact of this scheme on the performance of the timeline GUI will be
 ** investigated when there is a sufficiently complete implementation available -- see #1254
 ** 
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** @todo still WIP as of 3/2020 -- yet the basic structure is settled by now.
 ** 
 */


#ifndef STAGE_TIMELINE_TRACK_PRESENTER_H
#define STAGE_TIMELINE_TRACK_PRESENTER_H

#include "stage/gtk-base.hpp"
#include "include/ui-protocol.hpp"
#include "stage/model/view-hook.hpp"
#include "stage/model/controller.hpp"
#include "stage/timeline/display-evaluation.hpp"
#include "stage/timeline/marker-widget.hpp"
#include "stage/timeline/clip-presenter.hpp"
#include "stage/timeline/track-head-widget.hpp"
#include "stage/timeline/track-body.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/iter-tree-explorer.hpp"
#include "lib/util-coll.hpp"

#include "lib/nocopy.hpp"

#include <optional>
#include <utility>
#include <vector>



namespace stage  {
namespace timeline {
  
  using std::vector;
  using std::unique_ptr;
  using std::make_unique;
  
  using lib::time::TimeSpan;
  using lib::diff::TreeMutator;
  using lib::diff::collection;
  using lib::explore;
  using util::max;
  
  
  
  /**
   * Special CanvasHook decorator to apply a (dynamic) offset
   * when attaching or moving Widgets on the shared canvas.
   * @note the ctor uses #getAnchorHook, thus effectively,
   *       for a chain of RelativeCanvasHook instances,
   *       the #refHook_ holds the top level anchor.
   * @remark avoiding this 2-step indirect dispatch is possibly,
   *       bug likely not of any significance, given the overhead
   *       of drawing.
   * @todo investigate real-live performance ////////////////////////////////////////////////////////////////TICKET #1254 : investigate impact of indirection 
   */
  template<class WID>
  class RelativeCanvasHook
    : public model::CanvasHook<WID>
    {
      model::CanvasHook<WID>& refHook_;
      
      
      /* ==== Interface: ViewHook ===== */
      
      void
      hook (WID& widget, int xPos=0, int yPos=0) override
        {
          refHook_.hook (widget, hookAdjX (xPos), hookAdjY (yPos));
        }
      
      void
      move (WID& widget, int xPos, int yPos)  override
        {
          refHook_.move (widget, hookAdjX (xPos), hookAdjY (yPos));
        }

      void
      remove (WID& widget)  override
        {
          refHook_.remove (widget);
        }
      
      /** allow to build a derived relative hook with different offset */
      model::CanvasHook<WID>&
      getAnchorHook()  noexcept override
        {
          return this->refHook_;
        }
      
    protected: /* === extended Interface for relative canvas hook === */
      virtual int hookAdjX (int xPos)  =0;
      virtual int hookAdjY (int yPos)  =0;
      
      /** delegating layout metric to the root canvas */
      model::DisplayMetric&
      getMetric()  const override
        {
          return refHook_.getMetric();
        }
      
    public:
      RelativeCanvasHook(model::CanvasHook<WID>& baseHook)
        : refHook_{baseHook.getAnchorHook()}
        { }
    };
  
  
  
  using PFork  = unique_ptr<TrackPresenter>;
  using PClip  = unique_ptr<ClipPresenter>;
  using PMark  = unique_ptr<MarkerWidget>;
  using PRuler = unique_ptr<RulerTrack>;
  
  /**
   * Reference frame to organise the presentation related to a specific Track in the Timeline-GUI.
   * With the help of such a common frame of reference, we solve the problem that each individual
   * track display needs to hook into two distinct UI presentation structures: the track head controls
   * and the presentation of track contents on the BodyCanvasWidget.
   * @note the RelativeCanvasHook sub-interface is initialised by chaining with the given displayAnchor.
   *       Now, since RelativeCanvasHook itself removes intermediary delegates when chaining, in the end
   *       the #getClipHook() exposes the top-level canvas, while applying our local #hookAdjX()
   */
  class DisplayFrame
    : util::NonCopyable
    , public DisplayViewHooks
    , public RelativeCanvasHook<Gtk::Widget>
    {
      model::ViewHooked<TrackHeadWidget> head_;
      model::ViewHooked<TrackBody>       body_;
      
      /* === extended Interface for relative view/canvas hook === */
      
      int hookAdjX (int xPos)  override { return xPos; };
      int hookAdjY (int yPos)  override { return yPos + body_.getContentOffsetY(); };
      
    public: /* ==== Interface: DisplayViewHooks===== */
      
      model::ViewHook<TrackHeadWidget>& getHeadHook()  override { return head_; };
      model::ViewHook<TrackBody>&       getBodyHook()  override { return body_; };
      model::CanvasHook<Gtk::Widget>&   getClipHook()  override { return *this; };
      
    public:
      DisplayFrame (DisplayViewHooks& displayAnchor)
        : RelativeCanvasHook{displayAnchor.getClipHook()}
        , head_{displayAnchor.getHeadHook()}
        , body_{displayAnchor.getBodyHook()}
        { }
      
      void
      setTrackName (cuString& name)
        {
          head_.setTrackName (name);
          body_.setTrackName (name);  ///////////////////////////////////TICKET #1017 -- TODO 11/18 : not clear yet if TrackBody needs to know its name
        }
      
      vector<unique_ptr<RulerTrack>>&
      bindRulers()
        {
          return body_.bindRulers();
        }
      
      void establishExtension (vector<PClip>&, vector<PMark>&);
      void sync_and_balance (DisplayEvaluation&);
    };
  
  
  /**
   * A View-Model entity to represent a timeline track in the UI.
   * @todo WIP-WIP as of 12/2016
   */
  class TrackPresenter
    : public model::Controller
    , public LayoutElement
    {
      DisplayFrame display_;
      
      vector<PFork> subFork_;
      vector<PMark> markers_;
      vector<PClip> clips_;
      
      
    public:
      /**
       * @param id identity used to refer to a corresponding session::Fork
       * @param nexus a way to connect this Controller to the UI-Bus.
       * @param anchorDisplay a one-way closure to attach into the display fabric
       */
      TrackPresenter (ID id, ctrl::BusTerm& nexus, DisplayViewHooks& displayAnchor)
        : Controller{id, nexus}
        , display_{displayAnchor}
        , subFork_{}
        , markers_{}
        , clips_{}
        {
          setTrackName (id.getSym());  // fallback initialise track-name from human-readable ID symbol
        }
      
      
      /** set up a binding to respond to mutation messages via UiBus */
      virtual void buildMutator (lib::diff::TreeMutator::Handle)  override;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1201 : test/code... remove this
      void injectDebugTrackLabels();
      void attachElementBox();
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1201 : test/code... remove this
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1201 : test/code... remove this
      uString TODO_trackName_;
      widget::ElementBoxWidget* theBox_{nullptr};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1201 : test/code... remove this
      
    protected: /* ==== Interface: LayoutElement ===== */
      
      void establishLayout (DisplayEvaluation&)  override;
      void completeLayout (DisplayEvaluation&)   override;
      
    private:/* ===== Internals ===== */
      
      /** invoked via diff to show a (changed) track name */
      void
      setTrackName (string name)
        {
          display_.setTrackName (name);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1201 : test/code... remove this
          TODO_trackName_ = name;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1201 : test/code... remove this
        }
      
      void relinkContents ();
    };
  
  
  
  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1201 : test/code... remove this
  inline void
  TrackPresenter::injectDebugTrackLabels()
  {
    uint x = rand() % 50;
    uint y = 0;
    Gtk::Button* butt = Gtk::manage (new model::CanvasHooked<Gtk::Button, Gtk::Widget>{display_.hookedAt(x,y), TODO_trackName_});
    butt->signal_clicked().connect(
          [butt]{ cout << "|=="<<butt->get_label()<<endl; });
    butt->show();
    
    for (auto& subTrack : subFork_)
      subTrack->injectDebugTrackLabels();
  }
  inline void
  TrackPresenter::attachElementBox()
  {
    uint x = rand() % 200;
    uint y = 0;
    theBox_ = Gtk::manage (
        new model::CanvasHooked<widget::ElementBoxWidget, Gtk::Widget>{display_.hookedAt(x,y)
                                                                      , widget::Kind::MARK
                                                                      , widget::Type::LABEL
                                                                      , widget::name("ΩΩΩ")
                                                                      , widget::constrained(
                                                                          [arbitrarySiz = 10 + rand() % 80]() -> int
                                                                            {
                                                                              cout << "PPPP:: arbitrary="<<arbitrarySiz<<endl;
                                                                              return arbitrarySiz;
                                                                            })
                                                                      });
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1201 : test/code... remove this
  /**
   * @note we distinguish between the contents of our four nested child collections
   *       based on the symbolic type field sent in the Record type within the diff representation
   *       - "Marker" designates a Marker object
   *       - "Clip" designates a Clip placed on this track
   *       - "Fork" designates a nested sub-track
   *       - "Ruler" designates a nested ruler (timescale, overview,....) belonging to this track
   * @see TimelineController::buildMutator() for a basic explanation of the data binding mechanism
   */
  inline void
  TrackPresenter::buildMutator (TreeMutator::Handle buffer)
  {
    buffer.emplace(
      TreeMutator::build()
        .attach (collection(display_.bindRulers())
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {                                            // »Selector« : require object-like sub scope with type-field "Ruler"
                    return TYPE_Ruler == spec.data.recordType();
                  })
               .constructFrom ([&](GenNode const& spec) -> PRuler
                  {                                            // »Constructor« : how to attach a new ruler track
                    return make_unique<RulerTrack> (spec.idi, this->uiBus_, *this);
                  }))
        .attach (collection(markers_)
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {                                            // »Selector« : require object-like sub scope with type-field "Marker"
                    return TYPE_Marker == spec.data.recordType();
                  })
               .constructFrom ([&](GenNode const& spec) -> PMark
                  {
                    return make_unique<MarkerWidget> (spec.idi, this->uiBus_);
                  }))
        .attach (collection(clips_)
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {                                            // »Selector« : require object-like sub scope with type-field "Clip"
                    return TYPE_Clip == spec.data.recordType();
                  })
               .constructFrom ([&](GenNode const& spec) -> PClip
                  {
                    std::optional<TimeSpan> timing = spec.retrieveAttribute<TimeSpan> (string{ATTR_timing});
                    return make_unique<ClipPresenter> (spec.idi, this->uiBus_, display_.getClipHook(), timing);
                  }))                                                                              //////////TICKET #1251 : how to specify media type for the Clip
        .attach (collection(subFork_)
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {                                            // »Selector« : require object-like sub scope with type-field "Fork"
                    return TYPE_Fork == spec.data.recordType();
                  })
               .constructFrom ([&](GenNode const& spec) -> PFork
                  {
                    return make_unique<TrackPresenter> (spec.idi, uiBus_, this->display_);
                  }))
        .change(ATTR_name, [&](string val)
            {                                                  // »Attribute Setter« : receive a new value for the track name field
              this->setTrackName (val);
            }));
  }
  
  
  
  /** handle Phase-1 of the DisplayEvaluation pass for this track and its sub-tracks. */
  inline void
  TrackPresenter::establishLayout (DisplayEvaluation& displayEvaluation)
  {
    ASSERT (displayEvaluation.isCollectPhase());
    display_.establishExtension (clips_, markers_);
    for (auto& subTrack: subFork_)
      subTrack->establishLayout (displayEvaluation);
  }
  
  /** handle Phase-2 (collect/balancing phase) for this track and its sub-tracks. */
  inline void
  TrackPresenter::completeLayout (DisplayEvaluation& displayEvaluation)
  {
    ASSERT (not displayEvaluation.isCollectPhase());
    // recursion first, so all sub-Tracks are already balanced
    for (auto& subTrack: subFork_)
      subTrack->completeLayout (displayEvaluation);
    this->relinkContents();
    // re-sync and match the header / body display
    display_.sync_and_balance (displayEvaluation);
  }

  /**
   * Find out about the vertical extension of a single track display.
   * @note will be invoked during the first recursive walk, at which point
   *       the TrackProfile has not yet been established; the latter will
   *       happen after returning from that recursive walk. Thus within this
   *       first pass, we can only adjust local sizes within the content area,
   *       while the second pass, in #relinkContents() can also investigate
   *       the track body vertical extension, defined by the TrackProfile.
   */
  inline void
  DisplayFrame::establishExtension (vector<PClip>& clips, vector<PMark>&)
  {
    uint maxVSize = max (explore (clips)
                          .transform([](PClip const& clip)
                                       {
                                         return clip->determineRequiredVerticalExtension();
                                       }));
    maxVSize = max (maxVSize, head_.getContentHeight());
    this->body_.accommodateContentHeight (maxVSize);
    this->head_.accommodateContentHeight (maxVSize);
  }
  
  /** second pass of the DisplayEvaluation:
   *  reassemble content to match adjusted layout
   * @todo 2/2021 WIP-WIP initial draft; many aspects still unclear
   */
  inline void
  TrackPresenter::relinkContents ()
  {
    for (auto& clip: clips_)
      clip->relink();
    for (auto& mark: markers_)
      mark->relink();
  }
  
  
  /** re-flow and adjust after the global layout has been established
   *  At this point we can assume that both header and body are updated
   *  and have valid extensions within their perimeter. But the coordination
   *  of track head display and body content might be out of sync and needs
   *  readjustments. This second pass gives the opportunity to fix such
   *  discrepancies by further increasing vertical extension, but this
   *  also has the consequence to trigger yet another DisplayEvaluation,
   *  since any readjustment invalidates the global layout. However, since
   *  all adjustments are done by increasing monotonously, after several
   *  recursions the layout will be balanced eventually.
   * @note We can increase a head to match body size. Otherwise, if the body
   *       is too small, we're out of luck, since the Profile is already calculated.
   *       However, since we accommodated the local extension within each content area
   *       prior to calculating the Profile, chances are that such a discrepancy is small.
   * @remark we could also consider to transport a remaining discrepancy downwards by using
   *       a state variable in the DisplayEvaluation
   */
  inline void
  DisplayFrame::sync_and_balance (DisplayEvaluation&)
  {
    uint contentHeight = body_.calcContentHeight();
    uint overallHeight = contentHeight + body_.calcSubtrackHeight();
    head_.syncSubtrackStartHeight (contentHeight);
    head_.accommodateOverallHeight (overallHeight);
  }


  
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_TRACK_PRESENTER_H*/
