/*
  project.hpp  -  Definition of the Project class
 
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
 
*/
/** @file project.hpp
 ** This file contains the definition of Project, a class which
 ** stores project data, and wraps proc layer data
 */

#ifndef PROJECT_HPP
#define PROJECT_HPP

#include "sequence.hpp"

namespace gui {
namespace model {
  
class Project
{
public:
  Project();
  
  ~Project();
    
  const std::list< boost::shared_ptr<Sequence> >& get_sequences();
  
  void add_new_sequence(Glib::ustring name);
  
public:
  sigc::signal<void>& signal_sequence_list_changed();
  
private:

  std::list< boost::shared_ptr<Sequence> > sequences;
  
  sigc::signal<void> sequenceListChangedSignal;
};

}   // namespace model
}   // namespace gui

#endif // PROJECT_HPP
