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
#include "../timeline-widget.hpp"
#include "../../window-manager.hpp"

extern "C" {
#include "../../../lib/time.h"
}

using namespace Gtk;
using namespace Cairo;
using namespace std;
using namespace lumiera::gui;
using namespace lumiera::gui::widgets;
using namespace lumiera::gui::widgets::timeline;

namespace lumiera {
namespace gui {
namespace widgets {
namespace timeline {

TimelineRuler::TimelineRuler(
  lumiera::gui::widgets::TimelineWidget *timeline_widget) :
  Glib::ObjectBase("TimelineRuler"),
  mouseChevronOffset(0),
  annotationHorzMargin(0),
  annotationVertMargin(0),
  majorTickHeight(0),
  minorLongTickHeight(0),
  minorShortTickHeight(0),
  minDivisionWidth(100),
  mouseChevronSize(5),
  selectionChevronSize(5),
  timelineWidget(timeline_widget)
{
  REQUIRE(timelineWidget != NULL);
  
  // Install style properties
  register_styles();
}

void
TimelineRuler::set_mouse_chevron_offset(int offset)
{
  mouseChevronOffset = offset;
  queue_draw();
}

void
TimelineRuler::update_view()
{
  rulerImage.clear();
  queue_draw();
}

void
TimelineRuler::on_realize()
{
  Widget::on_realize();
  
  // Set event notifications
  add_events(Gdk::POINTER_MOTION_MASK | Gdk::SCROLL_MASK);

  // Load styles
  read_styles();
}

bool
TimelineRuler::on_expose_event(GdkEventExpose* event)
{
  REQUIRE(event != NULL);
  
  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(!window)
    return false;
    
  // Prepare to render via cairo      
  const Allocation allocation = get_allocation();

  Cairo::RefPtr<Context> cr = window->create_cairo_context();
  REQUIRE(cr);

  // Draw the ruler
  if(!rulerImage)
    {
      // We have no cached rendering - it must be redrawn
      // but do we need ro allocate a new image?
      if(!rulerImage ||
        rulerImage->get_width() != allocation.get_width() ||
        rulerImage->get_height() != allocation.get_height())
        rulerImage = ImageSurface::create(FORMAT_RGB24,
          allocation.get_width(), allocation.get_height());
          
      ENSURE(rulerImage);
        
      Cairo::RefPtr<Context> image_cairo = Context::create(rulerImage);
      ENSURE(image_cairo);
      
      draw_ruler(image_cairo, allocation);
    }
  
  // Draw the cached ruler image
  cr->set_source(rulerImage, 0, 0);
  cr->paint();
  
  // Draw the overlays
  draw_mouse_chevron(cr, allocation);
  draw_selection(cr, allocation);

  return true;
}

bool
TimelineRuler::on_motion_notify_event(GdkEventMotion *event)
{
  REQUIRE(event != NULL);

  set_mouse_chevron_offset(event->x);
  return true;
}

void
TimelineRuler::on_size_request (Gtk::Requisition *requisition)
{
  REQUIRE(requisition != NULL);
  
  // Initialize the output parameter
  *requisition = Gtk::Requisition();
  
  requisition->width = 0; 
  get_style_property("height", requisition->height);
}

void
TimelineRuler::on_size_allocate(Gtk::Allocation& allocation)
{
  Widget::on_size_allocate(allocation);
  rulerImage.clear(); // The widget has changed size - redraw
}

void
TimelineRuler::draw_ruler(Cairo::RefPtr<Cairo::Context> cr,
    const Gdk::Rectangle ruler_rect)
{
  REQUIRE(cr);
  REQUIRE(ruler_rect.get_width() > 0);
  REQUIRE(ruler_rect.get_height() > 0);
  REQUIRE(timelineWidget != NULL);
  
  const gavl_time_t left_offset = timelineWidget->timeOffset;
  const int64_t time_scale = timelineWidget->timeScale;
  
  // Preparation steps
  const int height = ruler_rect.get_height();
  Glib::RefPtr<Pango::Layout> pango_layout = create_pango_layout("");
  Glib::RefPtr<Style> style = get_style();
  
  // Render the background, and clip inside the area
  Gdk::Cairo::set_source_color(cr, style->get_bg(STATE_NORMAL));
  cr->rectangle(0, 0, 
    ruler_rect.get_width(), ruler_rect.get_height());
  cr->fill_preserve();
  cr->clip();
  
  // Make sure we don't have impossible zoom
  if(time_scale <= 0)
    return;
  
  // Render ruler annotations
  Gdk::Cairo::set_source_color(cr, style->get_fg(STATE_NORMAL));
  
  const gavl_time_t major_spacing = calculate_major_spacing();
  const gavl_time_t minor_spacing = major_spacing / 10;
  
  int64_t time_offset = left_offset - left_offset % major_spacing;
  if(left_offset < 0)
    time_offset -= major_spacing;
    
  int x = 0;
  const int64_t x_offset = left_offset / time_scale;

  do
    {    
      x = (int)(time_offset / time_scale - x_offset);
      
      cr->set_line_width(1);
      
      if(time_offset % major_spacing == 0)
        {
          // Draw the major grid-line
          cr->move_to(x + 0.5, height - majorTickHeight);
          cr->line_to(x + 0.5, height);
          cr->stroke();
          
          // Draw the text
          pango_layout->set_text(lumiera_tmpbuf_print_time(time_offset));
          cr->move_to(annotationHorzMargin + x, annotationVertMargin);
          pango_layout->add_to_cairo_context(cr);
          cr->fill();
        }
      else
        {
          // Draw the long or short minor grid-line
          if(time_offset % (minor_spacing * 2) == 0)           
            cr->move_to(x + 0.5, height - minorLongTickHeight);
          else
            cr->move_to(x + 0.5, height - minorShortTickHeight);
          
          cr->line_to(x + 0.5, height);
          cr->stroke();
        }
        
      time_offset += minor_spacing;
    }
  while(x < ruler_rect.get_width());
}

void
TimelineRuler::draw_mouse_chevron(Cairo::RefPtr<Cairo::Context> cr,
    const Gdk::Rectangle ruler_rect)
{
  REQUIRE(cr);
  REQUIRE(ruler_rect.get_width() > 0);
  REQUIRE(ruler_rect.get_height() > 0);
  
  // Is the mouse chevron in view?
  if(mouseChevronOffset < 0 ||
    mouseChevronOffset >= ruler_rect.get_width())
    return;
  
  // Set the source colour
  Glib::RefPtr<Style> style = get_style();
  Gdk::Cairo::set_source_color(cr, style->get_fg(STATE_NORMAL));
  
  cr->move_to(mouseChevronOffset + 0.5,
    ruler_rect.get_height());
  cr->rel_line_to(-mouseChevronSize, -mouseChevronSize);
  cr->rel_line_to(2 * mouseChevronSize, 0);
    
  cr->fill();
}

void
TimelineRuler::draw_selection(Cairo::RefPtr<Cairo::Context> cr,
  const Gdk::Rectangle ruler_rect)
{
  REQUIRE(cr);
  REQUIRE(ruler_rect.get_width() > 0);
  REQUIRE(ruler_rect.get_height() > 0);
  REQUIRE(timelineWidget != NULL);

  Glib::RefPtr<Style> style = get_style();
  Gdk::Cairo::set_source_color(cr, style->get_fg(STATE_NORMAL));
  
  // Draw the selection start chevron
  const int a = timelineWidget->time_to_x(
    timelineWidget->selectionStart) + 1;
  if(a >= 0 && a < ruler_rect.get_width())
    {
      cr->move_to(a, ruler_rect.get_height());
      cr->rel_line_to(0, -selectionChevronSize);
      cr->rel_line_to(-selectionChevronSize, 0);
      cr->fill();
    }
  
  // Draw the selection end chevron
  const int b = timelineWidget->time_to_x(
    timelineWidget->selectionEnd);
  if(b >= 0 && b < ruler_rect.get_width())
    {
      cr->move_to(b, ruler_rect.get_height());
      cr->rel_line_to(0, -selectionChevronSize);
      cr->rel_line_to(selectionChevronSize, 0);
      cr->fill();
    }
}

gavl_time_t 
TimelineRuler::calculate_major_spacing() const
{
  int i;
  
  REQUIRE(timelineWidget != NULL);
  
  const int64_t time_scale = timelineWidget->timeScale;
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
      const int64_t division_width = major_spacings[i] / time_scale;
      
      if(division_width > minDivisionWidth)
        break;
    }

  return major_spacings[i];
}

void
TimelineRuler::register_styles() const
{
  GtkWidgetClass *klass = GTK_WIDGET_CLASS(G_OBJECT_GET_CLASS(gobj()));

  gtk_widget_class_install_style_property(klass, 
    g_param_spec_int("height",
    "Height of the Ruler Widget",
    "The height of the ruler widget in pixels.",
    0, G_MAXINT, 18, G_PARAM_READABLE));
        
  gtk_widget_class_install_style_property(klass, 
    g_param_spec_int("major_tick_height",
    "Height of Major Ticks",
    "The length of major ticks in pixels.",
    0, G_MAXINT, 18, G_PARAM_READABLE));

  gtk_widget_class_install_style_property(klass, 
    g_param_spec_int("minor_long_tick_height",
    "Height of Long Minor Ticks",
    "The length of long minor ticks in pixels.",
    0, G_MAXINT, 6, G_PARAM_READABLE));
        
  gtk_widget_class_install_style_property(klass, 
    g_param_spec_int("minor_short_tick_height",
    "Height of Short Minor Ticks",
    "The length of short minor ticks in pixels.",
    0, G_MAXINT, 3, G_PARAM_READABLE));
    
  gtk_widget_class_install_style_property(klass, 
    g_param_spec_int("annotation_horz_margin",
    "Horizontal margin around annotation text",
    "The horizontal margin around the annotation text in pixels.",
    0, G_MAXINT, 3,
    G_PARAM_READABLE));

  gtk_widget_class_install_style_property(klass, 
    g_param_spec_int("annotation_vert_margin",
    "Vertical margin around annotation text",
    "The vertical margin around the annotation text in pixels.",
    0, G_MAXINT, 0, G_PARAM_READABLE));
        
  gtk_widget_class_install_style_property(klass, 
    g_param_spec_int("min_division_width",
    "Minimum Division Width",
    "The minimum distance in pixels that two major division may approach.",
    0, G_MAXINT, 100, G_PARAM_READABLE));

  gtk_widget_class_install_style_property(klass, 
    g_param_spec_int("mouse_chevron_size",
    "Mouse Chevron Size",
    "The height of the mouse chevron in pixels.",
    0, G_MAXINT, 5, G_PARAM_READABLE));
    
  gtk_widget_class_install_style_property(klass, 
    g_param_spec_int("selection_chevron_size",
    "Selection Chevron Size",
    "The height of the selection chevrons in pixels.",
    0, G_MAXINT, 5, G_PARAM_READABLE));
}

void
TimelineRuler::read_styles()
{
  get_style_property("annotation_horz_margin", annotationHorzMargin);
  get_style_property("annotation_vert_margin", annotationVertMargin);
  get_style_property("major_tick_height", majorTickHeight);
  get_style_property("minor_long_tick_height", minorLongTickHeight);
  get_style_property("minor_short_tick_height", minorShortTickHeight);
  get_style_property("min_division_width", minDivisionWidth);
  get_style_property("mouse_chevron_size", mouseChevronSize);
  get_style_property("selection_chevron_size", selectionChevronSize);
}
  
}   // namespace timeline
}   // namespace widgets
}   // namespace gui
}   // namespace lumiera
