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
 ** On construction, the ClipPresenter invokes ClipDelegate::buildDelegate().
 ** This is a limited variant of the more general ClipDelegate::switchAppearance(),
 ** insofar on construction we must ensure there always exists some kind of delegate.
 ** When especially the optional argument `timing` is provided by the _population diff_
 ** creating the clip, then we can use the given lib::time::TimeSpan data for actually
 ** allocating a screen rectangle, and thus only in this case, a ClipWidget is constructed
 ** and mapped into presentation.
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

//#include "stage/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
using util::unConst;
//using std::cout;
//using std::endl;
using std::optional;
using std::nullopt;


namespace stage {
namespace timeline {
  
  const int    ClipDelegate::defaultOffsetY{0};
  const string ClipDelegate::defaultName{_("clip")};
  
  
  ClipDelegate::~ClipDelegate() { }
  
  
  ClipDelegate::ClipDelegate()
    {
    }
  
  
  namespace {// details of concrete clip appearance styles...
   
    using WidgetHook   = model::CanvasHook<Gtk::Widget>;
    using HookedWidget = model::CanvasHooked<Gtk::Button, Gtk::Widget>;   ///////////////////////////////////////////TICKET #1211 : need preliminary placeholder clip widget for timeline layout
    
    enum Mode { HIDDEN, SUMMARY, INDIVIDUAL };
    
    inline Mode
    classify (ClipDelegate::Appearance appearance)
      {
        return appearance < ClipDelegate::SYMBOLIC? HIDDEN
             : appearance < ClipDelegate::ABRIDGED? SUMMARY
             :                                      INDIVIDUAL;
      }
    
    
    class ClipData
      : public ClipDelegate
      , util::NonCopyable
      {
        WidgetHook& display_;
        
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
            return "lala LOLO";     ///////////////////////////////////////////////////////////////TICKET #1038 : data storage; here : store the clip name/ID
          }
        
        Time
        getStartTime()  const override
          {
            return Time::NEVER;     ///////////////////////////////////////////////////////////////TICKET #1038 : data storage; here : store the clip start time
          }

        WidgetHook&
        getCanvas()  const override
          {
            return unConst(this)->display_;
          }
        
        
      public:
        ClipData(WidgetHook& displayAnchor)
          : ClipDelegate{}
          , display_{displayAnchor}
          { }
        
        /** state switch ctor */
        ClipData(ClipDelegate& existing)
          : ClipDelegate{}
          , display_{existing.getCanvas()}
          {
            TODO("copy further clip presentation properties");
          }
      };
    
    
    class ClipWidget
      : public HookedWidget
      , public ClipDelegate
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
            return this->get_label();
          }
        
        Time
        getStartTime()  const override
          {
            return Time::NEVER;     ///////////////////////////////////////////////////////////////TICKET #1038 : data storage; here : store the clip start time
          }

        WidgetHook&
        getCanvas()  const override
          {
            return HookedWidget::getCanvas();
          }
        
        
      public:
        ClipWidget(WidgetHook::Pos hookPoint, Duration dur, uString clipName)
          : HookedWidget{hookPoint, clipName}
          , ClipDelegate{}
          { }
        
        /** state switch ctor */
        ClipWidget(ClipDelegate& existing, WidgetHook* newView)
          : HookedWidget{existing.establishHookPoint(newView), existing.getClipName()}
          , ClipDelegate{}
          {
            TODO("copy further clip presentation properties");
          }
      };
    
    
    inline ClipDelegate*
    buildDelegateFor (Mode newMode, ClipDelegate& existingDelegate, WidgetHook* newView =0)
      {
        switch (newMode)
          { 
            case HIDDEN:
              return new ClipData (existingDelegate);
            case INDIVIDUAL:
              return new ClipWidget (existingDelegate, newView);
            case SUMMARY:
              UNIMPLEMENTED ("Summary/Overview presentation style");
          }
      }
    
  }//(End)clip appearance details.
  
  
  
  /* === Appearance Style state transitions === */
  
  ClipDelegate::Appearance
  ClipDelegate::switchAppearance (PDelegate& manager, Appearance desired, WidgetHook* newView)
  {
    REQUIRE (manager, "pre-existing clip delegate required");
    Mode curMode = classify (manager->currentAppearance());
    Mode newMode = classify (desired);
    if (newMode != curMode or newView)
      { // need to switch the clip delegate
        PDelegate newState (buildDelegateFor (newMode, *manager, newView));
        swap (manager, newState);
        return manager->changeAppearance (desired);
      }
    else
      return manager->changeAppearance (desired);
  }
  
  
  ClipDelegate::Appearance
  ClipDelegate::buildDelegate (PDelegate& manager, WidgetHook& view, optional<TimeSpan> timing)
  {
    if (timing)
      manager.reset (new ClipWidget{view.hookedAt(*timing, defaultOffsetY), timing->duration(), defaultName});
    else
      manager.reset (new ClipData{view});
    
    return timing? Appearance::COMPACT
                 : Appearance::PENDING;
  }
  
  
  WidgetHook::Pos
  ClipDelegate::establishHookPoint (WidgetHook* newView)
  {
    if (not newView)
      newView = & getCanvas();
    return newView->hookedAt (getStartTime(), defaultOffsetY);
  }
  
  
}}// namespace stage::timeline
