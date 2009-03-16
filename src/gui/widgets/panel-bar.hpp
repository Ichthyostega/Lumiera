/*
  panel-bar.hpp  -  Declaration of the panel bar widget
 
  Copyright (C)         Lumiera.org
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>
 
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
/** @file panel-bar.hpp
 ** This file contains the definition of timeline widget
 */

#ifndef PANEL_BAR_HPP
#define PANEL_BAR_HPP

#include <gtkmm.h>
#include "menu-button.hpp"

namespace gui {
namespace widgets {

/**
 * A container widget for widgets to be displayed on GDL panels grips.
 **/
class PanelBar : public Gtk::HBox
{
public:

  /**
   * Constructor - creates a PanelBar with a given stock_id.
   * @param stock_id The stock id with a name and an icon for this
   * panel.
   **/
  PanelBar(const gchar *stock_id);
  
private:
  
  /**
   * A helper function to do the internal setup of the widget.
   **/
  void internal_setup();
  
private:

  /**
   * The panel menu drop-down button widget, that will be displayed in
   * the corner of the bar.
   **/
  MenuButton panelButton;
};

} // gui
} // widgets

#endif // PANEL_BAR_HPP

