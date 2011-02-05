/*
  gtk-lumiera.cpp  -  The entry point for the GTK GUI application

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

#include <gtkmm.h>
#include <nobug.h>

#ifdef ENABLE_NLS
#  include <libintl.h>
#endif

#include "gtk-lumiera.hpp"
#include "window-manager.hpp"
#include "workspace/workspace-window.hpp"
#include "model/project.hpp"
#include "controller/controller.hpp"

extern "C" {
#include "common/interface.h"
}

NOBUG_CPP_DEFINE_FLAG(gui);

using namespace Gtk;
using namespace Glib;
using namespace gui;
using namespace gui::workspace;
using namespace gui::model;
using namespace gui::controller;
using namespace std;

GtkLumiera the_application;




namespace gui {

void
GtkLumiera::main(int argc, char *argv[])
{
  Glib::thread_init();

  Main kit(argc, argv);
  
  Glib::set_application_name(get_app_title());

  Project project;
  Controller controller(project);

  windowManager.init();
  windowManager.set_theme("lumiera_ui.rc");
  windowManager.new_window(project, controller);

  kit.run();
}

WindowManager&
GtkLumiera::get_window_manager()
{
  return windowManager;
}

Glib::ustring
GtkLumiera::get_home_data_path()
{
  const ustring app_name("lumiera");
  const ustring path(Glib::get_home_dir());
  return ustring::compose("%1/.%2", path, app_name);
}

const Glib::ustring
GtkLumiera::get_app_title()
{
  return "Lumiera";
}

const Glib::ustring
GtkLumiera::get_app_version()
{
  return "0.pre.01";
}

const Glib::ustring GtkLumiera::get_app_copyright()
{
  return _("© 2008 The Lumiera Team");
}

const Glib::ustring GtkLumiera::get_app_website()
{
  return "http://www.lumiera.org";
}

const std::vector<Glib::ustring>
GtkLumiera::get_app_authors()
{
  const gchar* app_authors[] = {
    "Joel Holdsworth",
    "Christian Thaeter",
    "Hermann Vosseler",
    "[Other Authors Here]"};
  
  const int count = sizeof(app_authors) / sizeof(gchar*);
  std::vector<Glib::ustring> list(count);
  for(int i = 0; i < count; i++)
    list[i] = app_authors[i];
  return list;
}

GtkLumiera&
application()
{
  return the_application;  
}

}   // namespace gui
