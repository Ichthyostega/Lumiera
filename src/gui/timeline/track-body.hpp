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
 ** in the [header pane](timeline::HeaderPaneWidget) and the display of the
 ** timeline body, which is actually a canvas for custom drawing.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#ifndef GUI_TIMELINE_TRACK_BODY_H
#define GUI_TIMELINE_TRACK_BODY_H

#include "gui/gtk-base.hpp"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>



namespace gui  {
namespace timeline {
  
  
  /**
   * @todo WIP-WIP as of 12/2016
   */
  class TrackBody
    {
    public:
      TrackBody ();
     ~TrackBody();
     
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace gui::timeline
#endif /*GUI_TIMELINE_TRACK_BODY_H*/
