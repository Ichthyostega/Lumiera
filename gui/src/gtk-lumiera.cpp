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
#include <iostream>

#ifdef ENABLE_NLS
#  include <libintl.h>
#endif

#include "gtk-lumiera.hpp"
#include "workspace/mainwindow.hpp"

using namespace lumiera::gui;
using namespace lumiera::gui::workspace;
using namespace Gtk;

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
	  Main kit(argc, argv);
		
	  Glib::set_application_name(AppTitle);
	
    init_ui();

	  MainWindow main_window;
	
	  kit.run(main_window); 
  }

  dialogs::Render*
  GtkLumiera::get_render_dialog() const
  {
    g_assert(renderDialog != NULL);
    return renderDialog;
  }

  void
  GtkLumiera::init_ui()
  {
    try
    {
      gladeXml = Gnome::Glade::Xml::create("gtk-lumiera.glade");
    }
    catch(const Gnome::Glade::XmlError& ex)
    {
      g_message(ex.what().data());
      return;
    }

    dialogs::Render::init(gladeXml, renderDialog);
  }

  GtkLumiera&
  application()
  {
    return the_application;  
  }

}   // namespace gui
}   // namespace lumiera


