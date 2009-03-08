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


class PanelBar : public Gtk::HBox
{
public:

  PanelBar(const gchar *name);

  PanelBar(const gchar *name, const gchar *stock_id);

  MenuButton panelButton;
  Gtk::Label test;
  
  //virtual bool 	on_button_press_event (GdkEventButton* event);
  
private:
  
  void internal_setup();
};

} // gui
} // widgets

#endif // PANEL_BAR_HPP

