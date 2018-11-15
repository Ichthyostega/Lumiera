/*
  Panel  -  common base class for all docking panels 

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


/** @file panel.cpp
 ** Implementation base for all dockable panels
 */


#include "stage/gtk-base.hpp"
#include "stage/panel/panel.hpp"
#include "stage/workspace/panel-manager.hpp"
#include "stage/workspace/workspace-window.hpp"

#include <gdl/gdl-dock-item-grip.h>

using namespace Gtk;         //////////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

namespace stage  {
namespace panel{
  
  Panel::Panel (workspace::PanelManager& panelManager
               ,Gdl::DockItem& dockItem
               ,const gchar* longName
               ,const gchar* stockID)
    : Gtk::Box{Gtk::ORIENTATION_VERTICAL}
    , panelManager_(panelManager)
    , dockItem_(dockItem)
    , panelBar_(*this, stockID)
    {
      // Set dockItems long-name property
      Glib::Value<std::string> val;
      val.init(val.value_type());
      val.set(longName);
      g_object_set_property (G_OBJECT (dockItem.gobj()), "long-name", val.gobj());
      
      /* Set the grip handle */
      ///////////////////////////////////////////////////////TICKET #1027 : find out how to do this with gdlmm (C++ binding). No direct usage of GDL !
      GdlDockItemGrip *grip = GDL_DOCK_ITEM_GRIP(
        gdl_dock_item_get_grip(dockItem.gobj()));
      gdl_dock_item_grip_show_handle(grip);
      gdl_dock_item_grip_set_label(grip, ((Widget&)panelBar_).gobj());
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
      ///////////////////////////////////////////////////////TICKET #172 : observed as a reason for crashes when closing the GUI. It was invoked after end of main, when the GUI was already gone.
    
  #if false /////////////////////////////////////////////////TICKET #937 : disabled for GTK-3 transition. TODO investigate why this logic existed...    
    /* Detach the panel bar */
      ///////////////////////////////////////////////////////TICKET #1027 !!
    GdlDockItemGrip *grip = GDL_DOCK_ITEM_GRIP(
      gdl_dock_item_get_grip(dockItem_.gobj()));
    gtk_container_remove (GTK_CONTAINER(grip),
      ((Widget&)panelBar_).gobj());
  
    /* Remove this panel's container from the DockItem */
    dockItem_.remove((Gtk::Widget&)*this);
  #endif    /////////////////////////////////////////////////TICKET #937 : (End)disabled for GTK-3 transition.
  }
  
  Gdl::DockItem&
  Panel::getDockItem()
  {
    return dockItem_;
  }
  
  void
  Panel::show(bool show)
  {
    //REQUIRE(dockItem != NULL);
    if(show) dockItem_.show_item();
    else dockItem_.hide_item();
  }
  
  bool
  Panel::is_shown() const
  {
    //REQUIRE(dockItem != NULL);
    return dockItem_.get_visible();
  }
  
  void
  Panel::iconify()
  {
    dockItem_.iconify_item();
  }
  
  bool
  Panel::is_iconified() const
  {
    /** Gdlmm as of v1.30.0 does not have
     * a facility for checking the whether
     * a dock item is iconified or not   ////////////////////TODO explicit usings please!
     */
    GdlDockItem *item = dockItem_.gobj();
    REQUIRE(item != NULL);
    return GDL_DOCK_ITEM_ICONIFIED (item);
  }
  
  void
  Panel::lock(bool lock)
  {
    if(lock) dockItem_.lock();
    else dockItem_.unlock();
  }
  
  bool
  Panel::is_locked() const
  {
    REQUIRE(dockItem_.gobj() != NULL);
    return not GDL_DOCK_ITEM_NOT_LOCKED(dockItem_.gobj());
  }
  
  workspace::PanelManager&
  Panel::getPanelManager()
  {
    return panelManager_;
  }
  
  workspace::WorkspaceWindow& 
  Panel::getWorkspaceWindow()
  {
    return panelManager_.getWorkspaceWindow();
  }
  
  sigc::signal<void>&
  Panel::signal_hidePanel()
  {
    return hidePanelSignal_;
  }
  
  void
  Panel::on_item_hidden()
  {
    hidePanelSignal_.emit();
  }
  
  
}}// namespace stage::panel
