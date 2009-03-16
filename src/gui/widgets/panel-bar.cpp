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
#include "../util/rectangle.hpp"

#include <nobug.h>

using namespace Gtk;
using namespace Glib;
using namespace sigc;

const int DragHandleSize = 10;

namespace gui {
namespace widgets {

PanelBar::PanelBar(const gchar *stock_id) :
  HBox(),
  panelButton(StockID(stock_id))
{
  panelButton.set_relief(RELIEF_NONE);
  panelButton.unset_flags(CAN_FOCUS);
  panelButton.show();
  pack_start(panelButton, PACK_SHRINK);
}

bool
PanelBar::on_expose_event(GdkEventExpose* event)
{
  HBox::on_expose_event(event);
  
  RefPtr<const Style> style = get_style();
  REQUIRE(style);
  
  const Allocation allocation(get_allocation());
  const Gdk::Rectangle rect(
    allocation.get_x() - DragHandleSize, allocation.get_y(),
    DragHandleSize, allocation.get_height());
    
  style->paint_handle(get_window(), STATE_NORMAL, SHADOW_NONE,
    rect, *this, "handlebox",  rect.get_x(), rect.get_y(),
    rect.get_width(), rect.get_height(), ORIENTATION_VERTICAL);
  
  return true;
}

void
PanelBar::on_size_allocate(Gtk::Allocation& allocation)
{
  allocation.set_x(allocation.get_x() + DragHandleSize);
  HBox::on_size_allocate(allocation);
}
  
void
PanelBar::on_size_request(Gtk::Requisition* requisition)
{
  REQUIRE(requisition);
    
  HBox::on_size_request(requisition);
  
  requisition->width += DragHandleSize;
}

} // widgets
} // gui
