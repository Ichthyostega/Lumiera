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

bool
WindowManager::set_theme(Glib::ustring path)
  {
    if(access(path.c_str(), R_OK))
      {
        ERROR(gui, "WindowManger: Unable to load rc file \"%s\"", path.c_str());
        return false;        
      }
    
    gtk_rc_parse(path.c_str());
    gtk_rc_reset_styles (gtk_settings_get_default());

    return true;
  }
    
GdkColor
WindowManager::read_style_colour_property(
  Gtk::Widget &widget, const gchar *property_name,
  guint16 red, guint16 green, guint16 blue)
  {
    GdkColor *colour;

    gtk_widget_style_get(widget.gobj(), property_name, &colour, NULL);

    // Did the color load successfully?
    if(colour != NULL)
      return *colour;
    else
    {
      WARN(gui, "%s style value failed to load", property_name);
      
      GdkColor default_colour;
      default_colour.red = red;
      default_colour.green = green;
      default_colour.blue = blue;     
      return default_colour;
    }
  }

}   // namespace gui
}   // namespace lumiera
