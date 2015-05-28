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
    Gdl::DockItem &dock_item, const gchar* long_name,
    const gchar *stock_id) :
  panelManager(panel_manager),
  dockItem(dock_item),
  panelBar(*this, stock_id)
{
  // Set dockItems long-name property
  Glib::Value<std::string> val;
  val.init(val.value_type());
  val.set(long_name);
  g_object_set_property (G_OBJECT (dockItem.gobj()), "long-name", val.gobj());
  
  /* Set the grip handle */
  GdlDockItemGrip *grip = GDL_DOCK_ITEM_GRIP(
    gdl_dock_item_get_grip(dockItem.gobj()));
  gdl_dock_item_grip_show_handle(grip);
  gdl_dock_item_grip_set_label(grip, ((Widget&)panelBar).gobj());
  //gdl_dock_item_grip_set_cursor_type(grip, GDK_LEFT_PTR);

  /* Set up the panel body */
  // Add this panel's container to the DockItem
  dockItem.add((Gtk::Widget&)*this);

  /* Connect the signals */
  dockItem.signal_hide().connect(
      sigc::mem_fun(*this, &Panel::on_item_hidden));

  dockItem.show();
}

Panel::~Panel()
{
    ///////////////////////////////////////////////////////TICKET #195 : violation of policy, dtors must not do any work 
    ///////////////////////////////////////////////////////TICKET #172 : observed as a reason for crashes when closing the GUI. It was invoked after end of main, when the GUI as already gone.
  
#if false /////////////////////////////////////////////////TICKET #937 : disabled for GTK-3 transition. TODO investigate why this logic existed...    
  /* Detach the panel bar */
  GdlDockItemGrip *grip = GDL_DOCK_ITEM_GRIP(
    gdl_dock_item_get_grip(dockItem.gobj()));
  gtk_container_remove (GTK_CONTAINER(grip),
    ((Widget&)panelBar).gobj());

  /* Remove this panel's container from the DockItem */
  dockItem.remove((Gtk::Widget&)*this);
#endif    /////////////////////////////////////////////////TICKET #937 : (End)disabled for GTK-3 transition.
}

Gdl::DockItem&
Panel::get_dock_item()
{
  return dockItem;
}

void
Panel::show(bool show)
{
  //REQUIRE(dockItem != NULL);
  if(show) dockItem.show_item();
  else dockItem.hide_item();
}

bool
Panel::is_shown() const
{
  //REQUIRE(dockItem != NULL);
  return dockItem.get_visible();
}

void
Panel::iconify()
{
  dockItem.iconify_item();
}

bool
Panel::is_iconified() const
{
  /** Gdlmm as of v1.30.0 does not have
   * a facility for checking the whether
   * a dock item is iconified or not
   */
  GdlDockItem *item = dockItem.gobj();
  REQUIRE(item != NULL);
  return GDL_DOCK_ITEM_ICONIFIED (item);
}

void
Panel::lock(bool lock)
{
  if(lock) dockItem.lock();
  else dockItem.unlock();
}

bool
Panel::is_locked() const
{
  REQUIRE(dockItem.gobj() != NULL);
  return !GDL_DOCK_ITEM_NOT_LOCKED(dockItem.gobj());
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
Panel::on_item_hidden()
{
  hidePanelSignal.emit();
}

}   // namespace panels
}   // namespace gui
