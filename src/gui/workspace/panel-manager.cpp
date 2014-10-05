/*
  panel-manager.cpp  -  Definition of the panel manager object

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

#include "gui/workspace/panel-manager.hpp"

#include "gui/panels/assets-panel.hpp"
#include "gui/panels/viewer-panel.hpp"
#include "gui/panels/timeline-panel.hpp"

#include "include/logging.h"

using namespace boost;
using namespace std;
using namespace Gtk;

namespace gui {
namespace workspace {

const PanelManager::PanelDescription
  PanelManager::panelDescriptionList[] = {
  PanelManager::Panel<TimelinePanel>(),
  PanelManager::Panel<ViewerPanel>(),
  PanelManager::Panel<AssetsPanel>()
  };
  
unsigned short PanelManager::panelID = 0;

PanelManager::PanelManager(WorkspaceWindow &workspace_window) :
  workspaceWindow(workspace_window),
  dock(),
  dockBar(dock),
  dockLayout()
{
  dockLayout = Gdl::DockLayout::create(dock);
  memset(&dockPlaceholders, 0, sizeof(dockPlaceholders)); 
}

PanelManager::~PanelManager()
{
  
  for(int i = 0; i < 4; i++)
    if(dockPlaceholders[i])
      g_object_unref(dockPlaceholders[i]);
      
  clear_panels();
}

void
PanelManager::setup_dock()
{

  REQUIRE(dockPlaceholders[0] == NULL && dockPlaceholders[1] == NULL &&
    dockPlaceholders[2] == NULL && dockPlaceholders[3] == NULL);
  dockPlaceholders[0] = GDL_DOCK_PLACEHOLDER(gdl_dock_placeholder_new(
    "ph1", GDL_DOCK_OBJECT(dock.gobj()), GDL_DOCK_TOP, FALSE));
  dockPlaceholders[1] = GDL_DOCK_PLACEHOLDER(gdl_dock_placeholder_new(
    "ph2", GDL_DOCK_OBJECT(dock.gobj()), GDL_DOCK_BOTTOM, FALSE));
  dockPlaceholders[2] = GDL_DOCK_PLACEHOLDER(gdl_dock_placeholder_new(
    "ph3", GDL_DOCK_OBJECT(dock.gobj()), GDL_DOCK_LEFT, FALSE));
  dockPlaceholders[3] = GDL_DOCK_PLACEHOLDER(gdl_dock_placeholder_new(
    "ph4", GDL_DOCK_OBJECT(dock.gobj()), GDL_DOCK_RIGHT, FALSE));
  ENSURE(dockPlaceholders[0] && dockPlaceholders[1] &&
    dockPlaceholders[2] && dockPlaceholders[3]);
  
  create_panels();
}

GdlDock*
PanelManager::get_dock()
{
  return dock.gobj();
}

GdlDockBar*
PanelManager::get_dock_bar()
{
  return dockBar.gobj();
}

WorkspaceWindow&
PanelManager::get_workspace_window()
{
  return workspaceWindow;
}

void
PanelManager::show_panel(const int description_index)
{
  // Try and find the panel and present it if possible
  list< panels::Panel* >::iterator i;
  for(i = panels.begin(); i != panels.end(); i++)
    {
      panels::Panel* const panel = *i;
      if(get_panel_type(panel) == description_index)
        {
          if(!panel->is_shown())
            panel->show();
          
          GdlDockItem *dock_item = panel->get_dock_item();
          ENSURE(dock_item);
          gdl_dock_object_present(GDL_DOCK_OBJECT(dock_item), NULL);
          return;
        }
    }
  
  // Create the new panel
  panels::Panel *new_panel = create_panel_by_index(description_index);
  
  // Dock the item
  TODO("Port to Gdlmm.");
  gdl_dock_add_item(dock.gobj(), new_panel->get_dock_item(),
    GDL_DOCK_FLOATING);
}

void PanelManager::switch_panel(panels::Panel &old_panel,
  const int description_index)
{
  REQUIRE(description_index >= 0 &&
    description_index < get_panel_description_count());
  
  // Get the dock item
  GdlDockItem *dock_item = old_panel.get_dock_item();
  g_object_ref(dock_item);
   
  // Release the old panel
  remove_panel(&old_panel);
  
  // Create the new panel
  create_panel_by_index(description_index, dock_item);
  g_object_unref(dock_item);
}

void
PanelManager::split_panel(panels::Panel &panel,
  Gtk::Orientation split_direction)
{
  // Create the new panel
  const int index = get_panel_type(&panel);
  panels::Panel *new_panel = create_panel_by_index(index);
  
  // Dock the panel
  GdlDockPlacement placement = GDL_DOCK_NONE;
  switch(split_direction)
    {
    case ORIENTATION_HORIZONTAL:
      placement = GDL_DOCK_RIGHT;
      break;
    case ORIENTATION_VERTICAL:
      placement = GDL_DOCK_BOTTOM;
      break;
    default:
      ERROR(gui, "Unrecognisized split_direction: %d", split_direction);
      return;
      break;
    }
    
  gdl_dock_object_dock(GDL_DOCK_OBJECT(panel.get_dock_item()),
    GDL_DOCK_OBJECT(new_panel->get_dock_item()), placement, NULL);
}

int
PanelManager::get_panel_description_count()
{
  return sizeof(panelDescriptionList) / sizeof(PanelDescription);
}

const gchar*
PanelManager::get_panel_stock_id(int index)
{
  REQUIRE(index >= 0 && index < get_panel_description_count());
  return panelDescriptionList[index].get_stock_id();
}

const char*
PanelManager::get_panel_title(int index)
{
  REQUIRE(index >= 0 && index < get_panel_description_count());
  return panelDescriptionList[index].get_title();
}

void
PanelManager::create_panels()
{
  panels::Panel* assetsPanel =
    create_panel_by_name("AssetsPanel");
  panels::Panel* viewerPanel = 
    create_panel_by_name("ViewerPanel");
  panels::Panel* timelinePanel = 
    create_panel_by_name("TimelinePanel");
    
  gdl_dock_add_item(dock.gobj(),
    assetsPanel->get_dock_item(), GDL_DOCK_LEFT);
  gdl_dock_add_item(dock.gobj(),
    timelinePanel->get_dock_item(), GDL_DOCK_BOTTOM);
  gdl_dock_add_item(dock.gobj(),
    viewerPanel->get_dock_item(), GDL_DOCK_RIGHT);
}

int
PanelManager::find_panel_description(const char* class_name) const
{
  REQUIRE(class_name);
  
  const int count = get_panel_description_count();
  for(int i = 0; i < count; i++)
    {
      if(strstr(panelDescriptionList[i].get_class_name(), class_name))
        return i;
    }
    
  ERROR(gui, "Unable to find a description with class name %s",
    class_name);
  return -1;
}

panels::Panel*
PanelManager::create_panel_by_index(const int index)
{
  REQUIRE(index >= 0 && index < get_panel_description_count());

  // Make a unique name for the panel
  char name[5];
  snprintf(name, sizeof(name), "%X", panelID++);
  
  // Create a dock item
  GdlDockItem *dock_item = GDL_DOCK_ITEM(
    gdl_dock_item_new(name, "", GDL_DOCK_ITEM_BEH_NORMAL));
    
  return create_panel_by_index(index, dock_item);
}

panels::Panel*
PanelManager::create_panel_by_index(
  const int index, GdlDockItem *dock_item)
{
  // Create the panel object
  panels::Panel *panel = 
    panelDescriptionList[index].create(*this, dock_item);
  ENSURE(panel);
  panel->show_all();
  
  // Connect event handlers
  panel->signal_hide_panel().connect(bind(
    mem_fun(*this, &PanelManager::on_panel_shown), panel));
  
  // Add the panel to the list
  panels.push_back(panel);
  
  return panel;    
}

panels::Panel*
PanelManager::create_panel_by_name(const char* class_name)
{ 
  REQUIRE(class_name);
  const int index = find_panel_description(class_name);  
  return create_panel_by_index(index);
}

int
PanelManager::get_panel_type(panels::Panel* const panel) const
{
  REQUIRE(panel);
  
  const type_info &info = typeid(*panel); 
  const int count = get_panel_description_count();
  for(int i = 0; i < count; i++)
    {
      if(info == panelDescriptionList[i].get_class_info())
        return i;
    }
  
  ERROR(gui, "Unable to find a description with with this class type");
  return -1;
}

void
PanelManager::remove_panel(panels::Panel* const panel)
{
  REQUIRE(panel);
  
  list< panels::Panel* >::iterator i;
  for(i = panels.begin(); i != panels.end(); i++)
    {
      if(*i == panel)
        {
          delete panel;
          panels.erase(i);
          break;
        }
    }
}

void
PanelManager::clear_panels()
{
  list< panels::Panel* >::iterator i;
  for(i = panels.begin(); i != panels.end(); i++)
    delete *i;
  panels.clear();
}

void
PanelManager::on_panel_shown(panels::Panel *panel)
{  
  REQUIRE(panel);
  
  if(panel->is_shown() || panel->is_iconified())
    return;
  
  remove_panel(panel);
}

}   // namespace workspace
}   // namespace gui
