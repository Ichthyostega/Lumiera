/*
  timeline.cpp  -  Implementation of the timeline widget
 
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

#include <gdkmm/drawable.h>
#include <gdkmm/general.h>
#include <cairomm-1.0/cairomm/cairomm.h>

#include "timeline-widget.hpp"

namespace lumiera {
namespace gui {
namespace widgets {

TimelineWidget::TimelineWidget()
  {
    set_flags(Gtk::NO_WINDOW);
  }

void
TimelineWidget::on_realize()
  {
    //Call base class:
    Gtk::Widget::on_realize();

  }

bool
TimelineWidget::on_expose_event(GdkEventExpose* event)
  {
    // This is where we draw on the window
    Glib::RefPtr<Gdk::Window> window = get_window();
    if(window)
    {
      /*Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
      if(event)
      {
        // clip to the area that needs to be re-exposed so we don't draw any
        // more than we need to.
        cr->rectangle(event->area.x, event->area.y,
                event->area.width, event->area.height);
        cr->clip();
      }

      // Paint the background
      cr->set_source_rgb(0.0, 0.0, 0.0);
      cr->paint();*/
    }
    return true;
  }



}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

