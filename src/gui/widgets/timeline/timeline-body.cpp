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
#include <boost/foreach.hpp>

#include "timeline-body.hpp"
#include "../timeline-widget.hpp"
#include "../../window-manager.hpp"

#include "timeline-arrow-tool.hpp"
#include "timeline-ibeam-tool.hpp"

using namespace Gtk;
using namespace std;
using namespace lumiera::gui;
using namespace lumiera::gui::widgets;
using namespace lumiera::gui::widgets::timeline;

namespace lumiera {
namespace gui {
namespace widgets {
namespace timeline {

TimelineBody::TimelineBody(lumiera::gui::widgets::TimelineWidget
  *timeline_widget) :
    Glib::ObjectBase("TimelineBody"),
    tool(NULL),
    dragType(None),
    mouseDownX(0),
    mouseDownY(0),
    beginShiftTimeOffset(0),
    selectionAlpha(0.5),
    timelineWidget(timeline_widget)
{
  REQUIRE(timelineWidget != NULL);
      
  // Connect up some events  
  timelineWidget->horizontalAdjustment.signal_value_changed().connect(
    sigc::mem_fun(this, &TimelineBody::on_scroll) );
  timelineWidget->verticalAdjustment.signal_value_changed().connect(
    sigc::mem_fun(this, &TimelineBody::on_scroll) );
  
  // Install style properties
  register_styles();
}

TimelineBody::~TimelineBody()
{
  REQUIRE(tool != NULL);
  if(tool != NULL)
    delete tool;
}

ToolType
TimelineBody::get_tool() const
{
  REQUIRE(tool != NULL);
  if(tool != NULL)
    return tool->get_type();
  return lumiera::gui::widgets::timeline::None;
}
  
void
TimelineBody::set_tool(timeline::ToolType tool_type)
{  
  // Tidy up old tool
  if(tool != NULL)
    {
      // Do we need to change tools?
      if(tool->get_type() == tool_type)
        return;
        
      delete tool;
    }
  
  // Create the new tool
  switch(tool_type)
    {
    case timeline::Arrow:
      tool = new ArrowTool(this);
      break;
      
    case timeline::IBeam:
      tool = new IBeamTool(this);
      break;
    }
    
  // Apply the cursor if possible
  tool->apply_cursor();
}

void
TimelineBody::on_realize()
{
  Widget::on_realize();
  
  // We wish to receive event notifications
  add_events(
    Gdk::POINTER_MOTION_MASK |
    Gdk::SCROLL_MASK |
    Gdk::BUTTON_PRESS_MASK |
    Gdk::BUTTON_RELEASE_MASK);
    
  // Apply the cursor if possible
  tool->apply_cursor();
}

void
TimelineBody::on_scroll()
{
  queue_draw();
}
  
bool
TimelineBody::on_scroll_event (GdkEventScroll* event)
{
  REQUIRE(event != NULL);
  REQUIRE(timelineWidget != NULL);
  
  if(event->state & GDK_CONTROL_MASK)
  {
    switch(event->direction)
    {
    case GDK_SCROLL_UP:
      // User scrolled up. Zoom in
      timelineWidget->zoom_view(event->x, 1);
      break;
      
    case GDK_SCROLL_DOWN:
      // User scrolled down. Zoom out
      timelineWidget->zoom_view(event->x, -1);
      break;    
    }
  }
  else
  {
    switch(event->direction)
    {
    case GDK_SCROLL_UP:
      // User scrolled up. Shift 1/16th left
      timelineWidget->shift_view(-1);
      break;
      
    case GDK_SCROLL_DOWN:
      // User scrolled down. Shift 1/16th right
      timelineWidget->shift_view(1);
      break;    
    }
  }
}

bool
TimelineBody::on_button_press_event(GdkEventButton* event)
{
  mouseDownX = event->x;
  mouseDownY = event->y;
  
  switch(event->button)
  {
  case 2:
    begin_shift_drag();
    break;
    
  default:
    dragType = None;
    break;
  }
  
  // Forward the event to the tool
  tool->on_button_press_event(event);
  
  return true;
}
  
bool
TimelineBody::on_button_release_event(GdkEventButton* event)
{
  // Terminate any drags
  dragType = None;
  
  // Forward the event to the tool
  tool->on_button_release_event(event);
  
  return true;
}

bool
TimelineBody::on_motion_notify_event(GdkEventMotion *event)
{
  REQUIRE(event != NULL);
    
  switch(dragType)
  {
  case Shift:
    {
      const int64_t scale = timelineWidget->get_time_scale();
      gavl_time_t offset = beginShiftTimeOffset +
        (int64_t)(mouseDownX - event->x) * scale;
      timelineWidget->set_time_offset(offset);
      
      set_vertical_offset((int)(mouseDownY - event->y) +
        beginShiftVerticalOffset);
      break;
    }
  }
  
  // Forward the event to the tool
  tool->on_motion_notify_event(event);
  
  // false so that the message is passed up to the owner TimelineWidget
  return false;
}

bool
TimelineBody::on_expose_event(GdkEventExpose* event)
{
  Cairo::Matrix view_matrix;
  
  REQUIRE(event != NULL);
  REQUIRE(timelineWidget != NULL);
  
  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(!window)
    return false;
  
  // Makes sure the widget styles have been loaded
  read_styles();
  
  // Prepare to render via cairo
  Glib::RefPtr<Style> style = get_style();  
  const Allocation allocation = get_allocation();
  Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
  
  REQUIRE(style);
  REQUIRE(cr);
  
  // Translate the view by the scroll distance
  cr->translate(0, -get_vertical_offset());
  cr->get_matrix(view_matrix);
  
  // Interate drawing each track
  BOOST_FOREACH( Track* track, timelineWidget->tracks )
    {
      ASSERT(track != NULL);

      const int height = track->get_height();
      ASSERT(height >= 0);
    
      // Draw the track background
      cr->rectangle(0, 0, allocation.get_width(), height);
      gdk_cairo_set_source_color(cr->cobj(), &backgroundColour);
      cr->fill();
    
      // Render the track
      cr->save();
      track->draw_track(cr);
      cr->restore();
      
      // Shift for the next track
      cr->translate(0, height + TimelineWidget::TrackPadding);
    }
    
  //----- Draw the selection -----//
  const int start_x = timelineWidget->time_to_x(
    timelineWidget->get_selection_start());
  const int end_x = timelineWidget->time_to_x(
    timelineWidget->get_selection_end());

  cr->set_matrix(view_matrix);
  
  // Draw the cover
  if(end_x > 0 && start_x < allocation.get_width())
    {
      cr->set_source_rgba(
        (float)selectionColour.red / 0xFFFF,
        (float)selectionColour.green / 0xFFFF,
        (float)selectionColour.blue / 0xFFFF,
        selectionAlpha);
      cr->rectangle(start_x + 0.5, 0,
        end_x - start_x, allocation.get_height());
      cr->fill();
    }
  
  gdk_cairo_set_source_color(cr->cobj(), &selectionColour);
  cr->set_line_width(1);
  
  // Draw the start
  if(start_x >= 0 && start_x < allocation.get_width())
    {
      cr->move_to(start_x + 0.5, 0);
      cr->line_to(start_x + 0.5, allocation.get_height());
      cr->stroke_preserve();
    }
    
  // Draw the end
  if(end_x >= 0 && end_x < allocation.get_width())
    {
      cr->move_to(end_x + 0.5, 0);
      cr->line_to(end_x + 0.5, allocation.get_height());
      cr->stroke_preserve();
    } 
  
  return true;
}

void
TimelineBody::begin_shift_drag()
{
  dragType = Shift;
  beginShiftTimeOffset = timelineWidget->get_time_offset();
  beginShiftVerticalOffset = get_vertical_offset();
}

int
TimelineBody::get_vertical_offset() const
{
  return (int)timelineWidget->verticalAdjustment.get_value();
}

void
TimelineBody::set_vertical_offset(int offset)
{
  timelineWidget->verticalAdjustment.set_value(offset);
}

void
TimelineBody::register_styles() const
{
  GtkWidgetClass *klass = GTK_WIDGET_CLASS(G_OBJECT_GET_CLASS(gobj()));

  gtk_widget_class_install_style_property(
    GTK_WIDGET_CLASS(G_OBJECT_GET_CLASS(gobj())), 
    g_param_spec_boxed("background",
      "Track Background",
      "The background colour of timeline tracks",
      GDK_TYPE_COLOR, G_PARAM_READABLE));
  
  gtk_widget_class_install_style_property(
    GTK_WIDGET_CLASS(G_OBJECT_GET_CLASS(gobj())), 
    g_param_spec_boxed("selection",
      "End lines of a selection",
      "The colour of selection limit lines",
      GDK_TYPE_COLOR, G_PARAM_READABLE));
      
  gtk_widget_class_install_style_property(klass, 
    g_param_spec_float("selection_alpha",
    "Selection Alpha",
    "The transparency of the selection marque.",
    0, 1.0, 0.5, G_PARAM_READABLE));
}
  
void
TimelineBody::read_styles()
{
  backgroundColour = WindowManager::read_style_colour_property(
    *this, "background", 0, 0, 0);
  selectionColour = WindowManager::read_style_colour_property(
    *this, "selection", 0, 0, 0);
    
  get_style_property("selection_alpha", selectionAlpha);
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
}   // namespace lumiera
