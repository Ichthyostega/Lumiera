/*
  panel.cpp  -  Implementation of Panel, the base class for docking panels 
 
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

#include "panel.hpp"
#include "../gtk-lumiera.hpp"
#include "../workspace/panel-manager.hpp"
#include "../workspace/workspace-window.hpp"
#include <libgdl-1.0/gdl/gdl-dock-item-grip.h>

using namespace Gtk;

namespace gui {
namespace panels {

Panel::Panel(workspace::PanelManager &panel_manager,
    GdlDockItem *dock_item, const gchar* long_name,
    const gchar *stock_id) :
  panelManager(panel_manager),
  dockItem(dock_item),
  panelBar(*this, stock_id)
{
  REQUIRE(dockItem);
  g_object_ref(dockItem);
    
  Glib::Value<std::string> val;
  val.init(val.value_type());
  val.set(long_name);
  g_object_set_property (G_OBJECT (dockItem), "long-name", val.gobj());
  
  // Set the grip handle
  GdlDockItemGrip *grip = GDL_DOCK_ITEM_GRIP(
    gdl_dock_item_get_grip(dockItem));
  gdl_dock_item_grip_show_handle(grip);
  gdl_dock_item_grip_set_label(grip, ((Widget&)panelBar).gobj());
    
  // Set up the panel body
  gtk_container_add ((GtkContainer*)dockItem, (GtkWidget*)gobj());
  
  gtk_widget_show ((GtkWidget*)dockItem);
}

Panel::~Panel()
{
  REQUIRE(dockItem != NULL);
  g_object_unref(dockItem);
  dockItem = NULL;
}

GdlDockItem*
Panel::get_dock_item() const
{
  return dockItem;
}

void
Panel::show(bool show)
{
  REQUIRE(dockItem != NULL);
  if(show) gdl_dock_item_show_item (dockItem);
  else gdl_dock_item_hide_item (dockItem);
}

bool
Panel::is_shown() const
{
  REQUIRE(dockItem != NULL);
  return GTK_WIDGET_VISIBLE((GtkWidget*)dockItem);
}

workspace::PanelManager&
Panel::get_panel_manager()
{
  return panelManager;
}

workspace::WorkspaceWindow& 
Panel::get_workspace_window()
{
  return panelManager.get_workspace_window();
}

model::Project&
Panel::get_project()
{
  return panelManager.get_workspace_window().get_project();
}

controller::Controller&
Panel::get_controller()
{
  return panelManager.get_workspace_window().get_controller();
}

}   // namespace panels
}   // namespace gui
