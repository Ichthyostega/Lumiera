/*
  timeline-tool.hpp  -  Implementation of the Tool class
 
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

#include "timeline-tool.hpp"
#include "../timeline-widget.hpp"

namespace lumiera {
namespace gui {
namespace widgets {
namespace timeline {

Tool::Tool(TimelineWidget *timeline_widget) :
  timelineWidget(timeline_widget)
{
  REQUIRE(timeline_widget != NULL);
  REQUIRE(timeline_widget->body != NULL);
}

bool
Tool::apply_cursor()
{ 
  REQUIRE(timelineWidget != NULL);
  
  if(timelineWidget->body == NULL)
    return false;
  
  Glib::RefPtr<Gdk::Window> window = 
    timelineWidget->body->get_window();
  if(!window)
    return false;
  
  window->set_cursor(get_cursor());

  return true;
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

