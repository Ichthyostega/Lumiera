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
#include "workspace-window.hpp"

#include "../dialogs/render.hpp"
#include "../dialogs/preferences-dialog.hpp"

using namespace Gtk;
using namespace Glib;
using namespace lumiera::gui;

namespace lumiera {
namespace gui {
namespace workspace {

  Actions::Actions(WorkspaceWindow &workspace_window) :
	  workspaceWindow(workspace_window)
  {
	  actionGroup = ActionGroup::create();

	  // File menu
	  actionGroup->add(Action::create("FileMenu", _("_File")));
	  actionGroup->add(Action::create("FileNewProject", Stock::NEW, _("_New Project...")),
      sigc::mem_fun(*this, &Actions::on_menu_file_new_project));
	  actionGroup->add(Action::create("FileOpenProject", Stock::OPEN, _("_Open Project...")),
      sigc::mem_fun(*this, &Actions::on_menu_file_open_project));
    actionGroup->add(Action::create("FileRender", _("_Render...")),
      Gtk::AccelKey("<shift>R"),
	    sigc::mem_fun(*this, &Actions::on_menu_file_render));
	  actionGroup->add(Action::create("FileQuit", Stock::QUIT),
	    sigc::mem_fun(*this, &Actions::on_menu_file_quit));

	  // Edit menu
	  actionGroup->add(Action::create("EditMenu", _("_Edit")));
	  actionGroup->add(Action::create("EditCopy", Stock::COPY),
	    sigc::mem_fun(*this, &Actions::on_menu_others));
	  actionGroup->add(Action::create("EditPaste", Stock::PASTE),
	    sigc::mem_fun(*this, &Actions::on_menu_others));
	  actionGroup->add(Action::create("EditPreferences", Stock::PREFERENCES),
	    sigc::mem_fun(*this, &Actions::on_menu_edit_preferences));

    // View Menu
    actionGroup->add(Action::create("ViewMenu", _("_View")));
	  actionGroup->add(Action::create("ViewViewer", _("_Viewer")),
	    sigc::mem_fun(*this, &Actions::on_menu_view_viewer));
	  actionGroup->add(Action::create("ViewTimeline", _("_Timeline")),
	    sigc::mem_fun(*this, &Actions::on_menu_view_timeline));

	  // Help Menu
	  actionGroup->add(Action::create("HelpMenu", _("_Help")) );
	  actionGroup->add(Action::create("HelpAbout", Stock::ABOUT),
	  sigc::mem_fun(*this, &Actions::on_menu_help_about) );
  }

  /* ===== File Menu Event Handlers ===== */

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
    dialogs::Render dialog(workspaceWindow);
    dialog.run();
  }

  void
  Actions::on_menu_file_quit()
  {
    workspaceWindow.hide(); // Closes the main window to stop the Gtk::Main::run().
  }

  void
  Actions::on_menu_edit_preferences()
  {
    dialogs::PreferencesDialog dialog(workspaceWindow);
    dialog.run();
  }

  void
  Actions::on_menu_view_viewer()
  {
    //workspaceWindow.viewer_panel.show();
  }

  void
  Actions::on_menu_view_timeline()
  {
    //workspaceWindow.timeline_panel.show();
  }

  void
  Actions::on_menu_help_about()
  {
    // Configure the about dialog
    AboutDialog dialog;
    
    //dialog.set_program_name(AppTitle);
    dialog.set_version(AppVersion);
    //dialog.set_version(Appconfig::get("version"));
    dialog.set_copyright(AppCopyright);
    dialog.set_website(AppWebsite);
    dialog.set_authors(StringArrayHandle(AppAuthors,
      sizeof(AppAuthors) / sizeof(gchar*),
      OWNERSHIP_NONE));

    dialog.set_transient_for(workspaceWindow);
    
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

