/*
  workspace-window.cpp  -  Definition of the main workspace window object
 
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

#include <gtkmm/stock.h>

#ifdef ENABLE_NLS
#  include <libintl.h>
#endif

#include <libgdl-1.0/gdl/gdl-tools.h>
#include <libgdl-1.0/gdl/gdl-dock.h>
#include <libgdl-1.0/gdl/gdl-dock-item.h>
#include <libgdl-1.0/gdl/gdl-dock-placeholder.h>
#include <libgdl-1.0/gdl/gdl-dock-bar.h>

#include "../gtk-lumiera.hpp"
#include "workspace-window.hpp"

using namespace Gtk;
using namespace gui::model;
using namespace gui::controller;

namespace gui {
namespace workspace {
	
WorkspaceWindow::WorkspaceWindow(Project &source_project,
  gui::controller::Controller &source_controller) :
  project(source_project),
  controller(source_controller),
  actions(*this)
{    
  layout = NULL;
  assetsPanel = NULL;
  viewerPanel = NULL;
  timelinePanel = NULL;

  create_ui();
}

WorkspaceWindow::~WorkspaceWindow()
{
  REQUIRE(layout != NULL);
  g_object_unref(layout);
  
  REQUIRE(assetsPanel != NULL);
  assetsPanel->unreference();
  REQUIRE(viewerPanel != NULL);
  viewerPanel->unreference();
  REQUIRE(timelinePanel != NULL);
  timelinePanel->unreference();
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

void
WorkspaceWindow::create_ui()
{    
  //----- Configure the Window -----//
  set_title(GtkLumiera::AppTitle);
  set_default_size(1024, 768);

  //----- Set up the UI Manager -----//
  // The UI will be nested within a VBox
  add(baseContainer);

  uiManager = Gtk::UIManager::create();
  uiManager->insert_action_group(actions.actionGroup);

  add_accel_group(uiManager->get_accel_group());

  //Layout the actions in a menubar and toolbar:
  Glib::ustring ui_info = 
      "<ui>"
      "  <menubar name='MenuBar'>"
      "    <menu action='FileMenu'>"
      "      <menuitem action='FileNewProject'/>"
      "      <menuitem action='FileOpenProject'/>"
      "      <separator/>"
      "      <menuitem action='FileRender'/>"
      "      <separator/>"
      "      <menuitem action='FileQuit'/>"
      "    </menu>"
      "    <menu action='EditMenu'>"
      "      <menuitem action='EditCopy'/>"
      "      <menuitem action='EditPaste'/>"
      "      <separator/>"
      "      <menuitem action='EditPreferences'/>"
      "    </menu>"
      "    <menu action='ViewMenu'>"
      "      <menuitem action='ViewAssets'/>"
      "      <menuitem action='ViewTimeline'/>"
      "      <menuitem action='ViewViewer'/>"
      "    </menu>"
      "    <menu action='SequenceMenu'>"
      "      <menuitem action='SequenceAdd'/>"
      "    </menu>"
      "    <menu action='TrackMenu'>"
      "      <menuitem action='TrackAdd'/>"
      "    </menu>"
      "    <menu action='HelpMenu'>"
      "      <menuitem action='HelpAbout'/>"
      "    </menu>"
      "  </menubar>"
      "  <toolbar  name='ToolBar'>"
      "    <toolitem action='FileNewProject'/>"
      "    <toolitem action='FileOpenProject'/>"
      "  </toolbar>"
      "</ui>";

  try
    {
      uiManager->add_ui_from_string(ui_info);
    }
  catch(const Glib::Error& ex)
    {
      ERROR(gui, "Building menus failed: %s", ex.what().data());
      return;
    }

  //----- Set up the Menu Bar -----//
  Gtk::Widget* menu_bar = uiManager->get_widget("/MenuBar");
  REQUIRE(menu_bar != NULL);
  baseContainer.pack_start(*menu_bar, Gtk::PACK_SHRINK);
  
  //----- Set up the Tool Bar -----//
  Gtk::Toolbar* toolbar = dynamic_cast<Gtk::Toolbar*>(
    uiManager->get_widget("/ToolBar"));
  REQUIRE(toolbar != NULL);
  toolbar->set_toolbar_style(TOOLBAR_ICONS);
  baseContainer.pack_start(*toolbar, Gtk::PACK_SHRINK);
  
  //----- Create the Panels -----//
  assetsPanel = new AssetsPanel(*this);
  ENSURE(assetsPanel != NULL);
  viewerPanel = new ViewerPanel(*this);
  ENSURE(viewerPanel != NULL);
  timelinePanel = new TimelinePanel(*this);
  ENSURE(timelinePanel != NULL);

  //----- Create the Dock -----//
  dock = Glib::wrap(gdl_dock_new());
  
  layout = gdl_dock_layout_new((GdlDock*)dock->gobj());
  
  dockbar = Glib::wrap(gdl_dock_bar_new ((GdlDock*)dock->gobj()));

  dockContainer.pack_start(*dockbar, PACK_SHRINK);
  dockContainer.pack_end(*dock, PACK_EXPAND_WIDGET);
  baseContainer.pack_start(dockContainer, PACK_EXPAND_WIDGET);

  gdl_dock_add_item ((GdlDock*)dock->gobj(),
    assetsPanel->get_dock_item(), GDL_DOCK_LEFT);
  gdl_dock_add_item ((GdlDock*)dock->gobj(),
    viewerPanel->get_dock_item(), GDL_DOCK_RIGHT);
  gdl_dock_add_item ((GdlDock*)dock->gobj(),
    timelinePanel->get_dock_item(), GDL_DOCK_BOTTOM);

  // Manually dock and move around some of the items
  gdl_dock_item_dock_to (timelinePanel->get_dock_item(),
    assetsPanel->get_dock_item(), GDL_DOCK_BOTTOM, -1);
  gdl_dock_item_dock_to (viewerPanel->get_dock_item(),
    assetsPanel->get_dock_item(), GDL_DOCK_RIGHT, -1);
  
  gchar ph1[] = "ph1";
  gdl_dock_placeholder_new (ph1, (GdlDockObject*)dock->gobj(),
    GDL_DOCK_TOP, FALSE);
  gchar ph2[] = "ph2";
  gdl_dock_placeholder_new (ph2, (GdlDockObject*)dock->gobj(),
    GDL_DOCK_BOTTOM, FALSE);
  gchar ph3[] = "ph3";
  gdl_dock_placeholder_new (ph3, (GdlDockObject*)dock->gobj(),
    GDL_DOCK_LEFT, FALSE);
  gchar ph4[] = "ph4";
  gdl_dock_placeholder_new (ph4, (GdlDockObject*)dock->gobj(),
    GDL_DOCK_RIGHT, FALSE);
    
  //----- Create the status bar -----//
  statusBar.set_has_resize_grip();
  baseContainer.pack_start(statusBar, PACK_SHRINK);
 
  show_all_children();
}

}   // namespace workspace
}   // namespace gui
