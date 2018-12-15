/*
  RulerTrack  -  track body area to show overview and timecode and markers

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file ruler-track.cpp
 ** Implementation details regarding custom drawing of track overview
 ** and time code ticks and markers onto the TimelineCanvas.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2018
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/timeline/ruler-track.hpp"

//#include "stage/ui-bus.hpp"
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


namespace stage {
namespace timeline {
  
  namespace {
    uint TIMERULER_SCALE_HEIGHT_px = 20;
    uint TIMERULER_LARGE_TICK_WEIGHT_px = 2;
    uint TIMERULER_SMALL_TICK_WEIGHT_px = 2;
  }
  
  
  
  
  RulerTrack::RulerTrack()
    : scales_{}
    { }
  
  
  RulerTrack::~RulerTrack()
  {
    TODO ("detach from parent; store a functor or backreference");
  }
  
  
  /**
   * recursively calculate the height in pixels to display this track,
   * including all nested sub-tracks
   */
  uint
  RulerTrack::calcHeight()
  {
    UNIMPLEMENTED ("calculate display height of the overview ruler in pixels");
  }
  
  
  
}}// namespace stage::timeline
