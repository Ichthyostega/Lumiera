/*
  project.hpp  -  Definition of the Project class

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

/** @file project.hpp
 ** Preliminary UI-model: Definition of Project, a class which
 ** stores project data, and wraps steam layer data.
 ** @warning as of 2016 this UI model is known to be a temporary workaround
 **          and will be replaced in entirety by UI-Bus and diff framework.
 ** 
 ** @todo this solution was used to build the first outline of the UI.
 **       It needs to be reshaped into a connector to Steam-Layer  ///////////////TICKET #959
 */

#ifndef PROJECT_HPP
#define PROJECT_HPP


#include "gui/model/sequence.hpp"
#include "lib/observable-list.hpp"

namespace gui {
namespace model {
  
/**
 * @deprecated 1/2015 see TICKET #959
 */
class Project
{
public:
  Project();
  
  ~Project();
    
  lumiera::observable_list<shared_ptr<Sequence> >&
    get_sequences();
  
  void add_new_sequence(uString name);
  
private:

  lumiera::observable_list<shared_ptr<Sequence> > sequences;
};

}   // namespace model
}   // namespace gui

#endif // PROJECT_HPP
