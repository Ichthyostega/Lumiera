/*
  ACTIONS.hpp  -  Definition of a helper class for user actions / menu

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


/** @file actions.hpp
 ** This file contains the definition of a helper class for the
 ** main workspace window object, which registers and handles
 ** user actions.
 ** @see mainwindow.hpp
 */


#ifndef GUI_WORKSPACE_ACTIONS_H
#define GUI_WORKSPACE_ACTIONS_H

#include "gui/gtk-lumiera.hpp"

namespace gui {
namespace workspace {
  
  class WorkspaceWindow;
  
  /**
  * A helper class which registers and handles
  * user action events.
  */
  class Actions
    {
      /**reference to the MainWindow which owns this helper */
      WorkspaceWindow& workspaceWindow_;
      
    public:
      Actions (WorkspaceWindow& owner);
      
      /**
       * Populates a uiManager with the main set of actions.
       * @param uiManager A pointer to the uiManager to populate.
       */
      void populateMainActions (Gtk::UIManager& uiManager);
      
      
      
    private: /* ===== Internals ===== */
      
      /**
       * Populates a uiManager with actions for the Show Panel menu.
       */
      void populateShowPanelActions(Gtk::UIManager& uiManager);
      
      /**
       * Updates the state of the menu/toolbar actions
       * to reflect the current state of the workspace */
      void updateActionState();
      
      
      /* ===== Event Handlers ===== */
    private:
      void onMenu_file_new_project();
      void onMenu_file_open_project();
      void onMenu_file_render();
      void onMenu_file_quit();
      
      void onMenu_edit_preferences();
      
      void onMenu_view_assets();
      void onMenu_view_timeline();
      void onMenu_view_viewer();
      
      void onMenu_sequence_add();
      
      void onMenu_track_add();
      
      void onMenu_window_new_window();
      void onMenu_window_close_window();
      void onMenu_show_panel(int panel_index);
      
      void onMenu_help_about();
      
      // Temporary Junk
      void onMenu_others();
      
      
      
    private: /* ===== Actions ===== */
      
      Glib::RefPtr<Gtk::ActionGroup> actionGroup;
      
      Glib::RefPtr<Gtk::ToggleAction> assetsPanelAction;
      Glib::RefPtr<Gtk::ToggleAction> timelinePanelAction;
      Glib::RefPtr<Gtk::ToggleAction> viewerPanelAction;
      
      
    private: /* ===== Internals ===== */
      bool is_updating_action_state;
    };
  
  
}}// namespace gui::workspace
#endif /*GUI_WORKSPACE_ACTIONS_H*/
