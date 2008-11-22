/*
  sequence.hpp  -  Implementation of the Sequence class
 
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
 
* *****************************************************/

#include "sequence.hpp"

namespace gui {
namespace model {

Sequence::Sequence()
{
  
}

const Glib::ustring
Sequence::get_name() const
{
  return name;
}
  
void
Sequence::set_name(const Glib::ustring &name)
{
  this->name = name;
}

}   // namespace model
}   // namespace gui
