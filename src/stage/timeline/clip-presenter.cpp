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
  
  
  
  
  
  ClipPresenter::ClipPresenter (ID identity, ctrl::BusTerm& nexus, optional<int> offsetX)
    : Controller{identity, nexus}
    , channels_{}
    , effects_{}
    , markers_{}
    , widget_{}
    {
      UNIMPLEMENTED ("decide upon the appearance style, based on the presence of the offset. Instantiate the appropriate delegate");
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
               .matchElement ([&](GenNode const& spec, PMarker const& elm) -> bool
                  {
                    return spec.idi == elm->getID();
                  })
               .constructFrom ([&](GenNode const& spec) -> PMarker
                  {
                    return make_unique<MarkerWidget> (spec.idi, this->uiBus_);
                  })
               .buildChildMutator ([&](PMarker& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                  {
                    if (subID != target->getID()) return false;
                    target->buildMutator (buff);
                    return true;
                  }))
        .attach (collection(effects_)
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {                                            // »Selector« : require object-like sub scope with type-field "Effect"
                    return TYPE_Effect == spec.data.recordType();
                  })
               .matchElement ([&](GenNode const& spec, PEffect const& elm) -> bool
                  {
                    return spec.idi == elm->getID();
                  })
               .constructFrom ([&](GenNode const& spec) -> PEffect
                  {
                    return make_unique<ClipPresenter> (spec.idi, this->uiBus_, std::nullopt);     ///////////TICKET #1213 : is it really such a good idea to pass that here??
                  })
               .buildChildMutator ([&](PEffect& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                  {
                    if (subID != target->getID()) return false;
                    target->buildMutator (buff);
                    return true;
                  }))
        .attach (collection(channels_)
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {                                            // »Selector« : require object-like sub scope with type-field "Channel"
                    return TYPE_Channel == spec.data.recordType();
                  })
               .matchElement ([&](GenNode const& spec, PChannel const& elm) -> bool
                  {
                    return spec.idi == elm->getID();
                  })
               .constructFrom ([&](GenNode const& spec) -> PChannel
                  {
                    return make_unique<ClipPresenter> (spec.idi, this->uiBus_, std::nullopt);     ///////////TICKET #1213 : is it really such a good idea to pass that here??
                  })
               .buildChildMutator ([&](PChannel& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                  {
                    if (subID != target->getID()) return false;
                    target->buildMutator (buff);
                    return true;
                  })));
  }
  
  
  int
  ClipPresenter::determineRequiredVerticalExtension()  const
  {
    UNIMPLEMENTED ("any details regarding clip presentation");
  }
  
  
  
}}// namespace stage::timeline
