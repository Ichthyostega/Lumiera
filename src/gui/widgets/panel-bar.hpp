/*
  panel-bar.hpp  -  Declaration of the panel bar widget

  Copyright (C)         Lumiera.org
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>

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
/** @file panel-bar.hpp
 ** This file contains the definition of timeline widget
 */

#ifndef PANEL_BAR_HPP
#define PANEL_BAR_HPP

#include <gtkmm.h>
#include "menu-button.hpp"

namespace gui {

namespace panels {
class Panel; 
}
  
namespace widgets {

/**
 * A container widget for widgets to be displayed on GDL panels grips.
 **/
class PanelBar : public Gtk::Box
{
public:

  /**
   * Constructor - creates a PanelBar with a given stock_id.
   * @param owner_panel The panel that is the parent of this panel bar.
   * @param stock_id The stock id with a name and an icon for this
   * panel.
   **/
  PanelBar(panels::Panel &owner_panel, const gchar *stock_id);
  
private:

  /**
   * Sets up panelButton, populating it with menu items.
   **/
  void setup_panel_button();

private:
  
  /**
   * An override to intercept realize events.
   **/
  void on_realize();

  /**
   * An event handler that is called to offer an allocation to this
   * widget.
   * @param requisition The area offered for this widget.
   */
  void on_size_request(Gtk::Requisition* requisition);

  /**
   * An override to intercept size allocate events.
   **/
  void on_size_allocate(Gtk::Allocation& allocation);
  
private:

  /**
   * An event handler for when a panel type is chosen.
   * @param type_index The index of the panel description that will be
   * instantiated.
   **/
  void on_panel_type(int type_index);

  /**
   * An event handler for when the "Hide" menu item is clicked
   **/
  void on_hide();
  
  /**
   * Event handler for when the "Lock" menu item is clicked
   **/
  void on_lock();
  
  /**
   * Event handler for when the split panel menu item is clicked
   * @param split_direction The direction to split in.
   **/
  void on_split_panel(Gtk::Orientation split_direction);

private:

  /**
   * A reference to the owner panel object.
   **/
  panels::Panel &panel;

  /**
   * The panel menu drop-down button widget, that will be displayed in
   * the corner of the bar.
   **/
  MenuButton panelButton;
  
  /**
   * A pointer to the lock menu item.
   * @remarks This value will remain NULL until after setup_panel_button
   * has been called.
   **/
  Gtk::CheckMenuItem *lockItem;
  
  /**
   * The bar window.
   * @remarks This window is used only to set the cursor as an arrow for
   * any child widgets.
   **/
  Glib::RefPtr<Gdk::Window> window;
};

} // gui
} // widgets

#endif // PANEL_BAR_HPP

