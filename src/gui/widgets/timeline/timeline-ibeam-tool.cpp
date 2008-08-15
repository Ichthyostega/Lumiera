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

namespace lumiera {
namespace gui {
namespace widgets {
namespace timeline {
  
const int IBeamTool::ScrollSlideRateDivisor = 16;

IBeamTool::IBeamTool(TimelineBody *timeline_body) :
  dragStartTime(0),
  scrollSlideRate(0),
  Tool(timeline_body)
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
  return Gdk::Cursor(Gdk::XTERM);
}

void
IBeamTool::on_button_press_event(GdkEventButton* event)
{
  Tool::on_button_press_event(event);
  
  lumiera::gui::widgets::TimelineWidget *timeline_widget =
    get_timeline_widget();
  
  if(event->button == 1)
    {
      dragStartTime = timeline_widget->x_to_time(event->x);
      timeline_widget->set_selection(dragStartTime, dragStartTime);
    }
}

void
IBeamTool::on_button_release_event(GdkEventButton* event)
{
  Tool::on_button_release_event(event);
    
  if(event->button == 1)
    set_leading_x(event->x);
}

void
IBeamTool::on_motion_notify_event(GdkEventMotion *event)
{
  Tool::on_motion_notify_event(event);
    
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
}

bool
IBeamTool::on_scroll_slide_timer()
{ 
  lumiera::gui::widgets::TimelineWidget *timeline_widget =
    get_timeline_widget();
    
  timeline_widget->shift_view(scrollSlideRate);
    
  // Return true to keep the timer going
  return true;
}

void
IBeamTool::set_leading_x(const int x)
{
  REQUIRE(timelineBody != NULL);
  lumiera::gui::widgets::TimelineWidget *timeline_widget =
    timelineBody->timelineWidget;
  REQUIRE(timeline_widget != NULL); 
  
  const gavl_time_t time = timeline_widget->x_to_time(x);
  if(time > dragStartTime)
    timeline_widget->set_selection(dragStartTime, time);
  else
    timeline_widget->set_selection(time, dragStartTime);
}

void
IBeamTool::begin_scroll_slide(int scroll_slide_rate)
{
  scrollSlideRate = scroll_slide_rate;
  if(!scrollSlideEvent.connected())
    scrollSlideEvent = Glib::signal_timeout().connect(
      sigc::mem_fun(this, &IBeamTool::on_scroll_slide_timer), 40);
}

void
IBeamTool::end_scroll_slide()
{
  scrollSlideRate = 0;
  if(scrollSlideEvent.connected())
    scrollSlideEvent.disconnect();
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

