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
 ** Especially here we define the model binding of all sub-elements belonging
 ** to a given track. The TrackPresenter::buildMutator() implementation hooks up
 ** the necessary callbacks, to allow adding and removing of sub elements and properties
 ** of a track, by sending appropriate _mutation messages_ over the stage::UiBus.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** 
 */


#include "stage/gtk-base.hpp"
#include "include/ui-protocol.hpp"
#include "stage/timeline/track-presenter.hpp"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1201 : test/code... remove this
#include "stage/timeline/body-canvas-widget.hpp"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1201 : test/code... remove this

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
  
  
  
  
  
  
  
  TrackPresenter::~TrackPresenter()
  {
    TODO ("find a way how to detach from parent tracks");        ////////////////////////////////////////////TICKET #1199 : how to deal with re-ordering of tracks?
  }
  
  
  
  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1201 : test/code... remove this
  void
  TrackPresenter::injectDebugTrackLabels(BodyCanvasWidget& bodyCanvas)
  {
    bodyCanvas.DEBUG_injectTrackLabel (display_.body.TODO_trackName_, display_.body.startLine_ + display_.body.contentOffset_);
    for (auto& subTrack : subFork_)
      subTrack->injectDebugTrackLabels (bodyCanvas);
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
  void
  TrackPresenter::buildMutator (TreeMutator::Handle buffer)
  {
    using PFork   = unique_ptr<TrackPresenter>;
    using PClip   = unique_ptr<ClipPresenter>;
    using PMarker = unique_ptr<MarkerWidget>;
    using PRuler  = unique_ptr<RulerTrack>;
    
    buffer.create (
      TreeMutator::build()
        .attach (collection(display_.body.bindRulers())
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {                                            // »Selector« : require object-like sub scope with type-field "Ruler"
                    return TYPE_Ruler == spec.data.recordType();
                  })
               .matchElement ([&](GenNode const& spec, PRuler const& elm) -> bool
                  {
                    return spec.idi == ID{*elm};
                  })
               .constructFrom ([&](GenNode const& spec) -> PRuler
                  {                                            // »Constructor« : how to attach a new ruler track
                    return make_unique<RulerTrack> (spec.idi, this->uiBus_, *this);
                  })
               .buildChildMutator ([&](PRuler& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                  {
                    if (ID{*target} != subID) return false;
                    target->buildMutator (buff);
                    return true;
                  }))
        .attach (collection(markers_)
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {                                            // »Selector« : require object-like sub scope with type-field "Marker"
                    return TYPE_Marker == spec.data.recordType();
                  })
               .matchElement ([&](GenNode const& spec, PMarker const& elm) -> bool
                  {
                    return spec.idi == ID{*elm};
                  })
               .constructFrom ([&](GenNode const& spec) -> PMarker
                  {
                    return make_unique<MarkerWidget> (spec.idi, this->uiBus_);
                  })
               .buildChildMutator ([&](PMarker& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                  {
                    if (ID{*target} != subID) return false;
                    target->buildMutator (buff);
                    return true;
                  }))
        .attach (collection(clips_)
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {                                            // »Selector« : require object-like sub scope with type-field "Clip"
                    return TYPE_Clip == spec.data.recordType();
                  })
               .matchElement ([&](GenNode const& spec, PClip const& elm) -> bool
                  {
                    return spec.idi == ID{*elm};
                  })
               .constructFrom ([&](GenNode const& spec) -> PClip
                  {
                    return make_unique<ClipPresenter> (spec.idi, this->uiBus_);
                  })
               .buildChildMutator ([&](PClip& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                  {
                    if (ID{*target} != subID) return false;
                    target->buildMutator (buff);
                    return true;
                  }))
        .attach (collection(subFork_)
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {                                            // »Selector« : require object-like sub scope with type-field "Fork"
                    return TYPE_Fork == spec.data.recordType();
                  })
               .matchElement ([&](GenNode const& spec, PFork const& elm) -> bool
                  {
                    return spec.idi == ID{*elm};
                  })
               .constructFrom ([&](GenNode const& spec) -> PFork
                  {
                    return make_unique<TrackPresenter> (spec.idi
                                                       ,this->uiBus_
                                                       ,[&](TrackHeadWidget& head,TrackBody& body)
                                                          {
                                                            display_.injectSubTrack (head, body); 
                                                          });
                  })
               .buildChildMutator ([&](PFork& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                  {
                    if (ID{*target} != subID) return false;
                    target->buildMutator (buff);
                    return true;
                  }))
        .change(ATTR_name, [&](string val)
            {                                                  // »Attribute Setter« : receive a new value for the track name field
              this->setTrackName (val);
            }));
  }
  
  
  
  
}}// namespace stage::timeline
