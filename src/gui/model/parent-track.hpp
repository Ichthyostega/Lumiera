/*
  parent-track.hpp  -  Definition of the ParentTrack class
 
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
 
*/
/** @file parent-track.hpp
 ** This file contains the definition of ParentTrack, a class which
 ** represents a base functionality for tracks and sequences which
 ** are also track parents. This class wraps proc layer data
 */

#include "track.hpp"
#include "lib/observable-list.hpp"

#ifndef PARENT_TRACK_HPP
#define PARENT_TRACK_HPP

namespace gui {
namespace model {

class ParentTrack : public Track
{
protected:
  ParentTrack();

public:
  virtual void add_child_track(Track* child)
    { (void)child; };
  
  std::list< boost::shared_ptr<Track> >
    get_child_tracks() const;

  lumiera::observable_list< boost::shared_ptr<Track> >&
    get_child_track_list();

protected:
  lumiera::observable_list< boost::shared_ptr<Track> > tracks;
};

}   // namespace model
}   // namespace gui

#endif // PARENT_TRACK_HPP
