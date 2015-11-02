/*
  group-track.hpp  -  Declaration of the timeline group track object

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

/** @file widget/timeline/group-track.hpp
 ** This file contains the definition of group track timeline objects
 */


#ifndef GROUP_TRACK_HPP
#define GROUP_TRACK_HPP

#include "gui/model/parent-track.hpp"


namespace gui {
namespace model {

/**
 * A class representation of a grouping of tracks.
 */
class GroupTrack : public ParentTrack
{
public:
  /**
   * Constructor
   */
  GroupTrack();

  /**
   * Produces a human readable debug string representation of this
   * track.
   * @return Returns the human readable string.
   */
  std::string print_track();
};

}   // namespace model
}   // namespace gui

#endif // GROUP_TRACK_HPP
