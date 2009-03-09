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
#include <libgdl-1.0/gdl/gdl-dock-item-grip.h>

namespace gui {
namespace panels {

Panel::Panel(workspace::WorkspaceWindow &workspace_window,
  const gchar *name, const gchar *long_name,
  GdlDockItemBehavior behavior) :
  workspace(workspace_window),
  panelBar(long_name)
{
  REQUIRE(name != NULL);
  REQUIRE(long_name != NULL);
  
  dock_item = (GdlDockItem*)gdl_dock_item_new (
    name, long_name, behavior);
  internal_setup();
  
  ENSURE(dock_item != NULL);
}

Panel::Panel(workspace::WorkspaceWindow &workspace_window,
  const gchar *name, const gchar *long_name, const gchar *stock_id,
  GdlDockItemBehavior behavior) :
  workspace(workspace_window),
  panelBar(long_name, stock_id)
{
  REQUIRE(name != NULL);
  REQUIRE(long_name != NULL);
  REQUIRE(stock_id != NULL);
  
  dock_item = (GdlDockItem*)gdl_dock_item_new_with_stock (
    name, long_name, stock_id, behavior);
  g_object_ref(dock_item);
  internal_setup();
  
  ENSURE(dock_item != NULL);
}

Panel::~Panel()
{
  REQUIRE(dock_item != NULL);
  g_object_unref(dock_item);
  dock_item = NULL;
}

GdlDockItem*
Panel::get_dock_item() const
{
  return dock_item;
}

void
Panel::show(bool show)
{
  REQUIRE(dock_item != NULL);
  if(show) gdl_dock_item_show_item (dock_item);
  else gdl_dock_item_hide_item (dock_item);
}

bool
Panel::is_shown() const
{
  REQUIRE(dock_item != NULL);
  return GTK_WIDGET_VISIBLE((GtkWidget*)dock_item);
}

void
Panel::internal_setup()
{
  REQUIRE(dock_item != NULL);
  REQUIRE(gobj() != NULL);
  
  GdlDockItemGrip *grip = GDL_DOCK_ITEM_GRIP(
    gdl_dock_item_get_grip(dock_item));
  gdl_dock_item_grip_set_label(grip, ((Widget&)panelBar).gobj());
    
  gtk_container_add ((GtkContainer*)dock_item, (GtkWidget*)gobj());  
  gtk_widget_show ((GtkWidget*)dock_item);
}

}   // namespace panels
}   // namespace gui
