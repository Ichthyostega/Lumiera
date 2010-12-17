/*
  workspace-window.hpp  -  Definition of the main workspace window object

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
/** @file workspace-window.hpp
 ** This file contains the definition of the main workspace window
 ** parent, which is the toplevel parent of the whole workspace.
 **
 ** @see actions.hpp
 */

#ifndef WORKSPACE_WINDOW_HPP
#define WORKSPACE_WINDOW_HPP

#include <gtkmm.h>

#include "actions.hpp"
#include "panel-manager.hpp"

#include "../panels/resources-panel.hpp"
#include "../panels/viewer-panel.hpp"
#include "../panels/timeline-panel.hpp"

using namespace gui::panels;

namespace gui {

namespace model {
  class Project;
} // model

namespace controller {
  class Controller;
} // model

namespace workspace {

/** 
* The main lumiera workspace window
*/
class WorkspaceWindow : public Gtk::Window
{
public:
  WorkspaceWindow(gui::model::Project &source_project,
    gui::controller::Controller &source_controller);
  
  ~WorkspaceWindow();
  
  gui::model::Project& get_project();
  
  gui::controller::Controller& get_controller();
  
  PanelManager& get_panel_manager();

private:
  void create_ui();
  
  /* ===== Model ===== */
private:
  gui::model::Project &project;

  /* ===== Controller ===== */
private:
  gui::controller::Controller &controller;

  /* ===== UI ===== */
private:
  Glib::RefPtr<Gtk::UIManager> uiManager;
  Gtk::VBox baseContainer;
  Gtk::HBox dockContainer;
  
  PanelManager panelManager;
  
  //----- Status Bar -----//
  Gtk::Statusbar statusBar;
       
  /* ===== Helpers ===== */
private:
  /**
   * The instantiation of the actions helper class, which
   * registers and handles user action events */
  Actions actions;
};

}   // namespace workspace
}   // namespace gui

#endif // WORKSPACE_WINDOW_HPP
