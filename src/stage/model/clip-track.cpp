/*
  clip-track.cpp  -  Implementation of a clip containing track object

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

* *****************************************************/


/** @file clip-track.cpp
 ** Preliminary UI-model: Implementation of timeline track model object.
 ** @warning as of 2016 this UI model is known to be a temporary workaround
 **          and will be replaced in entirety by UI-Bus and diff framework.
 */

#include "clip-track.hpp"

#include "clip.hpp"


namespace stage {
namespace model {

  ClipTrack::ClipTrack()
  {
    // TEST CODE: add a clip to the track

    shared_ptr<model::Clip> modelClip(new model::Clip());
    modelClip->setName("Clip Name");
    clips.push_back(modelClip);

    // END TEST CODE
  }

  std::string
  ClipTrack::print_track()
  {
    std::ostringstream os;

    os << "ClipTrack\t\"" << get_name() << "\"";

    return os.str();
  }

  lumiera::observable_list<shared_ptr<Clip>>&
  ClipTrack::getClipList()
  {
    return clips;
  }

}   // namespace model
}   // namespace stage
