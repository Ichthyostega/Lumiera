/*
  window-manager.cpp  -  Defines the global UI Manager class
 
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

#include "window-manager.hpp"

namespace lumiera {
namespace gui {

  WindowManager::WindowManager()
    {
    
    }

  bool WindowManager::set_theme(Glib::ustring path)
    {
      if(access(path.c_str(), R_OK))
        {
          g_error("WindowManger: Unable to load rc file \"%s\"", path.c_str());
          return false;        
        }
      
      gtk_rc_parse(path.c_str());
      gtk_rc_reset_styles (gtk_settings_get_default());

      return true;
    }

}   // namespace gui
}   // namespace lumiera
