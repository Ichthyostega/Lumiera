/*
  parent-track-.cpp  -  Implementation of the ParentTrack class

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


/** @file parent-track.cpp
 ** Preliminary UI-model: Implementation of ParentTrack.
 ** @warning as of 2016 this UI model is known to be a temporary workaround
 **          and will be replaced in entirety by UI-Bus and diff framework.
 ** @deprecated the existing timeline implementation will be completely rewritten
 */

#include "parent-track.hpp"
#include <boost/foreach.hpp>

using std::shared_ptr;

namespace stage {
namespace model {

ParentTrack::ParentTrack()
{
}

const std::list<shared_ptr<Track>>&
ParentTrack::get_child_tracks() const
{
  return tracks.get_list();
}

lumiera::observable_list<shared_ptr<Track>>&
ParentTrack::get_child_track_list()
{
  return tracks;
}

bool
ParentTrack::can_host_children() const
{
  return true;
}

bool
ParentTrack::remove_descendant_track(
  const shared_ptr<Track> track)
{
  REQUIRE(track);
  
  shared_ptr<ParentTrack> parent =
    find_descendant_track_parent(track);
  if(parent)
    {
      parent->tracks.remove(track);
      return true;
    }
    
  return false;
}

shared_ptr<ParentTrack>
ParentTrack::find_descendant_track_parent(shared_ptr<Track> child)
{ 
  using namespace boost;        ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

  REQUIRE(child != NULL);
  BOOST_FOREACH(std::shared_ptr<Track> track, tracks)  ///////TODO use a standard foreach loop
    {
      if(track == child)
        return shared_from_this();

      std::shared_ptr<ParentTrack> result =
        track->find_descendant_track_parent(child);
      if(result)
        return result;
    }
  
  return std::shared_ptr<ParentTrack>();
}

}   // namespace model
}   // namespace stage
