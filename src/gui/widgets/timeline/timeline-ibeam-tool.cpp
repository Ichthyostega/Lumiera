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

IBeamTool::IBeamTool(TimelineBody *timeline_body) :
  Tool(timeline_body)
{
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
  
  REQUIRE(timelineBody != NULL);
  lumiera::gui::widgets::TimelineWidget *timeline_widget =
    timelineBody->timelineWidget;
  REQUIRE(timeline_widget != NULL); 
  
  if(event->button == 1)
  {
    drag_start_time = timeline_widget->x_to_time(event->x);
    timeline_widget->set_selection(drag_start_time, drag_start_time);
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
    set_leading_x(event->x);
}

void IBeamTool::set_leading_x(const int x)
{
  REQUIRE(timelineBody != NULL);
  lumiera::gui::widgets::TimelineWidget *timeline_widget =
    timelineBody->timelineWidget;
  REQUIRE(timeline_widget != NULL); 
  
  const gavl_time_t time = timeline_widget->x_to_time(x);
  if(time > drag_start_time)
    timeline_widget->set_selection(drag_start_time, time);
  else
    timeline_widget->set_selection(time, drag_start_time);
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

