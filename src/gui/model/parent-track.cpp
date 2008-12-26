/*
  parent-track-.cpp  -  Implementation of the ParentTrack class
 
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

#include "parent-track.hpp"

namespace gui {
namespace model {

ParentTrack::ParentTrack()
{
}

std::list< boost::shared_ptr<Track> >
ParentTrack::get_child_tracks() const
{
  return tracks.to_list();
}

lumiera::observable_list< boost::shared_ptr<Track> >&
ParentTrack::get_child_track_list()
{
  return tracks;
}

}   // namespace model
}   // namespace gui
