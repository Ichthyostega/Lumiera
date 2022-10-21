/*
  ClipWidget  -  display of a clip in timeline or media bin view

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


/** @file clip-widget.cpp
 ** Implementation details of _clip display_ within the Lumiera UI.
 ** 
 ** @note a "clip" can be a media clip proper, or just some part
 **       of the former, a channel, or even an effect attached to
 **       a clip. Moreover, this clip display can be used both
 **       within the context of the timeline or as element in a
 **       media bin in the asset management section.
 ** 
 ** # Managing Clip appearance and behaviour
 ** 
 ** Here we are looking at the actual implementation of the clip display,
 ** which is hidden behind two layers of abstraction. The interface for
 ** controlling the _desired representation style_ can be found within the
 ** ClipPresenter, which in turn delegates through the ClipDelegate interface.
 ** 
 ** ## Organisation of representation mode
 ** 
 ** Due to the various clip appearance styles, the actual implementation needs
 ** to be flexible on several levels. Thus, the presentation mode is structured
 ** as follows:
 ** - the the Mode of representation controls the basic implementation approach
 **   
 **   * in Mode::HIDDEN, there is no actual UI representation; rather, the ClipDelegate
 **     acts as data container to receive and hold the presentation relevant properties
 **     of the clip, so to be able to return to a visible representation later on.
 **   * in Mode::SUMMARY, we use the help of a mediator to create a summarised display
 **     of timeline contents; the clip is not mapped individually into the display.
 **   * only in Mode::INDIVIDUAL there is an actual GTK widget, attached into the
 **     display framework _in some appropriate way_ -- even in this mode there is
 **     still a lot of flexibility, since the implementing widget itself has several
 **     options for representation, and, moreover, the widget can still be hidden
 **     or out of view.
 ** 
 ** - the ClipDelegate::Appearance can be seen as an ordered scale of increasingly
 **   detailed representation. Some segments of this scale are mapped into the
 **   aforementioned three modes of representation. Especially within the
 **   Mode::INDIVIDUAL, the appearance can be distinguished into
 **   
 **   * ClipDelegate::Appearance::ABRIDGED : the clip acts as placeholder icon
 **   * ClipDelegate::Appearance::COMPACT  : the clip has real extension
 **   * ClipDelegate::Appearance::EXPANDED : details within the clip are revealed
 ** 
 ** ## Choosing the appropriate representation
 ** On construction, the ClipPresenter invokes ClipPresenter::establishAppearance(),
 ** which in turn invokes the generic function ClipDelegate::selectAppearance(), which
 ** in this case will always build a new ClipDelegate, since a CanvasHook ("view") is
 ** explicitly given. Generally speaking, this function ensures there is a delegate,
 ** and this delegate reflects the desired presentation style.
 ** 
 ** When especially the optional argument `timing` is provided by the _population diff_
 ** creating the clip, then we can use the given lib::time::TimeSpan data for actually
 ** allocating a screen rectangle, and thus only when this condition is met (which should
 ** be the default), a ClipWidget is constructed and mapped into presentation.
 ** 
 ** Later the appearance style can be switched, which might incur the necessity also to
 ** exchange the actual implementation of the clip delegate. The latter is the case whenever
 ** we detect a different Mode of representation. Beyond that, we always know there is an
 ** existing delegate, which can be used to retrieve the further detail presentation data.
 ** In some cases the clip needs to be "re-hooked", in which case the existing CanvasHook
 ** is used to establish a new display attachment. For this to work, even the modes not
 ** directly representing the clip need to hold onto some CanvasHook, so each delegate
 ** can implement the ClipDelegate::getCanvas()
 ** 
 ** @todo WIP-WIP-WIP as of 1/2021
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/timeline/clip-widget.hpp"
#include "stage/widget/element-box-widget.hpp"

//#include "stage/ui-bus.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"  ///////////////////TODO debugging

#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>
#include <utility>



using util::_Fmt;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
using lib::time::TimeVar;
using util::unConst;
//using std::cout;
//using std::endl;
using std::optional;


namespace stage {
namespace timeline {
  namespace error = lumiera::error;
  
  const int    ClipDelegate::defaultOffsetY{0};
  const string ClipDelegate::defaultName{_("clip")};
  
  
  ClipDelegate::ClipDelegate()  { }
  ClipDelegate::~ClipDelegate() { }
  
  
  namespace {// details of concrete clip appearance styles...
   
    using WidgetHook   = model::CanvasHook<Gtk::Widget>;
    using HookedWidget = model::CanvasHooked<widget::ElementBoxWidget, Gtk::Widget>;
    
    
    class ClipData
      : public ClipDelegate
      , util::MoveOnly
      {
        TimeVar start_;
        TimeVar dur_;
        
      public: /* === Partial implementation of ClipDelegate === */
        TimeVar&
        accessStartTime()  override
          {
            return start_;
          }
        
        TimeVar&
        accessDuration()  override
          {
            return this->dur_;
          }
        
        
      public:
        ClipData(TimeSpan const& timings =TimeSpan{Time::NEVER, Duration::NIL})
          : ClipDelegate{}
          , start_{timings.start()}
          , dur_{timings.duration()}
          { }
        
        ClipData (ClipData&&)   = default;
      };
    
    
    /**
     * A Clip not directly mapped into presentation,
     * yet present as entity within the timeline framework.
     */
    class DormantClip
      : public ClipData
      {
        WidgetHook& display_;
        uString clipName_;
        
        /* === Interface ClipDelegate === */
        
        Appearance
        currentAppearance()  const override
          {
            return Appearance::PENDING;
          }
        
        Appearance
        changeAppearance (Appearance)  override
          {
            return currentAppearance();
          }
        
        cuString
        getClipName()  const override
          {
            return clipName_;
          }
        
        void
        setClipName(cuString& newName)  override
          {
            clipName_ = newName;
          }
        
        
        /**
         * This is a mere data record without actual presentation,
         * and thus can not occupy any screen extension.
         */
        uint
        calcRequiredHeight()  const override
          {
            return 0;
          }
        
        uint
        getVerticalOffset()  const override
          {
            return 0;
          }
        
        WidgetHook&
        getCanvas()  const override
          {
            return unConst(this)->display_;
          }
        
        void
        updatePosition()  override
          {
            /* NOOP */
          }

        
      public:
        DormantClip(WidgetHook& displayAnchor)
          : ClipData{}
          , display_{displayAnchor}
          , clipName_{}
          { }
        
        /** state switch ctor */
        DormantClip(ClipData&& existing)
          : ClipData{std::move (existing)}
          , display_{existing.getCanvas()}
          , clipName_{existing.getClipName()}
          { }
      };
    
    
    class ClipWidget
      : public HookedWidget
      , public ClipData
      {
        
        /* === Interface ClipDelegate === */
        
        Appearance
        currentAppearance()  const override
          {
            ///////////////////////////////////////////////////////////////////////////////////////TICKET #1038 : determine appearance style dynamically
            return Appearance::COMPACT;
          }
        
        Appearance
        changeAppearance (Appearance desired)  override
          {
            ///////////////////////////////////////////////////////////////////////////////////////TICKET #1038 : change possible appearance style dynamically
            return currentAppearance();
          }
        
        cuString
        getClipName()  const override
          {
            return widget::ElementBoxWidget::getName();
          }
        
        void
        setClipName(cuString& newName)  override
          {
            widget::ElementBoxWidget::setName (newName);
          }
        
        uint
        getVerticalOffset() const override
          {
            return defaultOffsetY;  ///////////////////////////////////////////////////////////////TICKET #1038 : data storage; here : store a per clip vertical offset
          }

        uint
        calcRequiredHeight()  const override
          {
            return this->get_height();   //////////////////////////////////////////////////////////TICKET #1038 : for the first draft we just use a Button; TODO: actual calculation here
          }

        WidgetHook&
        getCanvas()  const override
          {
            return HookedWidget::getCanvas();
          }
        
        void
        updatePosition()  override
          {
            WidgetHook::Pos nominalPos = establishHookPoint(nullptr);
            this->moveTo (nominalPos.x, nominalPos.y);
            establishHorizontalExtension();
          }
        
        /* ==== Size and Layout handling ==== */
        
        /** use underlying canvas metric to derive a size constraint,
         *  taking into account the duration of the clip and the zoom level.
         */
        int
        establishHorizontalExtension()
          {
            int hsiz = getCanvas().getMetric().translateTimeToPixels (accessDuration());
            return hsiz;
          }
        
        
      public:
        ClipWidget(WidgetHook& displayAnchor, TimeSpan const& timings)
          : HookedWidget{displayAnchor.hookedAt(timings, defaultOffsetY), widget::Kind::CONTENT
                                                                        , widget::Type::VIDEO    ////////////TICKET #1251 : how to specify media type for the Clip
                                                                        , widget::name(defaultName)
                                                                        , widget::constrained(
                                                                            [this]() { return establishHorizontalExtension(); }
                                                                        )}
          , ClipData{timings}
          {
            show_all();
          }
        
        /** state switch ctor */
        ClipWidget(ClipData&& existing, WidgetHook* newView =nullptr)
          : HookedWidget{existing.establishHookPoint(newView)           , widget::Kind::CONTENT
                                                                        , widget::Type::VIDEO    ////////////TICKET #1251 : how to specify media type for the Clip
                                                                        , widget::name(existing.getClipName())
                                                                        , widget::constrained(
                                                                            [this]() { return establishHorizontalExtension(); }
                                                                        )}
          , ClipData{std::move (existing)}
          {
            show_all();
          }
      };
    
    
    
    enum Mode { HIDDEN, SUMMARY, INDIVIDUAL };
    
    inline Mode
    classifyAppearance (ClipDelegate::Appearance appearance)
      {
        return appearance < ClipDelegate::SYMBOLIC? HIDDEN
             : appearance < ClipDelegate::ABRIDGED? SUMMARY
             :                                      INDIVIDUAL;
      }
    
    
    /** special convention to suppress a clip with start time == Time::NEVER */
    inline bool
    canShow (Time start)
    {
      return start != Time::NEVER;
    }
    
    inline bool
    canRepresentAsClip (PDelegate& existing, optional<TimeSpan> const& timing)
    {
      return (existing and canShow(existing->accessStartTime()))
          or (not existing and timing and canShow(timing->start())) ;
    }
    
    
    /** @internal either build a new delegate from scratch or build it based on the `existingDelegate`
     *  @remark this function unifies two quite distinct use cases
     *          - either we have mutated an existing clip delegate, and want to reflect the changes in clipData
     *          - or we just inserted a new clip, and have to build the delegate for the given timing
     */
    inline ClipDelegate*
    buildDelegateFor (Mode newMode, PDelegate& existingDelegate, WidgetHook* newView, optional<TimeSpan> const& timing)
    {
      if (existingDelegate)
        { // flip existing delegate to another instance for newMode...
          REQUIRE (INSTANCEOF (ClipData, existingDelegate.get()));
          ClipData& clipData = static_cast<ClipData&> (*existingDelegate);
          
          switch (newMode)
            {
              case HIDDEN:
                return new DormantClip (std::move (clipData));
              case INDIVIDUAL:
                return new ClipWidget (std::move (clipData), newView);
              case SUMMARY:
                UNIMPLEMENTED ("Summary/Overview presentation style");
            }
        }
      else
        { // First time: build new delegate from scratch
          REQUIRE (newMode==HIDDEN or (timing and canShow (timing->start())));
          REQUIRE (newView);
          
          switch (newMode)
            {
              case HIDDEN:
                return new DormantClip{*newView};
              case INDIVIDUAL:
                return new ClipWidget{*newView, *timing};
              case SUMMARY:
                UNIMPLEMENTED ("Summary/Overview presentation style");
            }
        }
      NOTREACHED("unsupported clip display mode.");
    }
  }//(End)clip appearance details.
  
  
  
  /* === Appearance Style state transitions === */
  
  ClipDelegate::Appearance
  ClipDelegate::selectAppearance (PDelegate& existing, Appearance desired, WidgetHook* newView, optional<TimeSpan> const& timing)
  {
    REQUIRE (existing or newView, "need either an existing delegate or also a new View/Canvas");
    
    Appearance current = existing? existing->currentAppearance()
                                 : Appearance::PENDING;
    if (not canRepresentAsClip (existing, timing))
      desired = Appearance::PENDING;
    // classify all possible appearances into three base presentation modes
    Mode curMode = classifyAppearance (current);
    Mode newMode = classifyAppearance (desired);
    
    if (newView or newMode != curMode)
      { // need to switch the clip delegate
        PDelegate newState (buildDelegateFor (newMode, existing, newView, timing));
        swap (existing, newState);
      }
    ENSURE (existing);
    return existing->changeAppearance (desired);
    // fine-tune appearance style within limits of the mode established
  }
  
  
  WidgetHook::Pos
  ClipDelegate::establishHookPoint (WidgetHook* newView)
  {
    if (not newView)
      newView = & getCanvas();
    return newView->hookedAt (Time{accessStartTime()}, defaultOffsetY);
  }
  
  
  Gtk::Widget&
  ClipDelegate::expect_and_expose_Widget (PDelegate& manager)
  {
    if (manager and manager->currentAppearance() >= ClipDelegate::ABRIDGED)
      return static_cast<ClipWidget&> (*manager);
    
    else
      throw error::State (_Fmt{"Attempt to access the Widget for clip('%s') in presentation state %d. "
                               "This implies an error in the signal wiring logic and state handling."}
                              % string{manager? manager->getClipName() : "<not initialised>"}
                              % int   {manager? manager->currentAppearance() : -1}
                         ,LERR_(UIWIRING)
                         );
  }

  
  
}}// namespace stage::timeline
