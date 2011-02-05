/*
  panel.hpp  -  Definition of Panel, the base class for docking panels            

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
/** @file panel.hpp
 ** This file contains the definition of Panel; the base class
 ** for all docking panels
 */

#ifndef PANEL_HPP
#define PANEL_HPP

#include <gdl/gdl-dock-item.h>

#include "gui/gtk-lumiera.hpp"
#include "gui/widgets/panel-bar.hpp"

namespace gui {

namespace workspace {
class PanelManager;  
}

namespace panels {

/** 
 * The main lumiera panel window
 */
class Panel : public Gtk::VBox
{
protected:
  
  /**
   * Contructor.
   * @param panel_manager The owner panel manager widget.
   * @param dock_item The GdlDockItem that will host this panel.
   * @param long_name This title of the panel
   * @param stock_id The stock_id of this panel.
   */
  Panel(workspace::PanelManager &panel_manager,
    GdlDockItem *dock_item, const gchar* long_name,
    const gchar *stock_id);

public:
  /**
   * Destructor
   */
  ~Panel();

  /**
   * Returns a pointer to the underlying GdlDockItem structure
   */
  GdlDockItem* get_dock_item() const;

  /**
   * Shows or hides the panel.
   * @param show A value of true will show the panel, false will hide
   * it.
   */
  void show(bool show = true);
  
  /**
   * Returns true if the panel is currently visible.
   */
  bool is_shown() const;
  
  /**
   * Iconifys the panel.
   */
  void iconify();
  
  /**
   * Returns true if the panel is currently iconified.
   */
  bool is_iconified() const;
  
  /**
   * Locks or unlocks the panel.
   * @param show A value of true will lock the panel, false will unlock
   * it.
   */
  void lock(bool show = true);
  
  /**
   * Returns true if the panel is currently locked.
   */
  bool is_locked() const;
  
  /**
   * Returns a reference to the owner panel manager object.
   */
  workspace::PanelManager& get_panel_manager();
  
public:

  /**
   * A signal that fires when the dock item is hidden.
   */
  sigc::signal<void>& signal_hide_panel();
  
protected:

  /**
   * Returns a reference to the owner workspace window object.
   */
  workspace::WorkspaceWindow& get_workspace_window();

  /**
   * Returns a reference to the project 
   */
  model::Project& get_project();
  
  /**
   * Returns a reference to the controller
   */
  controller::Controller& get_controller();
  
private:

  /**
   * An event handler for when dockItem is hidden.
   * @param func_data A pointer to the panel that owns dock_item
   */
  static void on_item_hidden(GdlDockItem*, Panel *panel);

protected:

  /**
   * The owner panel manager object.
   */
  workspace::PanelManager &panelManager;

  /**
   * The owner dock item widget that will host the widgets in this
   * panel.
   */
  GdlDockItem* dockItem;
  
  /**
   * A signal that fires when the dock item is hidden.
   */
  sigc::signal<void> hidePanelSignal;

  /**
   * The id of the hide panel handler.
   */
  gulong hide_panel_handler_id;
  
  /**
   * The panel bar to attach to the panel grip.
   */
  widgets::PanelBar panelBar;
};

}   // namespace panels
}   // namespace gui

#endif // PANEL_HPP
