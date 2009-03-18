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
#include "../panels/panel.hpp"
#include "../util/rectangle.hpp"

#include <nobug.h>
#include <algorithm>

using namespace Gtk;
using namespace Glib;
using namespace sigc;
using namespace std;

namespace gui {
namespace widgets {

PanelBar::PanelBar(panels::Panel &owner_panel, const gchar *stock_id) :
  HBox(),
  panel(owner_panel),
  panelButton(StockID(stock_id))
{  
  panelButton.set_relief(RELIEF_NONE);
  panelButton.unset_flags(CAN_FOCUS);
  panelButton.show();
  pack_start(panelButton, PACK_SHRINK);
  
  setup_panel_button();
}

void
PanelBar::setup_panel_button()
{
  Menu& menu = panelButton.get_menu();
  Menu::MenuList& list = menu.items();

  list.push_back( Menu_Helpers::MenuElem(_("_Hide"),
    mem_fun(*this, &PanelBar::on_hide) ) );
}

void
PanelBar::on_realize()
{
  set_flags(Gtk::NO_WINDOW);
  
  // Call base class:
  Gtk::Container::on_realize();
  
  // Create the GdkWindow:
  GdkWindowAttr attributes;
  memset(&attributes, 0, sizeof(attributes));

  const Allocation allocation(get_allocation());

  // Set initial position and size of the Gdk::Window:
  attributes.x = allocation.get_x();
  attributes.y = allocation.get_y();
  attributes.width = allocation.get_width();
  attributes.height = allocation.get_height();

  attributes.event_mask = GDK_ALL_EVENTS_MASK; 
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.wclass = GDK_INPUT_OUTPUT;

  window = Gdk::Window::create(get_window(), &attributes,
          GDK_WA_X | GDK_WA_Y);
  
  window->set_user_data(gobj());
  window->set_cursor(Gdk::Cursor(Gdk::LEFT_PTR));
  
  set_window(window);
  unset_flags(Gtk::NO_WINDOW);
  
  unset_bg(STATE_NORMAL);
}

void
PanelBar::on_size_allocate(Gtk::Allocation& allocation)
{
  if(window)
    {
      const Requisition requisition(get_requisition());
      const int width = max(min(requisition.width,
        allocation.get_width()), 0);
      window->move_resize(allocation.get_x(), allocation.get_y(),
        width, allocation.get_height());
    }
  
  allocation.set_x(0);
  HBox::on_size_allocate(allocation);
}

void
PanelBar::on_hide()
{
  panel.show(false);
}

} // widgets
} // gui
