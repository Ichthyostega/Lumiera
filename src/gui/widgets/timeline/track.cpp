/*
  track.cpp  -  Implementation of the timeline track object
 
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
 
* *****************************************************/

#include "track.hpp"

using namespace Gtk;

namespace lumiera {
namespace gui {
namespace widgets {
namespace timeline {

Track::Track() :
  label1("test1"), label2("test2"), label3("test3"), label4("test4")
{
  headerWidget.pack_start(label1, PACK_EXPAND_WIDGET);
  headerWidget.pack_start(label2, PACK_EXPAND_WIDGET);
  headerWidget.pack_start(label3, PACK_EXPAND_WIDGET);
  headerWidget.pack_start(label4, PACK_EXPAND_WIDGET);
}

Gtk::Widget&
Track::get_header_widget()
{
  return headerWidget;
}

Glib::ustring
Track::get_title()
{
  return "Hello";
}

int
Track::get_height()
{
  return 100;
}

void
Track::draw_track(Cairo::RefPtr<Cairo::Context> cairo)
{
  
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

