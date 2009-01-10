/*
  sequence.hpp  -  Implementation of the Sequence class
 
  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/

#include "sequence.hpp"

using namespace boost;

namespace gui {
namespace model {

Sequence::Sequence()
{
  // TEST CODE
  static bool first = true;
  
  shared_ptr<GroupTrack> group_track, group_track2;
  shared_ptr<ClipTrack> clip_track;
  tracks.push_back(group_track = shared_ptr<GroupTrack>(new GroupTrack()));
  group_track->set_name("Group Track");
    
  if(first)
  {
    group_track->get_child_track_list().push_back(
      clip_track = shared_ptr<ClipTrack>(new ClipTrack()));
    group_track->get_child_track_list().push_back(
      group_track2 = shared_ptr<GroupTrack>(new GroupTrack()));
    group_track2->set_name("Group Track 2");
    group_track2->get_child_track_list().push_back(
      shared_ptr<ClipTrack>(new ClipTrack()));
    first = false;
  }
  
  tracks.push_back(shared_ptr<GroupTrack>(new GroupTrack()));
  
  tracks.push_back(shared_ptr<Track>(new ClipTrack()));
  
  // END TEST CODE
  
  INFO(gui, "\n%s", print_branch().c_str());
}

std::string
Sequence::print_track()
{
  std::ostringstream os;
  
  os << "Sequence\t\"" << get_name() << "\"";
  
  return os.str();
}

}   // namespace model
}   // namespace gui
