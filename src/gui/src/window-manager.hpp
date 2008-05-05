/*
  window-manager.hpp  -  Defines the global UI Manager class
 
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
/** @file window-manager.hpp
 ** This file contains the defintion of global UI Manager class
 ** user actions.
 ** @see window-manager.cpp
 ** @see gtk-lumiera.hpp
 */

#include <gtkmm.h>

#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

namespace lumiera {
namespace gui {

  class WindowManager
  {
  public:
    WindowManager();

    bool set_theme(Glib::ustring path);

  protected:
    
  };

}   // namespace gui
}   // namespace lumiera

#endif // WINDOW_MANAGER_HPP
