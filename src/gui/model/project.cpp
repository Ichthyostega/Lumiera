/*
  project.cpp  -  Implementation of the Project class

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


#include "gui/model/project.hpp"
#include "gui/model/sequence.hpp"

#include <memory>


namespace gui {
namespace model {

Project::Project()
{
  // TEST CODE
  add_new_sequence("Sequence A");
  add_new_sequence("Sequence B");
}

Project::~Project()
{

}

lumiera::observable_list< shared_ptr<Sequence> >&
Project::get_sequences()
{
  return sequences;
}

void
Project::add_new_sequence(uString name)
{
  std::shared_ptr<Sequence> sequence(new Sequence());
  sequence->set_name(name);
  sequences.push_back(sequence);
}

}   // namespace model
}   // namespace gui
