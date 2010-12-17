/*
  menu-button.cpp  -  Implementation of the menu button widget

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

#include "menu-button.hpp"

#include <nobug.h>

using namespace Gtk;
using namespace sigc;

const int CaptionPadding = 4;

namespace gui {
namespace widgets {
  
const ArrowType arrowType = ARROW_DOWN;
const ShadowType shadowType = SHADOW_NONE;
  
MenuButton::MenuButton() :
  ToggleButton(),
  arrow(arrowType, shadowType)
{
  setup_button();
}

MenuButton::MenuButton(const StockID& stock_id) :
  ToggleButton(),
  image(stock_id, ICON_SIZE_MENU),
  caption(),
  arrow(arrowType, shadowType)
{
  StockItem stock_item;
  REQUIRE(StockItem::lookup(stock_id, stock_item));
  caption.set_text_with_mnemonic(stock_item.get_label());

  hBox.pack_start(image);

  setup_button();
}  

MenuButton::MenuButton(const Glib::ustring& label, bool mnemonic) :
  ToggleButton(),
  caption(label, mnemonic),
  arrow(arrowType, shadowType)
{
  setup_button();
}

Gtk::Menu&
MenuButton::get_menu()
{
  return menu;
}

void
MenuButton::popup()
{
  menu.popup( mem_fun(this, &MenuButton::on_menu_position),
    0, gtk_get_current_event_time());
  set_active();
}

void
MenuButton::setup_button()
{
  menu.signal_deactivate().connect(mem_fun(
    this, &MenuButton::on_menu_deactivated));
  
  arrow.set(ARROW_DOWN, SHADOW_NONE);
  
  hBox.pack_start(caption, PACK_EXPAND_WIDGET, CaptionPadding);
  hBox.pack_start(arrow);

  add(hBox);
  show_all();
}

void
MenuButton::on_pressed()
{
  popup();
}

void
MenuButton::on_menu_deactivated()
{
  set_active(false);
}

void
MenuButton::on_menu_position(int& x, int& y, bool& push_in)
{
  Glib::RefPtr<Gdk::Window> window = get_window();
  REQUIRE(window);
  
  window->get_origin(x, y);
  const Allocation allocation = get_allocation();
	x += allocation.get_x();
	y += allocation.get_y() + allocation.get_height();
	
	push_in = true;
}

} // widgets
} // gui
