/*
  track.hpp  -  Definition of the Track class
 
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
/** @file track.hpp
 ** This file contains the definition of Track, a class which
 ** represents a track, and wraps proc layer data
 */

#include "../gtk-lumiera.hpp"

#ifndef TRACK_HPP
#define TRACK_HPP

namespace gui {
namespace model {
  
class ParentTrack;

/**
 * The model representation of a track. This is the base class for all
 * types of track that are implemented.
 **/
class Track
{
protected:
  /**
   * Constructor
   **/
  Track();

public:

  /**
   * Gets the list of child tracks.
   **/
  virtual std::list< boost::shared_ptr<Track> >
    get_child_tracks() const;
  
  /**
   * Gets the name of this track.
   **/
  const std::string get_name() const;
  
  /**
   * Sets the name of this track.
   * @param[in] name The new name to set this track to.
   **/
  void set_name(const std::string &name);
  
  /**
   * Returns true if this track can own any child tracks.
   **/
  virtual bool can_host_children() const;
  
  /**
   * Tries to remove a given child track from the list.
   * @param The model track to try and remove.
   * @return Returns true if the track was successfully removed.
   **/
  virtual bool remove_child_track(const boost::shared_ptr<Track> track);
  
  /**
   * A debugging helper function that prints this track, and all it's
   * child tracks in a human-readable form.
   * @return Returns the human readable string.
   **/
  std::string print_branch();
  
  /**
   * A pure-virtual function which is the base of functions that print
   * this track in human readable form.
   * @return Returns the human readable string.
   **/
  virtual std::string print_track() = 0;
  
public:
  
  /**
   * A utility function that attempts to find the parent of a track by
   * searching through the tree from a root downward.
   * @param root The root track to begin searching down from.
   * @param child The child track to find the parent of.
   * @return Returns the parent track if one was found, or an empty
   * shared_ptr if none was found.
   **/
  static boost::shared_ptr<ParentTrack>
    find_parent(boost::shared_ptr<ParentTrack> root,
      boost::shared_ptr<Track> child);
  
protected:
  /**
   * The internal implementation of print_branch.
   * @param indentation The level of recursion into the tree. This value
   * is used to specify the width of indentation to print with.
   * @return Returns the human readable string.
   **/
  std::string print_branch_recursive(const unsigned int indentation);

private:
  //----- Data -----//
  /**
   * The name of this track.
   **/
  std::string name;

protected:
  /**
   * An object used internally as a return value for when there's no
   * children.
   **/
  static const std::list< boost::shared_ptr<Track> > NoChildren;
};

}   // namespace model
}   // namespace gui

#endif // TRACK_HPP
