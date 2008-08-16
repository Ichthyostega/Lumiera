/*
  panel.hpp  -  Definition of Panel, the base class for docking panels            
 
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
 
*/
/** @file panel.hpp
 ** This file contains the definition of Panel; the base class
 ** for all docking panels
 */

#ifndef PANEL_HPP
#define PANEL_HPP

#include <gtkmm.h>
#include <libgdl-1.0/gdl/gdl-dock-item.h>

namespace lumiera {
namespace gui {
namespace panels {

/** 
* The main lumiera workspace window
*/
class Panel : public Gtk::VBox
{
protected:
  /**
   *  Constructs a panel object
   *  @param name The internal name of this panel
   *  @param long_name The name to display on the caption
   *  @param behavior The GDL behaviour of this item
   */
  Panel(const gchar *name, const gchar *long_name,
    GdlDockItemBehavior behavior = GDL_DOCK_ITEM_BEH_NORMAL);

  /**
   *  Constructs a panel object with a stock item for a caption
   *  @param name The internal name of this panel
   *  @param long_name The name to display on the caption
   *  @param stock_id The id of the stock item to display on the caption
   *  @param behavior The GDL behaviour of this item
   */
  Panel(const gchar *name, const gchar *long_name, const gchar *stock_id,
    GdlDockItemBehavior behavior = GDL_DOCK_ITEM_BEH_NORMAL);

  ~Panel();

public:
  /**
   *  Returns a pointer to the underlying GdlDockItem structure
   */
  GdlDockItem* get_dock_item() const;

  /**
   *  Shows or hides the panel.
   *  @param show A value of true will show the panel,
   *    false will hide it. */
  void show(bool show = true);
  
  /**
   *  Returns true if the panel is currently visible.
   */
  bool is_shown() const;

private:
  /**
   *  @internal Used by both constructors
   *  The internal constructor for this class, whose purpose
   *  is to set up the internal container widgets.
   */
  void internal_setup();

protected:
  GdlDockItem* dock_item;
};

}   // namespace panels
}   // namespace gui
}   // namespace lumiera

#endif // PANEL_HPP
