/*
  RULER-TRACK.hpp  -  track body area to show overview and timecode and markers

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

*/


/** @file ruler-track.hpp
 ** Timeline presentation helper to organise drawing of the overview rulers.
 ** The scrollable body part of the timeline display relies on custom drawing onto
 ** a ["widget canvas"](\ref TimelineCanvas) for the nested track content; above
 ** this track content area we build a horizontal ruler to show the timecode or
 ** frame count references plus any markers, ranges and locators. In case of
 ** group tracks or collapsed tracks, a synopsis of the content may be rendered
 ** into this overview bar. Since any of these display elements need to be aligned
 ** precisely with the content, we employ custom drawing for the rulers as well.
 ** The RulerTrack -- like any parts of the coordinated TimelineLayout, will be
 ** referred to from and used by the BodyCanvasWidget for offloading specific
 ** parts of the drawing routines.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2018
 ** 
 */


#ifndef STAGE_TIMELINE_RULER_TRACK_H
#define STAGE_TIMELINE_RULER_TRACK_H

#include "stage/gtk-base.hpp"

//#include "lib/util.hpp"

//#include <memory>
#include <vector>



namespace stage  {
namespace timeline {
  
  struct RulerScale
    {
      
    };
  
  
  /**
   * Helper to organise and draw the time or content overview ruler at the top of the
   * timeline track display. The purpose is to support custom drawing onto the TimelineCanvas
   * to show rendered content, timecode or frame count ticks plus any markers, ranges and locators...
   * @todo WIP-WIP as of 10/2018
   */
  class RulerTrack
    {
      using Scales = std::vector<RulerScale>;
      
      Scales scales_;
      
    public:
      RulerTrack();
     ~RulerTrack();
      
      uint calcHeight();
      
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_RULER_TRACK_H*/