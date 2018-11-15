/*
  TimelineTool  -  base of timeline selection tools

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/


/** @file timeline-tool.cpp
 ** Tools and working modes for selections in the timeline. 
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */


#include "stage/widget/timeline/timeline-tool.hpp"
#include "stage/widget/timeline-widget.hpp"

using namespace Gdk;     ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using std::shared_ptr;

namespace gui {
namespace widget {
namespace timeline {
  
  
  
  
  Tool::Tool (TimelineBody &owner)
    : timelineBody(owner)
    , isDragging(false)
    { }
  
  
  
  bool
  Tool::apply_cursor()
  {
    Glib::RefPtr<Gdk::Window> window = timelineBody.get_window();
    if (!window)
      return false;
#if false                           ///////////////////TODO still necessary? 5/2015
    window->set_cursor(get_cursor());
#endif
    return true;
  }
  
  
  void
  Tool::on_button_press_event (GdkEventButton* event)
  {
    REQUIRE(event != NULL);
    
    if(event->button == 1)
      isDragging = true;
  }
  
  
  void
  Tool::on_button_release_event (GdkEventButton* event)
  {
    REQUIRE(event != NULL);
    
    if(event->button == 1)
      isDragging = false;
  }
  
  
  void
  Tool::on_motion_notify_event (GdkEventMotion *event)
  {
    REQUIRE (event != NULL);
  
    mousePoint = Point(event->x, event->y);
  }
  
  
  gui::widget::TimelineWidget&
  Tool::get_timeline_widget()  const
  {
    gui::widget::TimelineWidget &timeline_widget =
      timelineBody.timelineWidget;
    return timeline_widget;
  }
  
  
  Gdk::Rectangle
  Tool::get_body_rectangle()  const
  {
    return timelineBody.get_allocation();
  }
  
  
  shared_ptr<TimelineState>
  Tool::get_state()  const
  {
    TimelineWidget &timeline_widget = get_timeline_widget();
    shared_ptr<TimelineState> state = timeline_widget.get_state();
    REQUIRE(state);
    return state;
  }
  
  
  TimelineViewWindow&
  Tool::view_window()  const
  {
    return get_state()->getViewWindow();
  }
  
  
}}}// namespace gui::widget::timeline
