/*
  track.hpp  -  Definition of the Track class

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
/** @file track.hpp
 ** Preliminary UI-model: Definition of Track, a class which
 ** represents a track, and wraps steam layer data
 ** @warning as of 2016 this UI model is known to be a temporary workaround
 **          and will be replaced in entirety by UI-Bus and diff framework.
 ** @deprecated the existing timeline implementation will be completely rewritten
 */


#include "stage/gtk-base.hpp"

#include <memory>
#include <list>

#ifndef TRACK_HPP
#define TRACK_HPP

namespace stage {
namespace model {
  
using std::shared_ptr;
using std::list;
  
class ParentTrack;

/**
 * The model representation of a track. This is the abstract base class
 * for all types of track that are implemented.
 */
class Track
{
protected:
  /**
   * Constructor
   */
  Track();

public:
  virtual ~Track(); /// this is an interface

  /**
   * Returns true if this track can own any child tracks.
   */
  virtual bool
  can_host_children () const;

  /**
   * Gets the list of child tracks.
   */
  virtual const list<shared_ptr<Track>>&
  get_child_tracks () const;
  
  /**
   * Gets the enabled status of this track, i.e. if the track is to be rendered.
   */
  bool
  getEnabled () const;

  /**
   * Gets the locked status of this track, i.e. if the track can be edited.
   */
  bool
  getLocked () const;

  /**
   * Gets the name of this track.
   */
  const std::string
  get_name () const;

  /**
   * Sets the enabled status of this track, i.e. if the track is to be rendered.
   * @param[in] name The new enabled status.
   */
  void
  setEnabled (bool enabled);

  /**
   * Gets the locked status of this track, i.e. if the track can be edited.
   * @param[in] name The new locked status.
   */
  void
  setLocked (bool locked);

  /**
   * Sets the name of this track.
   * @param[in] name The new name to set this track to.
   */
  void
  set_name (const std::string &name);
      
  /**
   * A utility function that attempts to find the parent of a track by
   * searching through the tree from this track downward.
   * @param child The child track to find the parent of.
   * @return Returns the parent track if one was found, or an empty
   * shared_ptr if none was found.
   */
  virtual shared_ptr<ParentTrack>
  find_descendant_track_parent (shared_ptr<Track> child);

  /**
   * A signal which fires when the enabled status changes.
   * @return Returns the signal. The signal sends the new name for the
   * track.
   */
  sigc::signal<void, bool>
  signalEnabledChanged () const;

  /**
   * A signal which fires when the locked status changes changes.
   * @return Returns the signal. The signal sends the new name for the
   * track.
   */
  sigc::signal<void, bool>
  signalLockedChanged () const;

  /**
   * A signal which fires when the name changes.
   * @return Returns the signal. The signal sends the new name for the
   * track.
   */
  sigc::signal<void, std::string>
  signalNameChanged () const;

  /**
   * A debugging helper function that prints this track, and all it's
   * child tracks in a human-readable form.
   * @return Returns the human readable string.
   */
  std::string
  print_branch ();
  
  /**
   * A pure-virtual function which is the base of functions that print
   * this track in human readable form.
   * @return Returns the human readable string.
   */
  virtual std::string
  print_track () = 0;
   
protected:
  /**
   * An object used internally as a return value for when there's no
   * children.
   */
  static const std::list< shared_ptr<Track> > NoChildren;

  /**
   * The internal implementation of print_branch.
   * @param indentation The level of recursion into the tree. This value
   * is used to specify the width of indentation to print with.
   * @return Returns the human readable string.
   */
  std::string
  print_branch_recursive (const unsigned int indentation);

private:
  /**
   * The name of this track.
   */
  std::string name;

  /**
   * True if this track is enabled, i.e. will not be rendered.
   */
  bool enabled;

  /**
   * True if this track is locked, i.e. cannot be edited.
   */
  bool locked;

  /**
   * A signal which fires when the enabled status changes.
   */
  sigc::signal<void, bool> enabledChangedSignal;

  /**
   * A signal which fires when the locked status changes.
   */
  sigc::signal<void, bool> lockedChangedSignal;

  /**
   * A signal which fires when the name changes.
   */
  sigc::signal<void, std::string> nameChangedSignal;
};

}   // namespace model
}   // namespace stage

#endif // TRACK_HPP
