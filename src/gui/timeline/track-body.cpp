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
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#include "gui/gtk-base.hpp"
#include "gui/timeline/track-body.hpp"

//#include "gui/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

//#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace gui {
namespace timeline {
  
  namespace {
    const uint DEFAULT_OVERVIEW_HEIGHT_px = 20;
    const uint DEFAULT_CONTENT_HEIGHT_px = 80;
  }
  
  
  
  
  TrackBody::TrackBody (ID identity)
    : overviewHeight_{DEFAULT_OVERVIEW_HEIGHT_px}
    , contentHeight_{DEFAULT_OVERVIEW_HEIGHT_px}
    , subTracks_{}
    { }
  
  
  TrackBody::~TrackBody()
  {
    TODO ("detach from parent; store a functor or backreference");
  }
  
  
  /**
   * recursively calculate the height in pixels to display this track,
   * including all nested sub-tracks
   */
  uint
  TrackBody::calcHeight()
  {
    uint heightSum = overviewHeight_ + contentHeight_;
    for (TrackBody* subTrack : subTracks_)
      heightSum += subTrack->calcHeight();
    return heightSum;
  }
  
  
  
}}// namespace gui::timeline
