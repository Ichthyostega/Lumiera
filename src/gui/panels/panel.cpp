/*
  panel.cpp  -  Implementation of Panel, the base class for docking panels 

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
#include "gui/panels/panel.hpp"
#include "gui/workspace/panel-manager.hpp"
#include "gui/workspace/workspace-window.hpp"

#include <gdl/gdl-dock-item-grip.h>

using namespace Gtk;

namespace gui {
namespace panels {

Panel::Panel(workspace::PanelManager &panel_manager,
    GdlDockItem *dock_item, const gchar* long_name,
    const gchar *stock_id) :
  panelManager(panel_manager),
  dockItem(dock_item),
  hide_panel_handler_id(0),
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
  gtk_container_add (GTK_CONTAINER(dockItem), GTK_WIDGET(gobj()));
  
  gtk_widget_show (GTK_WIDGET(dockItem));
  
  // Connect the signals
	hide_panel_handler_id = g_signal_connect (GTK_OBJECT(dockItem),
	  "hide",	G_CALLBACK(on_item_hidden), this);
}

Panel::~Panel()
{
  REQUIRE(dockItem != NULL);

  // Detach the panel bar
  GdlDockItemGrip *grip = GDL_DOCK_ITEM_GRIP(
    gdl_dock_item_get_grip(dockItem));
  gtk_container_remove (GTK_CONTAINER(grip),
    ((Widget&)panelBar).gobj());
    
  gtk_container_remove (GTK_CONTAINER(dockItem), GTK_WIDGET(gobj()));
    
  // Detach the signals
  g_signal_handler_disconnect(
    GTK_OBJECT(dockItem), hide_panel_handler_id);

  // Unref the dock item
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

void
Panel::iconify()
{
  REQUIRE(dockItem != NULL);
  gdl_dock_item_iconify_item(dockItem);
}

bool
Panel::is_iconified() const
{
  REQUIRE(dockItem != NULL);
  return GDL_DOCK_ITEM_ICONIFIED(dockItem);
}

void
Panel::lock(bool lock)
{
  REQUIRE(dockItem != NULL);
  if(lock) gdl_dock_item_lock (dockItem);
  else gdl_dock_item_unlock (dockItem);
}

bool
Panel::is_locked() const
{
  REQUIRE(dockItem != NULL);
  return !GDL_DOCK_ITEM_NOT_LOCKED(dockItem);
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

sigc::signal<void>&
Panel::signal_hide_panel()
{
  return hidePanelSignal;
}

void
Panel::on_item_hidden(GdlDockItem*, Panel *panel)
{
  REQUIRE(panel);
  panel->hidePanelSignal();
}

}   // namespace panels
}   // namespace gui
