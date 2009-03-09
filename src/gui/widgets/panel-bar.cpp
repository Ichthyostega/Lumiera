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
 
* *****************************************************/

#include "panel-bar.hpp"

#include <nobug.h>

using namespace Gtk;
using namespace sigc;

namespace gui {
namespace widgets {
  
PanelBar::PanelBar(const gchar *name) :
  HBox(),
  panelButton(Glib::ustring(name))
{
  internal_setup();
  
    // Make the widget sensitive to mouse events
  /*add_events(
    Gdk::POINTER_MOTION_MASK |
    Gdk::BUTTON_PRESS_MASK |
    Gdk::BUTTON_RELEASE_MASK);*/
}

PanelBar::PanelBar(const gchar *name, const gchar *stock_id) :
  HBox(),
  panelButton(Gtk::StockID(stock_id))
{
  internal_setup();
}

void
PanelBar::internal_setup()
{
  panelButton.set_relief(RELIEF_NONE);
  panelButton.unset_flags(CAN_FOCUS);
  panelButton.show();
  pack_start(panelButton, PACK_SHRINK);
}

/*bool PanelBar::on_button_press_event (GdkEventButton* event)
{
  g_message("Hello");
  return EventBox::on_button_press_event(event);
}*/
} // widgets
} // gui
