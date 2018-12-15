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
 ** Timeline presentation helper to organise drawing of the time overview ruler.
 ** The scrollable body part of the timeline display relies on custom drawing onto
 ** a ["widget canvas"](\ref TimelineCanvas) for the nested track content; above
 ** this area we build a horizontal ruler to show the timecode and frame count
 ** references plus any markers, ranges and locators. Since these need to be
 ** aligned precisely with the content, we employ custom drawing for this
 ** part as well. The TimelineRuler helper -- like any parts of the coordinated
 ** TimelineLayout, are referred to from and used by the BodyCanvasWidget for
 ** offloading specific parts of the drawing routines.
 ** Actually, this space is just a working area and created by custom
 ** drawing on the [timeline canvas](\ref timeline::BodyCanvasWidget);
 ** yet for coordination of a globally consistent timeline layout, each
 ** track display is coordinated by a TrackPresenter, which corresponds
 ** to a session::Fork and directly controls the respective display elements
 ** in the [header pane](\ref timeline::HeaderPaneWidget) and the display of the
 ** timeline body, which is actually a canvas for custom drawing.
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
   * Helper to organise and draw the time overview ruler at the top of the
   * timeline BodyCanvasWidget. Support custom drawing onto the TimelineCanvas
   * to show the timecode or frame count ticks plus any markers, ranges and locators...
   * TrackBody units work together with the TimelineCanvas, which arranges all
   * elements placed into the tracks and performs custom drawing to mark the
   * working space available for placing those elements (Clips, Effects, Markers).
   * A given TrackBody works with coordinates relative to its vertical starting point;
   * coordinates on the TimelineCanvas operate from top downwards. The fundamental
   * task of a TrackBody is to find out about its own overall height, including the
   * overall height required by all its nested children. Moreover, the height of
   * the content area needs to be negotiated with the actual content elements.
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
