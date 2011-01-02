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

#include "clip.hpp"

namespace gui {
namespace model {

  Clip::Clip()
    : begin(1000000),
      end(2000000)
  {  }

  gavl_time_t
  Clip::getBegin() const
  {
    return begin;
  }

  gavl_time_t
  Clip::getEnd() const
  {
    return end;
  }

  const std::string
  Clip::getName() const
  {
    return name;
  }

  bool
  Clip::isPlayingAt(lumiera::Time position) const
  {
    return (begin <= position && end >= position);
  }

  void
  Clip::setBegin(gavl_time_t begin)
  {
    this->begin = begin;
    // TODO: emit signal
  }

  void
  Clip::setEnd(gavl_time_t end)
  {
    this->end = end;
    // TODO: emit signal
  }

  void
  Clip::setName(const std::string &name)
  {
    this->name = name;
    nameChangedSignal.emit(name);
  }

  sigc::signal<void, std::string>
  Clip::signalNameChanged() const
  {
    return nameChangedSignal;
  }

}   // namespace model
}   // namespace gui

