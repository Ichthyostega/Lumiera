/*
  TrackPresenter  -  presentation control element for a track within the timeline

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


/** @file track-presenter.cpp
 ** Implementation details of track presentation management.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** 
 */


#include "gui/gtk-base.hpp"
#include "gui/timeline/track-presenter.hpp"
#include "gui/timeline/clip-presenter.hpp"
#include "gui/timeline/marker-widget.hpp"

//#include "gui/ui-bus.hpp"
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


namespace gui {
namespace timeline {
  
  
  
  
  
  TrackPresenter::TrackPresenter (ID identity, ctrl::BusTerm& nexus)
    : Controller{identity, nexus}
    , subFork_{}
    , markers_{}
    , clips_{}
    , head_{}
    , body_{}
    {
      UNIMPLEMENTED ("how to attach the TrackPresenter into the two relevant GTK display contexts");
    }
  
  
  TrackPresenter::~TrackPresenter()
  {
  }
  
  
  
  
  /**
   * @note we distinguish between the contents of our three nested child collections
   *       based on the symbolic type field sent in the Record type within the diff representation
   *       - "Marker" designates a Marker object
   *       - "Clip" designates a Clip placed on this track
   *       - "Fork" designates a nested sub-track
   * @see TimelineController::buildMutator() for a basic explanation of the data binding mechanism
   */
  void
  TrackPresenter::buildMutator (TreeMutator::Handle buffer)
  {
    using PFork   = unique_ptr<TrackPresenter>;
    using PClip   = unique_ptr<ClipPresenter>;
    using PMarker = unique_ptr<MarkerWidget>;
    
    buffer.create (
      TreeMutator::build()
        .attach (collection(markers_)
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {                                            // »Selector« : require object-like sub scope with type-field "Marker"
                    return "Marker" == spec.data.recordType();
                  })
               .matchElement ([&](GenNode const& spec, PMarker const& elm) -> bool
                  {
                    return spec.idi == ID(elm);
                  })
               .constructFrom ([&](GenNode const& spec) -> PMarker
                  {
                    return make_unique<MarkerWidget>(spec.idi, this->uiBus_);
                  })
               .buildChildMutator ([&](PMarker& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                  {
                    if (ID(target) != subID) return false;
                    target->buildMutator (buff);
                    return true;
                  }))
        .attach (collection(clips_)
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {                                            // »Selector« : require object-like sub scope with type-field "Clip"
                    return "Clip" == spec.data.recordType();
                  })
               .matchElement ([&](GenNode const& spec, PClip const& elm) -> bool
                  {
                    return spec.idi == ID(elm);
                  })
               .constructFrom ([&](GenNode const& spec) -> PClip
                  {
                    return make_unique<ClipPresenter>(spec.idi, this->uiBus_);
                  })
               .buildChildMutator ([&](PClip& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                  {
                    if (ID(target) != subID) return false;
                    target->buildMutator (buff);
                    return true;
                  }))
        .attach (collection(subFork_)
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {                                            // »Selector« : require object-like sub scope with type-field "Fork"
                    return "Fork" == spec.data.recordType();
                  })
               .matchElement ([&](GenNode const& spec, PFork const& elm) -> bool
                  {
                    return spec.idi == ID(elm);
                  })
               .constructFrom ([&](GenNode const& spec) -> PFork
                  {
                    return make_unique<TrackPresenter>(spec.idi, this->uiBus_);
                  })
               .buildChildMutator ([&](PFork& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                  {
                    if (ID(target) != subID) return false;
                    target->buildMutator (buff);
                    return true;
                  })));
  }
  
  
  
  
}}// namespace gui::timeline
