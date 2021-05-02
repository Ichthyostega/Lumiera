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
 ** @todo WIP-WIP-WIP as of 6/2019
 ** @todo this is more or less preliminary/placeholder code as of 4/2019
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/style-scheme.hpp"  /////////////////////TODO needed?
#include "stage/timeline/ruler-track.hpp"

//#include "stage/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>


using lib::diff::TreeMutator;

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
    const uint DEFAULT_OVERVIEW_HEIGHT_px = 20;
    
    const uint TIMERULER_SCALE_HEIGHT_px = 20;
    const uint TIMERULER_LARGE_TICK_WEIGHT_px = 2;
    const uint TIMERULER_SMALL_TICK_WEIGHT_px = 2;
    const uint TIMERULE_GAP_BELOW_px = 5;
  }
  
  
  
  
  RulerTrack::RulerTrack (ID id, ctrl::BusTerm& nexus, TrackPresenter& parent)
    : Controller{id, nexus}
    , track_{parent}
    , scales_ {}
    { }
  
  
  RulerTrack::~RulerTrack() { }
  
  void
  RulerTrack::buildMutator (TreeMutator::Handle buffer)
  {
    UNIMPLEMENTED ("what can actually be manipulated on a RulerTrack by diff message?");
    
//  buffer.emplace(
//    TreeMutator::build()
//      .change(ATTR_name, [&](string val)
//          {                                                  // »Attribute Setter« : receive a new value for the track name field
//            this->setTrackName (val);
//          }));
  }
  
  
  /**
   * recursively calculate the height in pixels to display this ruler track.
   * @todo this is more or less braindead placeholder code as of 4/2019
   */
  uint
  RulerTrack::calcHeight()
  {
    if (isnil (scales_))
      return DEFAULT_OVERVIEW_HEIGHT_px;
    else 
      { // handle time ruler                                 ////////////////////////////////////////////////TICKET #1194 : proper handling of ruler tracks
        uint combinedScaleHeight = 0;
        for (auto& scale : scales_)
          combinedScaleHeight += scale.calcHeight();
        return combinedScaleHeight;
      }
  }
  
  /**
   * possibly cause the display of an additional padding gap below this ruler track
   * @return `0` when no gap shall be displayed, _otherwise_ the gap height in pixels.
   * @todo this is more or less braindead placeholder code as of 4/2019
   */
  uint
  RulerTrack::getGapHeight()
    {
      if (not isnil (scales_))
        return TIMERULE_GAP_BELOW_px;
      else
        return 0; // no Gap
      
    }
  
  /**
   * get vertical extension of this scale on the time(code) overview ruler
   * @todo this is more or less braindead placeholder code as of 4/2019
   */
  uint
  RulerScale::calcHeight()
    {                                                        ////////////////////////////////////////////////TICKET #1194 : proper handling of ruler tracks
      return TIMERULER_SCALE_HEIGHT_px;
    }
  
  
  
}}// namespace stage::timeline
