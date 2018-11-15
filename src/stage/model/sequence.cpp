/*
  Sequence  -  GUI-model: Sequence

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


/** @file gui/model/sequence.cpp
 ** Preliminary UI-model: implementation of an editable sequence.
 ** @warning as of 2016 this UI model is known to be a temporary workaround
 **          and will be replaced in entirety by UI-Bus and diff framework.
 */

#include "stage/model/sequence.hpp"
#include "include/logging.h"

#include <memory>

// TEST CODE
#include "stage/model/group-track.hpp"
#include "stage/model/clip-track.hpp"


namespace gui {
namespace model {


void
Sequence::populateDummySequence()
{
  // TEST CODE
  static bool first = true;
  
  std::shared_ptr<GroupTrack> group_track, group_track2;
  std::shared_ptr<ClipTrack> clip_track;
  tracks.push_back(group_track = shared_ptr<GroupTrack>(new GroupTrack()));
  group_track->set_name("Group Track");
    
  if(first)
  {
    group_track->get_child_track_list().push_back(
      clip_track = shared_ptr<ClipTrack>(new ClipTrack()));
    clip_track->set_name("Clip Track 1");
    group_track->get_child_track_list().push_back(
      group_track2 = shared_ptr<GroupTrack>(new GroupTrack()));
    group_track2->set_name("Group Track 2");
    group_track2->get_child_track_list().push_back(
      clip_track = shared_ptr<ClipTrack>(new ClipTrack()));
    clip_track->set_name("Clip Track 2");
    first = false;
  }
  
  tracks.push_back(group_track = shared_ptr<GroupTrack>(new GroupTrack()));
  group_track->set_name("Group Track 3");
  
  tracks.push_back(clip_track = shared_ptr<ClipTrack>(new ClipTrack()));
  clip_track->set_name("Clip Track 3");
  
  // END TEST CODE
  
  INFO(gui, "\n%s", print_branch().c_str());
}


std::string
Sequence::print_track()
{
  return "Sequence\t\"" + get_name() + "\"";
}

}   // namespace model
}   // namespace gui
