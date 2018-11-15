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

/** @file gui/model/group-track.hpp
 ** Preliminary UI-model: Definition of group track timeline objects.
 ** @warning as of 2016 this UI model is known to be a temporary workaround
 **          and will be replaced in entirety by UI-Bus and diff framework.
 ** @deprecated the existing timeline implementation will be completely rewritten
 */


#ifndef GROUP_TRACK_HPP
#define GROUP_TRACK_HPP

#include "stage/model/parent-track.hpp"


namespace stage {
namespace model {

/**
 * A class representation of a grouping of tracks.
 * @deprecated the existing timeline implementation will be rewritten
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
}   // namespace stage

#endif // GROUP_TRACK_HPP
