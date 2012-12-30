/*
  sequence.hpp  -  Definition of the Sequence class

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
/** @file sequence.hpp
 ** This file contains the definition of Sequence, a class which
 ** contains a 
 */

#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include "gui/model/parent-track.hpp"

namespace gui {
namespace model {
  
class Track;

/**
 * A class representation of a sequence.
 * @remarks Sequence objects are also the roots of track trees.
 */
class Sequence : public ParentTrack
{
public:
  /**
   * Constructor
   */
  Sequence();
  
  /**
   * Produces a human readable debug string representation of this
   * track.
   * @return Returns the human readable string.
   */
  std::string print_track();
};

}   // namespace model
}   // namespace gui

#endif // SEQUENCE_HPP
