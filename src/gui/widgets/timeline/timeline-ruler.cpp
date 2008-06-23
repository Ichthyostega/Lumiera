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
  timeScale(1),
  annotationHorzMargin(0),
  annotationVertMargin(0)
{
  set_flags(Gtk::NO_WINDOW);  // This widget will not have a window
  set_size_request(-1, 20);
  
  // Install style properties
  gtk_widget_class_install_style_property(
    GTK_WIDGET_CLASS(G_OBJECT_GET_CLASS(gobj())), 
      g_param_spec_int("annotation_horz_margin",
        "Horizontal margin around annotation text",
        "The horiztontal margin around the annotation text in pixels.",
        G_MININT, G_MAXINT, 4,
        G_PARAM_READABLE));

  gtk_widget_class_install_style_property(
    GTK_WIDGET_CLASS(G_OBJECT_GET_CLASS(gobj())), 
      g_param_spec_int("annotation_vert_margin",
        "Vertical margin around annotation text",
        "The vertical margin around the annotation text in pixels.",
        G_MININT, G_MAXINT, 4,
        G_PARAM_READABLE));
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
    
  // Load styles
  read_styles();

  // Prepare to render via cairo      
  Allocation allocation = get_allocation();
  Glib::RefPtr<Style> style = get_style();
  Cairo::RefPtr<Cairo::Context> cairo = window->create_cairo_context();
  Glib::RefPtr<Pango::Layout> pango_layout = create_pango_layout("");

  cairo->translate(allocation.get_x(), allocation.get_y());

  // Render the background, and clip inside the area
  Gdk::Cairo::set_source_color(cairo, style->get_bg(STATE_NORMAL));
  cairo->rectangle(0, 0, 
    allocation.get_width(), allocation.get_height());
  cairo->fill_preserve();
  cairo->clip();
  
  // Make sure we don't have impossible zoom
  if(timeScale <= 0)
    return true;
  
  // Render ruler annotations
  Gdk::Cairo::set_source_color(cairo, style->get_fg(STATE_NORMAL));
  
  const gavl_time_t major_spacing = calculate_major_spacing();
  const gavl_time_t minor_spacing = major_spacing / 10;
  
  int64_t time_offset = timeOffset - timeOffset % minor_spacing;
  int x = 0;
  const int64_t x_offset = timeOffset / timeScale;

  do
    {    
      x = (int)(time_offset / timeScale - x_offset);
      
      cairo->set_line_width(1);
      
      if(time_offset % major_spacing == 0)
        {
          
          // Draw the major grid-line
          cairo->move_to(x + 0.5, 0);
          cairo->line_to(x + 0.5, allocation.get_height());
          cairo->stroke();
          
          // Draw the text
          pango_layout->set_text(lumiera_tmpbuf_print_time(time_offset));
          //Pango::Rectangle text_extents = pango_layout->get_logical_extents();
          //const int text_height = text_extents.get_height() / Pango::SCALE;
          cairo->move_to(annotationHorzMargin + x, annotationVertMargin);
          pango_layout->add_to_cairo_context(cairo);
          cairo->fill();
        }
      else
        {
          // Draw the minor grid-line
          if(time_offset % (minor_spacing * 2) == 0)           
            cairo->move_to(x + 0.5, (4 * allocation.get_height()) / 5);
          else
            cairo->move_to(x + 0.5, (3 * allocation.get_height()) / 5);
          
          cairo->line_to(x + 0.5, allocation.get_height());
          cairo->stroke();
        }
        
      time_offset += minor_spacing;
    }
  while(x < allocation.get_width());

  return true;
}

gavl_time_t 
TimelineRuler::calculate_major_spacing() const
{
  int i = 0;
  const int min_division_width = 100;
  
  const gavl_time_t major_spacings[] = {
      GAVL_TIME_SCALE / 1000,    
      GAVL_TIME_SCALE / 400,
      GAVL_TIME_SCALE / 200,
      GAVL_TIME_SCALE / 100,    
      GAVL_TIME_SCALE / 40,
      GAVL_TIME_SCALE / 20,
      GAVL_TIME_SCALE / 10,    
      GAVL_TIME_SCALE / 4,
      GAVL_TIME_SCALE / 2,
      GAVL_TIME_SCALE,
      2l * GAVL_TIME_SCALE,
      5l * GAVL_TIME_SCALE,
      10l * GAVL_TIME_SCALE,
      15l * GAVL_TIME_SCALE,
      30l * GAVL_TIME_SCALE,
      60l * GAVL_TIME_SCALE,
      2l * 60l * GAVL_TIME_SCALE, 
      5l * 60l * GAVL_TIME_SCALE,
      10l * 60l * GAVL_TIME_SCALE,
      15l * 60l * GAVL_TIME_SCALE,
      30l * 60l * GAVL_TIME_SCALE,
      60l * 60l * GAVL_TIME_SCALE
    };     
  
  for(i = 0; i < sizeof(major_spacings) / sizeof(gavl_time_t); i++)
    {
      const int64_t division_width = major_spacings[i] / timeScale;
      
      if(division_width > min_division_width)
        break;
    }

  return major_spacings[i];
}

void
TimelineRuler::read_styles()
{
  get_style_property("annotation_horz_margin", annotationHorzMargin);
  get_style_property("annotation_vert_margin", annotationVertMargin);  
}
  
}   // namespace timeline
}   // namespace widgets
}   // namespace gui
}   // namespace lumiera
