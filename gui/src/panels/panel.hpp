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
      Panel(const gchar *name, const gchar *long_name,
        GdlDockItemBehavior behavior = GDL_DOCK_ITEM_BEH_NORMAL);
      ~Panel();

    public:
      GdlDockItem* get_dock_item() const;


    protected:
      GdlDockItem* dock_item;
    };

}   // namespace panels
}   // namespace gui
}   // namespace lumiera

#endif // PANEL_HPP
