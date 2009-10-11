/*
  timeline-ibeam-tool.cpp  -  Implementation of the IBeamTool class
 
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

#include "timeline-ibeam-tool.hpp"
#include "../timeline-widget.hpp"

using namespace boost;
using namespace gui::widgets;
using namespace lumiera;

namespace gui {
namespace widgets {
namespace timeline {

// ===== Constants ===== //

const int IBeamTool::DragZoneWidth = 5;
const int IBeamTool::ScrollSlideRateDivisor = 16;
const int IBeamTool::ScrollSlideEventInterval = 40;

// ===== Implementation ===== //

IBeamTool::IBeamTool(TimelineBody &timeline_body) :
  Tool(timeline_body),
  dragType(None),
  pinnedDragTime(0),
  scrollSlideRate(0)
{

}

IBeamTool::~IBeamTool()
{
  end_scroll_slide();
}

ToolType
IBeamTool::get_type() const
{
  return IBeam;
}

Gdk::Cursor
IBeamTool::get_cursor() const
{
  // Are we dragging?
  // Make the cursor indicate that type of drag
  switch(dragType)
    {
    case Selection:
      return Gdk::Cursor(Gdk::XTERM);
    case GrabStart:
      return Gdk::Cursor(Gdk::LEFT_SIDE);
    case GrabEnd:
      return Gdk::Cursor(Gdk::RIGHT_SIDE);
    default:
      break;
    }
  
  // Are we hovering over the ends of the selection?
  // Make the cursor indicate that the user can resize the selection.
  if(is_mouse_in_start_drag_zone())
    return Gdk::Cursor(Gdk::LEFT_SIDE);
  if(is_mouse_in_end_drag_zone())
    return Gdk::Cursor(Gdk::RIGHT_SIDE);
  
  // By default return an I-beam cursor
  return Gdk::Cursor(Gdk::XTERM);
}

void
IBeamTool::on_button_press_event(GdkEventButton* event)
{
  Tool::on_button_press_event(event);
  
  shared_ptr<TimelineState> state = get_state();
  
  if(event->button == 1)
    {
      const Time time =
        state->get_view_window().x_to_time(event->x);
      
      if(is_mouse_in_start_drag_zone())
        {
          // User began to drag the start of the selection
          dragType = GrabStart;
          pinnedDragTime = state->get_selection_end();
        }
      else if(is_mouse_in_end_drag_zone())
        {
          // User began to drag the end of the selection
          dragType = GrabEnd;
          pinnedDragTime = state->get_selection_start();
        }
      else
        {
          // User began the drag in clear space, begin a Select drag
          dragType = Selection;
          pinnedDragTime = time;
          state->set_selection(time, time);
        }
    }
}

void
IBeamTool::on_button_release_event(GdkEventButton* event)
{ 
  // Ensure that we can't get a mixed up state
  ENSURE(isDragging == (dragType != None));
  ENSURE(isDragging == (event->button == 1));
    
  if(event->button == 1)
    {
      set_leading_x(event->x);
      
      // Terminate the drag now the button is released
      dragType = None;
      
      // If there was a scroll slide, terminate it
      end_scroll_slide();
      
      // Apply the cursor - there are some corner cases where it can
      // change by the end of the drag
      apply_cursor();
    }
    
  Tool::on_button_release_event(event);
}

void
IBeamTool::on_motion_notify_event(GdkEventMotion *event)
{
  Tool::on_motion_notify_event(event);
  
  // Ensure that we can't get a mixed up state
  ENSURE(isDragging == (dragType != None));
    
  if(isDragging)
    {
      set_leading_x(event->x);
    
      // Is the mouse out of bounds? if so we must begin scrolling
      const Gdk::Rectangle body_rect(get_body_rectangle());
      if(event->x < 0)
        begin_scroll_slide(
          event->x / ScrollSlideRateDivisor);
      else if(event->x > body_rect.get_width())
        begin_scroll_slide(
          (event->x - body_rect.get_width()) / ScrollSlideRateDivisor);
      else end_scroll_slide();
    }
    
  apply_cursor();
}

bool
IBeamTool::on_scroll_slide_timer()
{
  const Gdk::Rectangle body_rect(get_body_rectangle());     
  view_window().shift_view(body_rect.get_width(), scrollSlideRate);
    
  // Return true to keep the timer going
  return true;
}

void
IBeamTool::set_leading_x(const int x)
{
  shared_ptr<TimelineState> state = get_state();

  const bool set_playback_period = dragType == Selection;
  const Time time = state->get_view_window().x_to_time(x);
  if(time > pinnedDragTime)
    state->set_selection(pinnedDragTime, time, set_playback_period);
  else
    state->set_selection(time, pinnedDragTime, set_playback_period);
}

void
IBeamTool::begin_scroll_slide(int scroll_slide_rate)
{
  scrollSlideRate = scroll_slide_rate;
  if(!scrollSlideEvent.connected())
    scrollSlideEvent = Glib::signal_timeout().connect(
      sigc::mem_fun(this, &IBeamTool::on_scroll_slide_timer),
      ScrollSlideEventInterval);
}

void
IBeamTool::end_scroll_slide()
{
  scrollSlideRate = 0;
  if(scrollSlideEvent.connected())
    scrollSlideEvent.disconnect();
}

bool
IBeamTool::is_mouse_in_start_drag_zone() const
{    
  const int start_x = view_window().time_to_x(
    get_state()->get_selection_start());
    
  return (mousePoint.get_x() <= start_x &&
    mousePoint.get_x() > start_x - DragZoneWidth);
}
  
bool
IBeamTool::is_mouse_in_end_drag_zone() const
{     
  const int end_x = view_window().time_to_x(
    get_state()->get_selection_end());
    
  return (mousePoint.get_x() >= end_x &&
    mousePoint.get_x() < end_x + DragZoneWidth);
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
