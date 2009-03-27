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
#include "include/logging.h"

using namespace Gtk;
using namespace std;
using namespace boost;

namespace gui {
namespace widgets {
namespace timeline {

TimelineBody::TimelineBody(TimelineWidget &timeline_widget) :
    Glib::ObjectBase("TimelineBody"),
    tool(NULL),
    mouseDownX(0),
    mouseDownY(0),
    dragType(None),
    beginShiftTimeOffset(0),
    selectionAlpha(0.5),
    timelineWidget(timeline_widget)
{      
  // Connect up some events
  timeline_widget.state_changed_signal().connect(
    sigc::mem_fun(this, &TimelineBody::on_state_changed) );
  
  // Install style properties
  register_styles();
  
  // Reset the state
  on_state_changed();
}

TimelineBody::~TimelineBody()
{
  WARN_IF(!tool, gui, "An invalid tool pointer is unexpected here");
}

ToolType
TimelineBody::get_tool() const
{
  REQUIRE(tool);
  return tool->get_type();
}
  
void
TimelineBody::set_tool(timeline::ToolType tool_type)
{  
  // Tidy up old tool
  if(tool)
    {
      // Do we need to change tools?
      if(tool->get_type() == tool_type)
        return;
    }
  
  // Create the new tool
  switch(tool_type)
    {
    case timeline::Arrow:
      tool.reset(new ArrowTool(*this));
      break;
      
    case timeline::IBeam:
      tool.reset(new IBeamTool(*this));
      break;
      
    default:
      break;
    }
    
  // Apply the cursor if possible
  tool->apply_cursor();
}

void
TimelineBody::on_update_view()
{
  queue_draw();
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

bool
TimelineBody::on_expose_event(GdkEventExpose* event)
{
  REQUIRE(event != NULL);
  
  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(!window)
    return false;
  
  // Makes sure the widget styles have been loaded
  read_styles();
  
  // Prepare to render via cairo
  const Allocation allocation = get_allocation();
  Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

  REQUIRE(cr);
  
  //----- Draw the view -----//
  draw_tracks(cr);
  draw_selection(cr);
  draw_playback_point(cr);
  
  return true;
}
  
bool
TimelineBody::on_scroll_event (GdkEventScroll* event)
{
  REQUIRE(event != NULL);
  
  TimelineViewWindow &window = view_window();
  const Allocation allocation = get_allocation();
  
  if(event->state & GDK_CONTROL_MASK)
  {
    switch(event->direction)
    {
    case GDK_SCROLL_UP:
      // User scrolled up. Zoom in
      window.zoom_view(event->x, 1);
      break;
      
    case GDK_SCROLL_DOWN:
      // User scrolled down. Zoom out
      window.zoom_view(event->x, -1);
      break;
      
    default:
      break;
    }
  }
  else
  {
    switch(event->direction)
    {
    case GDK_SCROLL_UP:
      // User scrolled up. Shift 1/16th left
      window.shift_view(allocation.get_width(), -16);
      break;
      
    case GDK_SCROLL_DOWN:
      // User scrolled down. Shift 1/16th right
      window.shift_view(allocation.get_width(), 16);
      break;
    
    default:
      break;
    }
  }
  
  return true;
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
  
  // Handle a middle-mouse drag if one is occuring
  switch(dragType)
    {
    case Shift:
      {
        TimelineViewWindow &window = view_window();
        
        const int64_t scale = window.get_time_scale();
        gavl_time_t offset = beginShiftTimeOffset +
          (int64_t)(mouseDownX - event->x) * scale;
        window.set_time_offset(offset);
        
        set_vertical_offset((int)(mouseDownY - event->y) +
          beginShiftVerticalOffset);
        break;
      }
      
    default:
      break;
    }
  
  // Forward the event to the tool
  tool->on_motion_notify_event(event);
  
  // See if the track that we're hovering over has changed
  shared_ptr<timeline::Track> new_hovering_track(
    timelineWidget.layoutHelper.track_from_y(event->y));
  if(timelineWidget.get_hovering_track() != new_hovering_track)
      timelineWidget.set_hovering_track(new_hovering_track);
  
  // false so that the message is passed up to the owner TimelineWidget
  return false;
}

void
TimelineBody::on_state_changed()
{
  // Connect up some events
  view_window().changed_signal().connect(
    sigc::mem_fun(this, &TimelineBody::on_update_view) );
    
  // Redraw
  queue_draw();
}

void
TimelineBody::draw_tracks(Cairo::RefPtr<Cairo::Context> cr)
{
  REQUIRE(cr);
  
  // Prepare
  TimelineLayoutHelper &layout_helper = timelineWidget.layoutHelper;
  const TimelineLayoutHelper::TrackTree &layout_tree =
    layout_helper.get_layout_tree();
  const Allocation allocation = get_allocation();
  
  // Save the view matrix
  Cairo::Matrix view_matrix;
  cr->get_matrix(view_matrix);
  
  // Iterate drawing each track
  TimelineLayoutHelper::TrackTree::pre_order_iterator iterator;
  for(iterator = ++layout_tree.begin(); // ++ so we skip the sequence root
    iterator != layout_tree.end();
    iterator++)
    {
      const shared_ptr<model::Track> model_track(*iterator);
      const shared_ptr<timeline::Track> timeline_track =
        timelineWidget.lookup_timeline_track(*iterator);
        
      optional<Gdk::Rectangle> rect =
        layout_helper.get_track_header_rect(timeline_track);
      
      // Is this track visible?
      if(rect)
        {
          // Translate to the top of the track
          cr->set_matrix(view_matrix);
          cr->translate(0, rect->get_y());
          
          // Draw the track
          draw_track(cr, timeline_track, allocation.get_width());
        } 
    }
  
  // Restore the view matrix  
  cr->set_matrix(view_matrix);
}

void
TimelineBody::draw_track(Cairo::RefPtr<Cairo::Context> cr,
  shared_ptr<timeline::Track> timeline_track,
  const int view_width) const
{
  REQUIRE(cr);
  REQUIRE(timeline_track != NULL);
    
  const int height = timeline_track->get_height();
  REQUIRE(height >= 0);
  
  // Draw the track background
  cr->rectangle(0, 0, view_width, height);
  GdkColor colour = backgroundColour;   // Needed to preserve const qualifier
  gdk_cairo_set_source_color(cr->cobj(), &colour);
  cr->fill();

  // Render the track
  cr->save();
  TimelineViewWindow &window = view_window();
  timeline_track->draw_track(cr, &window);
  cr->restore();
}

void
TimelineBody::draw_selection(Cairo::RefPtr<Cairo::Context> cr)
{
  REQUIRE(cr);
  
  // Prepare
  const Allocation allocation = get_allocation();
  
  shared_ptr<TimelineState> state = timelineWidget.get_state();
  REQUIRE(state);
  
  const TimelineViewWindow &window = state->get_view_window();
  const int start_x = window.time_to_x(state->get_selection_start());
  const int end_x = window.time_to_x(state->get_selection_end());
  
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
      cr->stroke();
    }
    
  // Draw the end
  if(end_x >= 0 && end_x < allocation.get_width())
    {
      cr->move_to(end_x + 0.5, 0);
      cr->line_to(end_x + 0.5, allocation.get_height());
      cr->stroke();
    }
}

void
TimelineBody::draw_playback_point(Cairo::RefPtr<Cairo::Context> cr)
{   
  REQUIRE(cr);
  
  // Prepare
  
  shared_ptr<TimelineState> state = timelineWidget.get_state();
  REQUIRE(state);
  
  const Allocation allocation = get_allocation();
    
  const gavl_time_t point = state->get_playback_point();
  if(point == (gavl_time_t)GAVL_TIME_UNDEFINED)
    return;
  
  const int x = view_window().time_to_x(point);
    
  // Set source
  gdk_cairo_set_source_color(cr->cobj(), &playbackPointColour);
  cr->set_line_width(1);
    
  // Draw
  if(x >= 0 && x < allocation.get_width())
    {
      cr->move_to(x + 0.5, 0);
      cr->line_to(x + 0.5, allocation.get_height());
      cr->stroke();
    }
}

void
TimelineBody::begin_shift_drag()
{
  dragType = Shift;
  beginShiftTimeOffset = view_window().get_time_offset();
  beginShiftVerticalOffset = get_vertical_offset();
}

int
TimelineBody::get_vertical_offset() const
{
  return (int)timelineWidget.verticalAdjustment.get_value();
}

void
TimelineBody::set_vertical_offset(int offset)
{
  timelineWidget.verticalAdjustment.set_value(offset);
}

TimelineViewWindow&
TimelineBody::view_window() const
{
  shared_ptr<TimelineState> state = timelineWidget.get_state();
  REQUIRE(state);
  return state->get_view_window();
}

void
TimelineBody::register_styles() const
{
  static bool registered = false;
  if(registered) return;
  registered = true;
  
  GtkWidgetClass *klass = GTK_WIDGET_CLASS(G_OBJECT_GET_CLASS(gobj()));

  gtk_widget_class_install_style_property(klass, 
    g_param_spec_boxed("background", "Track Background",
      "The background colour of timeline tracks",
      GDK_TYPE_COLOR, G_PARAM_READABLE));
  
  gtk_widget_class_install_style_property(klass, 
    g_param_spec_boxed("selection", "End lines of a selection",
      "The colour of selection limit lines",
      GDK_TYPE_COLOR, G_PARAM_READABLE));
      
  gtk_widget_class_install_style_property(klass, 
    g_param_spec_float("selection_alpha", "Selection Alpha",
    "The transparency of the selection marque.",
    0, 1.0, 0.5, G_PARAM_READABLE));
    
  gtk_widget_class_install_style_property(klass, 
    g_param_spec_boxed("playback_point", "Playback Point",
      "The colour of the playback marker line",
      GDK_TYPE_COLOR, G_PARAM_READABLE));
}
  
void
TimelineBody::read_styles()
{
  backgroundColour = WindowManager::read_style_colour_property(
    *this, "background", 0, 0, 0);
    
  selectionColour = WindowManager::read_style_colour_property(
    *this, "selection", 0, 0, 0);
  get_style_property("selection_alpha", selectionAlpha);
  
  playbackPointColour = WindowManager::read_style_colour_property(
    *this, "playback_point", 0, 0, 0);
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
