/*
  TrackBody  -  track body area within the timeline display canvas

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


/** @file track-body.cpp
 ** Implementation details regarding display management of the
 ** track body area within the timeline display canvas.
 ** 
 ** @todo WIP-WIP-WIP as of 6/2019
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/timeline/track-body.hpp"
#include "stage/timeline/track-profile.hpp"

//#include "stage/ui-bus.hpp"
//#include "lib/format-string.hpp"

#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
using util::isnil;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace stage {
namespace timeline {
  
  namespace {
    const uint DEFAULT_CONTENT_HEIGHT_px = 80;
    const uint TIMELINE_BOTTOM_PADDING_px = 5;
  }
  
  
  
  
  TrackBody::TrackBody()
    : contentHeight_{DEFAULT_CONTENT_HEIGHT_px}
    , subTracks_{}
    , rulers_{}
    { }
  
  
  TrackBody::~TrackBody()
  { // indicate change of the global track structure
    signalStructureChange_();
  }
  
  
  void
  TrackBody::setTrackName (cuString& trackName)
  {
    TODO ("is the track name of any relevance for the TrackBody widget?");
  }
  
  
  void
  TrackBody::attachSubTrack (TrackBody* subBody)
  {
    REQUIRE (subBody);
    subTracks_.push_back (subBody);           /////////////////////////////////////////////////////TICKET #1199 : this can not possibly work; we need a way to retain the order of tracks, and we need to detach tracks...
    
    // detect changes of the track structure
    subBody->signalStructureChange_ = signalStructureChange_;
    signalStructureChange_(); // this _is_ such a change
  }
  
  
  /**
   * recursively calculate the height in pixels to display this track,
   * including all nested sub-tracks
   */
  uint
  TrackBody::calcHeight()
  {
    uint heightSum = calcRulerHeight() + contentHeight_;
    for (TrackBody* subTrack : subTracks_)
      heightSum += subTrack->calcHeight();
    return heightSum;
  }
  
  
  /**
   * sum up the vertical extension required by all overview rulers.
   * @return height in pixels, including all gap space
   */
  uint
  TrackBody::calcRulerHeight()
  {
    uint overviewHeight = 0;
    for (auto& ruler : rulers_)
      {
        overviewHeight += ruler->calcHeight()
                        + ruler->getGapHeight();
      }
    return overviewHeight;
  }
  
  
  /**
   * recursively establish the screen space allocation for this structure of nested tracks.
   * The TrackProfile is an abstracted description of the sequence of track elements,
   * which constitute a vertical cross section through the track bodies
   * - pre: the given profile is built and complete up to the (upper side) start of the current track.
   * - post: the profile is elaborated for this track and its children, down to the lower end.
   * @todo 6/19 this very much looks like the "display evaluation pass" envisioned for the timeline::DisplayManager.
   *            Need to find out if we'll need a dedicated evaluation pass and how to interconnect both. 
   */
  void
  TrackBody::establishTrackSpace (TrackProfile& profile)
  {
    bool topLevel = isnil (profile);
    if (topLevel)
      {
        // global setup for the profile
        profile.append_prelude();
        
        // The first Profile elements are always visible on top:
        // Top-level rules and one additionally for the prelude
        profile.pinnedPrefixCnt = 1 + rulers_.size();
      }
    
    for (auto& ruler : rulers_)
      {
        profile.append_ruler (ruler->calcHeight());
        uint gapHeight = ruler->getGapHeight();
        if (gapHeight > 0)
          profile.append_gap (gapHeight);
      }
    profile.append_content (this->contentHeight_);
    if (not isnil (subTracks_))
      {
        profile.addSlopeDown();
        for (TrackBody* subTrack : subTracks_)
          subTrack->establishTrackSpace (profile);
        profile.addSlopeUp();
      }
    
    if (topLevel)
      profile.append_coda (TIMELINE_BOTTOM_PADDING_px);
  }
  
  
  
}}// namespace stage::timeline
