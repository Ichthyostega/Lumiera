/*
  SEQUENCE.hpp  -  GUI-model: Sequence

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
 ** The GUI-Model representation of an editable sequence.
 ** @todo Currently (1/2014) this is a dummy placeholder, but later on
 **       the GUI-Model entities will be connected to the Session model
 **       through the use of PlacementRef
 */



#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include "gui/model/parent-track.hpp"
#include "lib/hash-indexed.hpp"

#include <string>

namespace gui {
namespace model {

class Track;


/**
 * GUI-model representation of a sequence.
 * @remarks Sequence objects are also the roots of track trees.
 */
class Sequence
  : public ParentTrack
  {
    lib::hash::LuidH id_;
    
  public:
    Sequence()
      : id_()
      { 
        populateDummySequence();
      };
    
    operator lib::HashVal()  const
      {
        return id_;
      }
    
    /** human readable debug string representation of this track. */
    std::string print_track();
  
  private:
    void populateDummySequence();
  };


}   // namespace model
}   // namespace gui

#endif // SEQUENCE_HPP
