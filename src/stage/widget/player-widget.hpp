/*
  PLAYER-WIDGET.hpp  -  display of a playback control element

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


/** @file player-widget.hpp
 ** Widget to render the typical playback control element in the UI.
 ** When a running playback process is created within the core and a
 ** _viewer display connection_ is established, a viewer, some timeline
 ** and a player control entity are collaborating. The latter is the
 ** typical setup of play, stop, jump and fast forward / fast backward
 ** buttons, and in addition it allows to control loop playback and
 ** single stepping. Of course, there is also a key binding to control
 ** these operations, and in addition, Lumiera may be connected with
 ** some hardware control surface (TODO obviously we need that, but
 ** as of 12/2016 we don't even know remotely how to implement it).
 ** Typically, the PlayerWidget is also combined with a timecode
 ** display, and when several player widgets are connected to the
 ** same playback process, their UI controls are "ganged together".
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016 -- the initial version of our
 **       UI implemented these controls directly within the timeline
 **       pane's button bar. We need to extract this common setup,
 **       since it is consensus that there is no consensus regarding
 **       the "best" position of such a control within the UI. So
 **       we need to offer the option to have "a player" either in
 **       the timeline, in the viewer(s) or as a standalone UI element.
 ** 
 */


#ifndef STAGE_WIDGET_PLAYER_WIDGET_H
#define STAGE_WIDGET_PLAYER_WIDGET_H

#include "stage/gtk-base.hpp"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>



namespace stage  {
namespace widget {
  
  
  /**
   * @todo WIP-WIP as of 12/2016
   */
  class PlayerWidget
    {
    public:
      PlayerWidget ();
     ~PlayerWidget();
     
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace stage::widget
#endif /*STAGE_WIDGET_PLAYER_WIDGET_H*/
