/*
  track.cpp  -  Implementation of the Track class

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


/** @file track.cpp
 ** Preliminary UI-model: Implementation for the track timeline objects.
 ** @warning as of 2016 this UI model is known to be a temporary workaround
 **          and will be replaced in entirety by UI-Bus and diff framework.
 ** @deprecated the existing timeline implementation will be completely rewritten
 */


#include "stage/model/track.hpp"
#include "stage/model/parent-track.hpp"

#include <boost/foreach.hpp>
#include <memory>
#include <string>
#include <list>

namespace stage {
namespace model {
  
using std::list;
using std::string;
using std::shared_ptr;

const list< shared_ptr<Track> > Track::NoChildren;


Track::Track()
  : enabled(true),
    locked(false)
{ }

Track::~Track() { }


const list< shared_ptr<Track> >&
Track::get_child_tracks() const
{
  return Track::NoChildren;
}

bool
Track::getEnabled() const
{
  return enabled;
}

bool
Track::getLocked() const
{
  return locked;
}

const string
Track::get_name() const
{
  return name;
}

void
Track::setEnabled(bool enabled)
{
  this->enabled = enabled;
  enabledChangedSignal.emit(enabled);
}

void
Track::setLocked(bool locked)
{
  this->locked = locked;
  lockedChangedSignal.emit(locked);
}

void
Track::set_name(const string &name)
{
  this->name = name;
  nameChangedSignal.emit(name);
}

bool
Track::can_host_children() const
{
  return false;
}

string
Track::print_branch()
{
  return print_branch_recursive(0);
}

shared_ptr<ParentTrack>
Track::find_descendant_track_parent (shared_ptr<Track>)
{ 
  return shared_ptr<ParentTrack>();
}

sigc::signal<void, bool>
Track::signalEnabledChanged() const
{
  return enabledChangedSignal;
}

sigc::signal<void, bool>
Track::signalLockedChanged() const
{
  return lockedChangedSignal;
}

sigc::signal<void, std::string>
Track::signalNameChanged() const
{
  return nameChangedSignal;
}

string
Track::print_branch_recursive (const unsigned int indentation)
{
  string str;
  
  for(unsigned int i = 0; i < indentation; i++)
    str += "  ";
  str += print_track();
  str += "\n";
  
  BOOST_FOREACH(shared_ptr<Track> track, get_child_tracks())
    {
      REQUIRE(track);
      str += track->print_branch_recursive(indentation + 1);
    }
  
  return str;
}

}   // namespace model
}   // namespace stage
