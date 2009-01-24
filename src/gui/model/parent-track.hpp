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

/**
 * ParentTrack is the abstract base class of all tracks that can parent
 * children.
 **/
class ParentTrack : public Track
{
protected:
  /**
   * Constructor
   **/
  ParentTrack();

public: 

  /**
   * Gets a read-only reference to the the list of child tracks.
   **/
  const std::list< boost::shared_ptr<Track> >&
    get_child_tracks() const;

  /**
   * Gets read-write access to the list of child tracks.
   **/
  lumiera::observable_list< boost::shared_ptr<Track> >&
    get_child_track_list();

  /**
   * Returns true if this track can own any child tracks.
   * @return Returns true because all classed derrived from ParentTrack
   * can.
   **/
  bool can_host_children() const;
  
  /**
   * Tries to remove a given track from amongst the descendants of this
   * track.
   * @param The model track to try and remove.
   * @return Returns true if the track was successfully removed.
   **/
  bool remove_descendant_track(const boost::shared_ptr<Track> track);

protected:
  /**
   * The internal list of child tracks of this paremt.
   **/
  lumiera::observable_list< boost::shared_ptr<Track> > tracks;
};

}   // namespace model
}   // namespace gui

#endif // PARENT_TRACK_HPP
