/*
  button-bar.cpp  -  Implementation of the button bar widget
 
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

#include "button-bar.hpp"

#include <nobug.h>

using namespace Gtk;
using namespace sigc;

namespace gui {
namespace widgets {

ButtonBar::ButtonBar()
{
}

void
ButtonBar::append(MiniButton& button)
{
  pack_start(button, PACK_SHRINK);
}

void
ButtonBar::append(MiniButton& button,
  const sigc::slot<void>& clicked_slot)
{
  button.signal_clicked().connect(clicked_slot);
  append(button);
}

void
ButtonBar::append(SeparatorToolItem &seperator)
{
  pack_start(seperator, PACK_SHRINK);
}

} // widgets
} // gui
