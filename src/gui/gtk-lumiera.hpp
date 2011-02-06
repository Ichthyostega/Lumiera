/*
  GTK-LUMIERA.hpp  -  The Lumiera GUI Application Object

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
/** @file gtk-lumiera.hpp
 ** This file contains application wide global definitions
 ** user actions.
 ** @see gtk-lumiera.cpp
 */

#ifndef GUI_GTK_LUMIERA_H
#define GUI_GTK_LUMIERA_H


#include "gui/gtk-base.hpp"
#include "gui/window-manager.hpp"

#include <boost/noncopyable.hpp>
#include <vector>


namespace gui {


/* ====== The Application Class ====== */

/**
 *  The main application class.
 */
class GtkLumiera
  : boost::noncopyable
  {
    /** Central application window manager instance */
    WindowManager windowManagerInstance_;
    
    
  public:
    /** access the the global application object */
    static GtkLumiera& application();
    
    
    
    void main(int argc, char *argv[]);
    
    WindowManager& windowManager();
    
    static Glib::ustring get_home_data_path();
    
    
    /** the name of the application */
    static const Glib::ustring get_app_title();
    
    static const Glib::ustring get_app_version();
    
    static const Glib::ustring get_app_copyright();
    
    static const Glib::ustring get_app_website();
    
    /** alphabetical list of the application's authors */
    static const std::vector<Glib::ustring> get_app_authors();
    
  };

}// namespace gui
#endif
