/*
  ClipPresenter  -  presentation control element for a clip within the timeline

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


/** @file clip-presenter.cpp
 ** Implementation details of clip presentation management.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** 
 */


#include "stage/gtk-base.hpp"
#include "include/ui-protocol.hpp"
#include "stage/timeline/clip-presenter.hpp"
#include "stage/timeline/marker-widget.hpp"

//#include "stage/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

//#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
using lib::diff::TreeMutator;
using lib::diff::collection;
using std::make_unique;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace stage {
namespace timeline {
  
  
  
  
  /**
   * @param identity referring to the corresponding session::Clip in Steam-Layer.
   * @param nexus a way to connect this Controller to the UI-Bus.
   * @param view (abstracted) canvas or display framework to attach this clip to
   * @param timing (optional) start time point and duration of the clip.
   * @note Clip can not be displayed unless #timing is given
   */
  ClipPresenter::ClipPresenter (ID identity, ctrl::BusTerm& nexus, WidgetHook& view, optional<TimeSpan> const& timing)
    : Controller{identity, nexus}
    , channels_{}
    , effects_{}
    , markers_{}
    , widget_{}
    {
      establishAppearance (&view, timing);
      ENSURE (widget_);
    }
  
  
  ClipPresenter::~ClipPresenter()
  {
  }
  
  
  
  
  void
  ClipPresenter::buildMutator (TreeMutator::Handle buffer)
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
  
  
  
  void
  ClipPresenter::establishAppearance (WidgetHook* newView, optional<TimeSpan> const& timing)
  {
    ClipDelegate::selectAppearance (this->widget_, defaultAppearance, newView, timing);
  }
  
  WidgetHook&
  ClipPresenter::getClipContentCanvas()
  {
    UNIMPLEMENTED ("how to create and wire an embedded canvas for the clip contents/effects");
  }

  uint
  ClipPresenter::determineRequiredVerticalExtension()  const
  {
    REQUIRE (widget_);
    return widget_->calcRequiredHeight()
         + widget_->getVerticalOffset();
  }
  
  void
  ClipPresenter::relink()
  {
    REQUIRE (widget_);
    widget_->updatePosition();
  }
  
  
}}// namespace stage::timeline
