/*
  gtk-lumiera.cpp  -  The entry point for the GTK GUI application
 
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

#include <gtkmm.h>
#include <nobug.h>

#ifdef ENABLE_NLS
#  include <libintl.h>
#endif

#include "gtk-lumiera.hpp"
#include "window-manager.hpp"
#include "workspace/workspace-window.hpp"
#include "model/project.hpp"

NOBUG_CPP_DEFINE_FLAG(gui);

using namespace Gtk;
using namespace Glib;
using namespace lumiera::gui;
using namespace lumiera::gui::workspace;
using namespace lumiera::gui::model;

GtkLumiera the_application;

int
main (int argc, char *argv[])
{
  return the_application.main(argc, argv);
}



namespace lumiera {
namespace gui {

int
GtkLumiera::main(int argc, char *argv[])
{
  NOBUG_INIT;

  Main kit(argc, argv);
  
  Glib::set_application_name(AppTitle);

  Project project;
  WindowManager window_manager;

  window_manager.set_theme("lumiera_ui.rc");

  WorkspaceWindow main_window(&project);

  kit.run(main_window); 
}

Glib::ustring
GtkLumiera::get_home_data_path()
{
  const ustring app_name("lumiera");
  const ustring path(Glib::get_home_dir());
  return ustring::compose("%1/.%2", path, app_name);
}

GtkLumiera&
application()
{
  return the_application;  
}

}   // namespace gui
}   // namespace lumiera


