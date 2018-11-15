/*
  clip.cpp  -  Implementation of the Clip object

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


/** @file gui/model/clip.cpp
 ** Preliminary UI-model: implementation of a Clip object as placeholder to
 ** base the GUI implementation on.
 ** @warning as of 2016 this UI model is known to be a temporary workaround
 **          and will be replaced in entirety by UI-Bus and diff framework.
 */


#include "stage/model/clip.hpp"
#include "lib/time/mutation.hpp"

using lib::time::FSecs;
using lib::time::Mutation;


namespace stage {
namespace model {

  Clip::Clip()
    : timeCoord_(Time(FSecs(1)), FSecs(3))
  {  }
  
  
  void
  Clip::setBegin (Time newStartTime)
  {
    timeCoord_.accept (Mutation::changeTime (newStartTime));
    TODO("Emit A Signal");
  }

  void
  Clip::setDuration (Duration newLength)
  {
    timeCoord_.accept (Mutation::changeDuration(newLength));
    TODO("Emit A Signal");
  }

  void
  Clip::setName (string const& name)
  {
    this->name_ = name;
    nameChangedSignal_.emit (name);
  }

  sigc::signal<void, std::string>
  Clip::signalNameChanged() const
  {
    return nameChangedSignal_;
  }

}   // namespace model
}   // namespace stage

