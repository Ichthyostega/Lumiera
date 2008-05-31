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

#include <cairomm-1.0/cairomm/cairomm.h>

#include "timeline-body.hpp"

using namespace Gtk;
using namespace std;
using namespace lumiera::gui::widgets::timeline;

namespace lumiera {
namespace gui {
namespace widgets {
namespace timeline {

TimelineBody::TimelineBody(lumiera::gui::widgets::TimelineWidget &timeline_widget) :
    timelineWidget(timeline_widget)
  {
    timelineWidget.horizontalAdjustment.signal_value_changed().connect(
      sigc::mem_fun(this, &TimelineBody::on_scroll) );
    timelineWidget.verticalAdjustment.signal_value_changed().connect(
      sigc::mem_fun(this, &TimelineBody::on_scroll) );
  }

void
TimelineBody::on_scroll()
  {
    queue_draw();
  }

bool
TimelineBody::on_expose_event(GdkEventExpose* event)
  {
    // This is where we draw on the window
    Glib::RefPtr<Gdk::Window> window = get_window();
    if(window)
    {
      Gtk::Allocation allocation = get_allocation();
      const int width = allocation.get_width();
      const int height = allocation.get_height();

      Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
      cr->set_line_width(10.0);

      cr->translate(-horizontalAdjustment.get_value(), -verticalAdjustment.get_value());
      cr->save();

      vector<timeline::Track*>::iterator i;
      for(i = tracks.begin(); i != tracks.end(); i++)
      {
        timeline::Track *track = *i;
        g_assert(track != NULL);    
        track->draw_track();
      }

      cr->restore();
    }

    return true;

  }

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

