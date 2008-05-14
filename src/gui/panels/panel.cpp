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

namespace lumiera {
namespace gui {
namespace panels {

Panel::Panel(const gchar *name, const gchar *long_name, GdlDockItemBehavior behavior)
{
  dock_item = (GdlDockItem*)gdl_dock_item_new (name, long_name, behavior);
  internal_setup();
}

Panel::Panel(const gchar *name, const gchar *long_name, const gchar *stock_id,
  GdlDockItemBehavior behavior)
{
  dock_item = (GdlDockItem*)gdl_dock_item_new_with_stock (name, long_name, stock_id, behavior);
  internal_setup();
}

Panel::~Panel()
{

}

GdlDockItem* Panel::get_dock_item() const
{
  return dock_item;
}

void Panel::internal_setup()
{
  gtk_container_add ((GtkContainer*)dock_item, (GtkWidget*)gobj());  
  gtk_widget_show ((GtkWidget*)dock_item);
}

}   // namespace panels
}   // namespace gui
}   // namespace lumiera

