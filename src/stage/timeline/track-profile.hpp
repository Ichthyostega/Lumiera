/*
  TRACK-PROFILE.hpp  -  building the 3D profile of tracks for timeline presentation

  Copyright (C)         Lumiera.org
    2019,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file track-profile.hpp
 ** Abstraction build the layout for the track spaces for timeline display.
 ** In Lumiera, tracks are arranged into a fork of nested shapes, which structure
 ** is parallelled into nested structure of TrackBody elements. A tree walk over
 ** this structure yields a sequence of adjacent timeline elements, like overview
 ** rulers, content area and nested child track display. This sequence can then
 ** be transformed into suitable drawing instructions to create a 3D shaded
 ** display, clearly highlighting the complex structure of the track arrangement.
 ** 
 ** @todo WIP-WIP-WIP as of 4/2019
 ** 
 */


#ifndef STAGE_TIMELINE_TRACK_PROFILE_H
#define STAGE_TIMELINE_TRACK_PROFILE_H

#include "stage/gtk-base.hpp"
#include "lib/verb-token.hpp"

//#include "lib/util.hpp"

//#include <memory>
#include <vector>



namespace stage  {
namespace timeline {
  
  
  /**
   * Description of the structure and arrangement of tracks for display in the UI.
   * This sequence of verb tokens especially details the _profile_ of a vertical
   * cross-section; the nested structure of the track fork is translated into
   * a series of steps and insets, running alongside the timeline display.
   * To decouple the drawing code -- thus allowing for later customisations --
   * we let the individual TrackBody elements just emit these structure description.
   * @todo WIP-WIP as of 4/2019
   */
  class TrackProfile
    {
      
    public:
      
      
    private:/* ===== Internals ===== */
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_TRACK_PROFILE_H*/
