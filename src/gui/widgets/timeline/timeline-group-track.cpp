/*
  timeline-group-track.cpp  -  Implementation of the timeline group track object
 
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

#include "timeline-group-track.hpp"

using namespace Gtk;

namespace gui {
namespace widgets {
namespace timeline {
  
GroupTrack::GroupTrack()
{
}
  
void
GroupTrack::add_child_track(Track* child)
{
  REQUIRE(child != NULL);
  children.push_back(child);
}

const std::vector<Track*>&
GroupTrack::get_child_tracks() const
{
  return children;
}

void
GroupTrack::draw_track(Cairo::RefPtr<Cairo::Context> cairo,
    TimelineViewWindow* const window) const
{
  
}
  
}   // namespace timeline
}   // namespace widgets
}   // namespace gui
