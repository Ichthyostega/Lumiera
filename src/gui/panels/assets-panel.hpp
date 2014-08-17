/*
  assets-panel.hpp  -  Definition of the assets panel            

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
/** @file assets-panel.hpp
 ** This file contains the definition of the assets panel
 */

#ifndef ASSETS_PANEL_HPP
#define ASSETS_PANEL_HPP


#include "gui/panels/panel.hpp"

namespace gui {
namespace panels {

class AssetsPanel : public Panel
{
public:
  /**
   * Contructor.
   * @param panel_manager The owner panel manager widget.
   * @param dock_item The GdlDockItem that will host this panel.
   */
  AssetsPanel(workspace::PanelManager &panel_manager,
    GdlDockItem *dock_item);
  
  /**
   * Get the title of the panel.
   * @return Returns a pointer to the string title of the panel.
   */
  static const char* get_title();

  /**
   * Get the stock id for this type panel.
   * @return Returns a pointer to the string stock id of the panel.
   */
  static const gchar* get_stock_id();

protected:
  Gtk::Notebook notebook;
  
  Gtk::IconView media;
  Gtk::IconView clips;
  Gtk::IconView effects;
  Gtk::IconView transitions;
};

}   // namespace panels
}   // namespace gui

#endif // ASSETS_PANEL_HPP
