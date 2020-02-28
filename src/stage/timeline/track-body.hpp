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
 ** timeline body; the latter is actually a canvas for custom drawing.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#ifndef STAGE_TIMELINE_TRACK_BODY_H
#define STAGE_TIMELINE_TRACK_BODY_H

#include "stage/gtk-base.hpp"
#include "stage/timeline/ruler-track.hpp"
#include "stage/timeline/display-manager.hpp"

//#include "lib/util.hpp"

#include <memory>
#include <vector>
#include <array>



namespace stage  {
namespace timeline {
  
  class TrackPresenter;
  class TrackProfile;
  
  
  /**
   * Configure additional vertical padding for the decorations added through CSS.
   * Our drawing code retrieves the margin, padding and border size settings from the
   * appropriate CSS rules and adds the necessary additional space to the drawing coordinates;
   * however, since one purpose of TrackBody is to calculate overall space requirements, we also
   * need to account for this additional space. These common amounts are stored into a static
   * field and (re)configured when [establishing the track spacing](\ref TrackBody::establishTrackSpace).
   */
  struct Decoration
    {
      uint content = 0;
      uint ruler  = 0;
      uint topMar = 0;
      uint botMar = 0;
      uint trackPad = 0;
      
      using Borders = std::array<uint, 6>;
      Borders borders{0,0,0,0,0,0};    ///< width of up to 6 levels of combined upward slope borders (defined in CSS)
    };
  
  
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
   * @todo WIP-WIP as of 6/2019
   */
  class TrackBody
    : public model::ViewHook<TrackBody>
    {
      uint contentHeight_;
      uint contentOffset_;
      uint startLine_;
      
      using PRuler    = std::unique_ptr<RulerTrack>;
      using Rulers    = std::vector<PRuler>;
      
      using SubTracks = std::vector<TrackBody*>;
      
      SubTracks subTracks_;
      Rulers    rulers_;
      
    public:
      static Decoration decoration;
      
      TrackBody();
     ~TrackBody();
      
      void setTrackName (cuString&);
      uint establishTrackSpace (TrackProfile&);
      uint calcRulerHeight();
      uint calcHeight();
      
      uint getContentOffsetY() { return startLine_ + contentOffset_; }
      
      DisplayManager::SignalStructureChange signalStructureChange_;
      
      
      /* ==== Interface: ViewHook ===== */
      
      void hook (TrackBody&, int xPos=0, int yPos=0) override;
      void move (TrackBody&, int xPos, int yPos)     override;
      void remove (TrackBody&)                       override;
      void rehook (TrackBody&) noexcept              override;
      
      /* ===== Internals ===== */
      /**
       * @internal Allow the TrackPresenter to manage the rulers
       * The collection of rulers is part of the systematic UI model
       * and thus formally a direct child of the TrackPresenter; however they
       * are only relevant for the immediate display and interaction mechanics,
       * thus we store them right here, close to usage site.
       * @note Ruler entries can be added and removed by diff message, but since
       *       the UI is performed single threaded, these mutations never interfere
       *       with display evaluation passes.
       */ 
      Rulers&
      bindRulers()
        {
          return rulers_;
        }
      
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_TRACK_BODY_H*/
