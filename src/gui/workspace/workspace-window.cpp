/*
  workspace-window.cpp  -  Definition of the main workspace window object

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

#include "gui/gtk-lumiera.hpp"
#include "workspace-window.hpp"
#include "include/logging.h"

#include <gtkmm/stock.h>

#ifdef ENABLE_NLS
#  include <libintl.h>
#endif



using namespace Gtk;
using namespace Glib;
using namespace gui::model;
using namespace gui::controller;

namespace gui {
namespace workspace {
	
WorkspaceWindow::WorkspaceWindow(Project &source_project,
  gui::controller::Controller &source_controller) :
  project(source_project),
  controller(source_controller),
  panelManager(*this),
  actions(*this)
{    
  create_ui();
}

WorkspaceWindow::~WorkspaceWindow()
{
  INFO (gui_dbg, "Closing workspace window...");
}

Project&
WorkspaceWindow::get_project()
{
  return project;
}

Controller&
WorkspaceWindow::get_controller()
{
  return controller;
}

PanelManager&
WorkspaceWindow::get_panel_manager()
{
  return panelManager;
}

void
WorkspaceWindow::create_ui()
{
  // RTL Test Code
  //set_default_direction (TEXT_DIR_RTL);
   
  //----- Configure the Window -----//
  set_title(GtkLumiera::getAppTitle());
  set_default_size(1024, 768);

  //----- Set up the UI Manager -----//
  // The UI will be nested within a VBox
  add(baseContainer);

  uiManager = Gtk::UIManager::create();
  actions.populate_main_actions(uiManager);
  add_accel_group(uiManager->get_accel_group());

  //----- Set up the Menu Bar -----//
  Gtk::Widget* menu_bar = uiManager->get_widget("/MenuBar");
  REQUIRE(menu_bar != NULL);
  baseContainer.pack_start(*menu_bar, Gtk::PACK_SHRINK);

  //----- Create the Docks -----//
  panelManager.setup_dock();
  dockContainer.pack_start(panelManager.get_dock_bar(),false,false,0);
  dockContainer.pack_start(panelManager.get_dock(),true,true,0);
  baseContainer.pack_start(dockContainer, PACK_EXPAND_WIDGET);
    
  //----- Create the status bar -----//
  //statusBar.set_has_resize_grip();
  statusBar.set_resize_mode(Gtk::RESIZE_PARENT);
  baseContainer.pack_start(statusBar, PACK_SHRINK);
 
  show_all_children();
}

void
WorkspaceWindow::set_close_window_sensitive(bool enable)
{
  uiManager->get_action("/MenuBar/WindowMenu/WindowCloseWindow")
    ->set_sensitive(enable);
}

}   // namespace workspace
}   // namespace gui

