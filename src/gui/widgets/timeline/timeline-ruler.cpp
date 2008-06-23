/*
  timeline-ruler.cpp  -  Implementation of the time ruler widget
 
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

#include "timeline-ruler.hpp"
#include "../../window-manager.hpp"

extern "C" {
#include "../../../lib/time.h"
}

using namespace Gtk;
using namespace std;
using namespace lumiera::gui;
using namespace lumiera::gui::widgets;
using namespace lumiera::gui::widgets::timeline;

namespace lumiera {
namespace gui {
namespace widgets {
namespace timeline {

TimelineRuler::TimelineRuler() :
  Glib::ObjectBase("TimelineRuler"),
  timeOffset(0),
  timeScale(1)  
{
  set_flags(Gtk::NO_WINDOW);  // This widget will not have a window
  set_size_request(-1, 20);
}

void
TimelineRuler::set_time_offset(gavl_time_t time_offset)
{
  timeOffset = time_offset;
  queue_draw();
}

void
TimelineRuler::set_time_scale(int64_t time_scale)
{
  REQUIRE(time_scale > 0);
  timeScale = time_scale;
  queue_draw();
}

bool
TimelineRuler::on_expose_event(GdkEventExpose* event)
{
  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(!window)
    return false;

  // Prepare to render via cairo      
  Allocation allocation = get_allocation();
  Glib::RefPtr<Style> style = get_style();
  Cairo::RefPtr<Cairo::Context> cairo = window->create_cairo_context();
  Glib::RefPtr<Pango::Layout> pango_layout = create_pango_layout("");

  cairo->translate(allocation.get_x(), allocation.get_y());

  // Render the background, and clip inside the area
  Gdk::Cairo::set_source_color(cairo, style->get_bg(STATE_NORMAL));
  cairo->rectangle(0, 0, allocation.get_width(), allocation.get_height());
  cairo->fill_preserve();
  cairo->clip();
  
  // Make sure we don't have impossible zoom
  if(timeScale <= 0)
    return true;
  
  // Render ruler annotations
  Gdk::Cairo::set_source_color(cairo, style->get_fg(STATE_NORMAL));
  
  gavl_time_t major_spacing = GAVL_TIME_SCALE;
  
  int64_t time_offset = timeOffset - timeOffset % major_spacing;
  int x = 0;
  const int64_t x_offset = timeOffset / timeScale;

  do
    {    
      x = (int)(time_offset / timeScale - x_offset);
      
      // Draw the major grid-line
      cairo->move_to(x + 0.5, 0);
      cairo->line_to(x + 0.5, allocation.get_height());
      cairo->set_line_width(1);
      cairo->stroke();
      
      // Draw the text
      pango_layout->set_text(lumiera_tmpbuf_print_time(time_offset));
      Pango::Rectangle text_extents = pango_layout->get_logical_extents();   
      cairo->move_to(4 + x, (allocation.get_height() - text_extents.get_height() / Pango::SCALE) / 2);
      pango_layout->add_to_cairo_context(cairo);
      cairo->fill();
      
      time_offset += major_spacing;
    }
  while(x < allocation.get_width());

  return true;
}
  
}   // namespace timeline
}   // namespace widgets
}   // namespace gui
}   // namespace lumiera
