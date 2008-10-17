/*
  timeline-arrow-tool.cpp  -  Implementation of the ArrowTool class
 
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

#include "timeline-arrow-tool.hpp"

namespace gui {
namespace widgets {
namespace timeline {

ArrowTool::ArrowTool(TimelineBody *timeline_body) :
  Tool(timeline_body)
{

}

ToolType
ArrowTool::get_type() const
{
  return Arrow;
}

Gdk::Cursor
ArrowTool::get_cursor() const
{
  return Gdk::Cursor(Gdk::ARROW);
}

void
ArrowTool::on_button_press_event(GdkEventButton* event)
{
  Tool::on_button_press_event(event);
}

void
ArrowTool::on_button_release_event(GdkEventButton* event)
{
  Tool::on_button_release_event(event);
}

void
ArrowTool::on_motion_notify_event(GdkEventMotion *event)
{
  Tool::on_motion_notify_event(event);
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
