/*
  Actions.cpp  -  Definition of the main workspace window object
 
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

#include "../gtk-lumiera.hpp"
#include "actions.hpp"
#include "main-window.hpp"

using namespace Gtk;
using namespace Glib;

namespace lumiera {
namespace gui {
namespace workspace {

  Actions::Actions(MainWindow &main_window) :
	mainWindow(main_window)
  {
	  actionGroup = Gtk::ActionGroup::create();

	  // File menu:
	  actionGroup->add(Gtk::Action::create("FileMenu", _("File")));
	  actionGroup->add(Gtk::Action::create("FileNewProject", Gtk::Stock::NEW, _("_New Project...")),
      sigc::mem_fun(*this, &Actions::on_menu_file_new_project));
	  actionGroup->add(Gtk::Action::create("FileOpenProject", Gtk::Stock::OPEN, _("_Open Project...")),
      sigc::mem_fun(*this, &Actions::on_menu_file_new_project));
    actionGroup->add(Gtk::Action::create("FileRender", _("_Render...")),
      Gtk::AccelKey("<shift>R"),
	    sigc::mem_fun(*this, &Actions::on_menu_file_render));
	  actionGroup->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
	    sigc::mem_fun(*this, &Actions::on_menu_file_quit));

	  // Edit menu:
	  actionGroup->add(Gtk::Action::create("EditMenu", _("Edit")));
	  actionGroup->add(Gtk::Action::create("EditCopy", Gtk::Stock::COPY),
	    sigc::mem_fun(*this, &Actions::on_menu_others));
	  actionGroup->add(Gtk::Action::create("EditPaste", Gtk::Stock::PASTE),
	    sigc::mem_fun(*this, &Actions::on_menu_others));

	  // Help menu:
	  actionGroup->add( Gtk::Action::create("HelpMenu", "Help") );
	  actionGroup->add( Gtk::Action::create("HelpAbout", Gtk::Stock::ABOUT),
	  sigc::mem_fun(*this, &Actions::on_menu_help_about) );
  }

  void
  Actions::on_menu_file_new_project()
  {
    g_message("A File|New menu item was selecteda.");
  }

  void
  Actions::on_menu_file_open_project()
  {
    g_message("A File|Open menu item was selecteda.");
  }

  void
  Actions::on_menu_file_render()
  {
    mainWindow.renderDialog->run();
  }

  void
  Actions::on_menu_file_quit()
  {
    mainWindow.hide(); // Closes the main window to stop the Gtk::Main::run().
  }

  void
  Actions::on_menu_help_about()
  {
    // Configure the about dialog
    AboutDialog dialog;
    
    dialog.set_program_name(AppTitle);
    dialog.set_version(AppVersion);
    //dialog.set_version(Appconfig::get("version"));
    dialog.set_copyright(AppCopyright);
    dialog.set_website(AppWebsite);
    dialog.set_authors(StringArrayHandle(AppAuthors,
      sizeof(AppAuthors) / sizeof(gchar*),
      OWNERSHIP_NONE));

    dialog.set_transient_for(mainWindow);
    
    // Show the about dialog
    dialog.run();
  }



  //----- Temporary junk
  void
  Actions::on_menu_others()
  {
    g_message("A menu item was selected.");
  }

}   // namespace workspace
}   // namespace gui
}   // namespace lumiera

