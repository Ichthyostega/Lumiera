/*
  parent-track.hpp  -  Definition of the ParentTrack class

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
/** @file parent-track.hpp
 ** Preliminary UI-model: Definition of ParentTrack, a class which
 ** represents a base functionality for tracks and sequences which
 ** are also track parents. This class was intended to wraps steam layer data.
 ** @warning as of 2016 this UI model is known to be a temporary workaround
 **          and will be replaced in entirety by UI-Bus and diff framework.
 ** @deprecated the existing timeline implementation will be completely rewritten
 */

#ifndef PARENT_TRACK_HPP
#define PARENT_TRACK_HPP

#include "gui/model/track.hpp"
#include "lib/observable-list.hpp"

namespace gui {
namespace model {

/**
 * ParentTrack is the abstract base class of all tracks that can parent children.
 * @deprecated the existing timeline implementation will be rewritten
 */
class ParentTrack :
  public Track,
  public std::enable_shared_from_this<ParentTrack>
{
protected:
  /**
   * Constructor
   */
  ParentTrack();

public: 

  /**
   * Gets a read-only reference to the the list of child tracks.
   */
  const list<shared_ptr<Track>>&
    get_child_tracks() const;

  /**
   * Gets read-write access to the list of child tracks.
   */
  lumiera::observable_list<shared_ptr<Track>>&
    get_child_track_list();

  /**
   * Returns true if this track can own any child tracks.
   * @return Returns true because all classed derrived from ParentTrack
   * can.
   */
  bool can_host_children() const;
  
  /**
   * Tries to remove a given track from amongst the descendants of this
   * track.
   * @param The model track to try and remove.
   * @return Returns true if the track was successfully removed.
   */
  bool remove_descendant_track (const shared_ptr<Track> track);
  
  /**
   * A utility function that attempts to find the parent of a track by
   * searching through the tree from this track downward.
   * @param child The child track to find the parent of.
   * @return Returns the parent track if one was found, or an empty
   * shared_ptr if none was found.
   */
  shared_ptr<ParentTrack>
  find_descendant_track_parent (shared_ptr<Track> child);

protected:
  /**
   * The internal list of child tracks of this parent.
   */
  lumiera::observable_list<shared_ptr<Track>> tracks;
};

}   // namespace model
}   // namespace gui

#endif // PARENT_TRACK_HPP
