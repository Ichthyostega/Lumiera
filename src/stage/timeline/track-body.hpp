/*
  TRACK-BODY.hpp  -  track body area within the timeline display canvas

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

*/


/** @file track-body.hpp
 ** This helper class serves to manage the layout and display of the
 ** horizontally extended space of a "track" within the timeline.
 ** Actually, this space is just a working area and created by custom
 ** drawing on the [timeline canvas](\ref timeline::BodyCanvasWidget);
 ** yet for coordination of a globally consistent timeline layout, each
 ** track display is coordinated by a TrackPresenter, which corresponds
 ** to a session::Fork and directly controls the respective display elements
 ** in the [header pane](\ref timeline::HeaderPaneWidget) and the display of the
 ** timeline body, which is actually a canvas for custom drawing.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#ifndef STAGE_TIMELINE_TRACK_BODY_H
#define STAGE_TIMELINE_TRACK_BODY_H

#include "stage/gtk-base.hpp"
#include "stage/ctrl/bus-term.hpp"

//#include "lib/util.hpp"

//#include <memory>
#include <vector>



namespace stage  {
namespace timeline {
  
  using ID = ctrl::BusTerm::ID;
  
  
  /**
   * Helper to organise and draw the space allocated for a fork of sub-tracks.
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
  class TrackBody
    {
      uint overviewHeight_;
      uint contentHeight_;
      
      using SubTracks = std::vector<TrackBody*>;
      
      SubTracks subTracks_;
      
    public:
      TrackBody();
     ~TrackBody();
      
      void setTrackName (cuString&);
      
      uint calcHeight();
      
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_TRACK_BODY_H*/
