/*
  ACTIONS.hpp  -  Definition of a helper class for user actions
 
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
/** @file actions.hpp
 ** This file contains the definition of a helper class for the
 ** main workspace window object, which registers and handles
 ** user actions.
 ** @see mainwindow.hpp
 */

#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include "../gtk-lumiera.hpp"

namespace lumiera {
namespace gui {
namespace workspace {

class WorkspaceWindow;

  /**
   * A helper class which registers and handles
   * user action events.
   */
  class Actions
    {
    private:	
	    Actions(WorkspaceWindow &workspace_window);

	    /* ===== Internals ===== */
    private:
	    /**
       * Registers application stock items: icons and
       * labels associated with IDs */
			static void register_stock_items();

      static void add_stock_item(const Glib::RefPtr<Gtk::IconFactory>& factory,
                          const Glib::ustring& filepath,
                          const Glib::ustring& id, const Glib::ustring& label);

	    /**
       * Updates the state of the menu/toolbar actions
       * to reflect the current state of the workspace */
      void update_action_state();

	    /**
       * A reference to the MainWindow which owns
       * this helper */
	    WorkspaceWindow &workspaceWindow;

	    /* ===== Event Handlers ===== */
    private:
      void on_menu_file_new_project();
      void on_menu_file_open_project();
      void on_menu_file_render();
      void on_menu_file_quit();

      void on_menu_edit_preferences();

      void on_menu_view_assets();
      void on_menu_view_timeline();
      void on_menu_view_viewer();

      void on_menu_help_about();
      
      // Temporary Junk
      void on_menu_others();


	    /* ===== Actions ===== */
    private:
	    Glib::RefPtr<Gtk::ActionGroup> actionGroup;

      Glib::RefPtr<Gtk::ToggleAction> assetsPanelAction;
      Glib::RefPtr<Gtk::ToggleAction> timelinePanelAction;
      Glib::RefPtr<Gtk::ToggleAction> viewerPanelAction;

	    friend class WorkspaceWindow;
  };

}   // namespace workspace
}   // namespace gui
}   // namespace lumiera

#endif // ACTIONS_H
