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
  }
  
  
  
  
  TrackBody::TrackBody()
    : contentHeight_{DEFAULT_CONTENT_HEIGHT_px}
    , subTracks_{}
    , rulers_{}
    { }
  
  
  TrackBody::~TrackBody()
  {
    TODO ("detach from parent; store a functor or backreference");
  }
  
  
  void
  TrackBody::setTrackName (cuString& trackName)
  {
    TODO ("is the track name of any relevance for the TrackBody widget?");
  }
  
  
  /**
   * recursively calculate the height in pixels to display this track,
   * including all nested sub-tracks
   */
  uint
  TrackBody::calcHeight()
  {
    uint overviewHeight = 0;
    for (auto& ruler : rulers_)
      {
        overviewHeight += ruler->calcHeight();
                        + ruler->getGapHeight();
      }
    uint heightSum = overviewHeight + contentHeight_;
    for (TrackBody* subTrack : subTracks_)
      heightSum += subTrack->calcHeight();
    return heightSum;
  }
  
  
  /**
   * recursively establish the screen space allocation for this structure of nested tracks.
   * The TrackProfile is an abstracted description of the sequence of track elements,
   * which constitute a vertical cross section through the track bodies
   * - pre: the given profile is built and complete up to the (upper side) start of this timeline
   * - post: the profile is elaborated for this track and its children, down to the lower end.
   * 
   */
  void
  TrackBody::establishTrackSpace (TrackProfile& profile)
  {
    for (auto& ruler : rulers_)
      {
        profile.append_ruler (ruler->calcHeight());
        uint gapHeight = ruler->getGapHeight();
        if (gapHeight > 0)
          profile.append_gap (gapHeight);
      }
    profile.append_content (this->contentHeight_);
    if (not isnil(subTracks_))
      {
        profile.addSlopeDown();
        for (TrackBody* subTrack : subTracks_)
          subTrack->establishTrackSpace (profile);
        profile.addSlopeUp();
      }
  }
  
  
  
}}// namespace stage::timeline
