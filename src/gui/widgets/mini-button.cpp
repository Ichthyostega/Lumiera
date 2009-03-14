/*
  mini-button.cpp  -  Implementation of the mini button widget
 
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

#include "mini-button.hpp"

#include <nobug.h>

using namespace Gtk;
using namespace sigc;

namespace gui {
namespace widgets {

MiniButton::MiniButton(const StockID& stock_id,
  const IconSize icon_size) :
  image(stock_id, icon_size)
{
  add(image);
  set_relief(RELIEF_NONE);
  set_focus_on_click(false);
}

void
MiniButton::set_stock_id(const StockID& stock_id,
  const IconSize icon_size)
{
  image.set(stock_id, icon_size);
}

} // widgets
} // gui
