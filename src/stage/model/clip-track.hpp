/*
  clip-track.hpp  -  Declaration of a clip containing track object

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>

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

/**
 ** @file model/clip-track.hpp
 ** Preliminary UI-model: Definition of timeline track objects which contain clips.
 ** @warning as of 2016 this UI model is known to be a temporary workaround
 **          and will be replaced in entirety by UI-Bus and diff framework.
 */


#ifndef CLIP_TRACK_HPP
#define CLIP_TRACK_HPP

#include "gui/model/track.hpp"
#include "lib/observable-list.hpp"

namespace gui {
namespace model {

  class Clip;

  class ClipTrack : public Track
  {
  public:
    /**
     * Constructor
     */
    ClipTrack();

    /**
     * Gets a string representation of the track that is suitable for debugging
     */
    std::string
    print_track();

    /**
     * Gets the list of clips associated with this track.
     */
    lumiera::observable_list<shared_ptr<Clip>>&
    getClipList(void);

  private:

    lumiera::observable_list<shared_ptr<Clip>> clips;

  };

}   // namespace timeline
}   // namespace gui

#endif // CLIP_TRACK_HPP
