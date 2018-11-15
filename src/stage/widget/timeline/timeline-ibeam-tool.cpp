/*
  TimelineIBeamTool  -  Selection marking tool

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


/** @file timeline-ibeam-tool.cpp
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */


#include "gui/widget/timeline/timeline-ibeam-tool.hpp"
#include "gui/widget/timeline-widget.hpp"
#include "lib/time/mutation.hpp"

using namespace gui::widget;     ///////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

using lib::time::Mutation;
using std::shared_ptr;


namespace gui {
namespace widget {
namespace timeline {
  
  /* ===== Constants ===== */
  
  const int IBeamTool::DragZoneWidth = 5;
  const int IBeamTool::ScrollSlideRateDivisor = 16;
  const int IBeamTool::ScrollSlideEventInterval = 40;
  
  
  
  /* ===== Implementation ===== */
  
  IBeamTool::IBeamTool (TimelineBody& timeline_body)
    : Tool(timeline_body)
    , selectionControl()
    , dragType(None)
    , pinnedDragTime()
    , scrollSlideRate(0)
    {
      // Connect the timlinebody selection to the selectionControl
      get_state()->setSelectionControl (selectionControl);
    }
  
  
  IBeamTool::~IBeamTool()
  {
    selectionControl.disconnect();
    end_scroll_slide();
  }
  
  
  ToolType
  IBeamTool::get_type()  const
  {
    return IBeam;
  }
  
  
  Glib::RefPtr<Gdk::Cursor>
  IBeamTool::get_cursor()  const
  {
    // Are we dragging?
    // Make the cursor indicate that type of drag
    switch(dragType)
      {
      case Selection:
        return Gdk::Cursor::create(Gdk::XTERM);
      case GrabStart:
        return Gdk::Cursor::create(Gdk::LEFT_SIDE);
      case GrabEnd:
        return Gdk::Cursor::create(Gdk::RIGHT_SIDE);
      default:
        break;
      }
    
    // Are we hovering over the ends of the selection?
    // Make the cursor indicate that the user can resize the selection.
    if(is_mouse_in_start_drag_zone())
      return Gdk::Cursor::create(Gdk::LEFT_SIDE);
    if(is_mouse_in_end_drag_zone())
      return Gdk::Cursor::create(Gdk::RIGHT_SIDE);
    
    // By default return an I-beam cursor
    return Gdk::Cursor::create(Gdk::XTERM);
  }
  
  
  void
  IBeamTool::on_button_press_event (GdkEventButton* event)
  {
    Tool::on_button_press_event(event);
    shared_ptr<TimelineState> state = get_state();
    
    if (event->button == 1)
      {
        const Time time = state->getViewWindow().x_to_time(event->x);
        
        if (is_mouse_in_start_drag_zone())
          {
            // User began to drag the start of the selection
            dragType = GrabStart;
            pinnedDragTime = state->getSelectionEnd();
          }
        else
        if (is_mouse_in_end_drag_zone())
          {
            // User began to drag the end of the selection
            dragType = GrabEnd;
            pinnedDragTime = state->getSelectionStart();
          }
        else
          {
            // User began the drag in clear space, begin a Select drag
            dragType = Selection;
            pinnedDragTime = time;
            selectionControl (TimeSpan(time, Duration::NIL));
          }
      }
  }
  
  
  void
  IBeamTool::on_button_release_event (GdkEventButton* event)
  { 
    // Ensure that we can't get a mixed up state
    ENSURE(isDragging == (dragType != None));
    ENSURE(isDragging == (event->button == 1));
    
    if (event->button == 1)
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
  IBeamTool::on_motion_notify_event (GdkEventMotion *event)
  {
    Tool::on_motion_notify_event(event);
    
    // Ensure that we can't get a mixed up state
    ENSURE(isDragging == (dragType != None));
      
    if (isDragging)
      {
        set_leading_x(event->x);
        
        // Is the mouse out of bounds? if so we must begin scrolling
        const Gdk::Rectangle body_rect(get_body_rectangle());
        if (event->x < 0)
          begin_scroll_slide(
            event->x / ScrollSlideRateDivisor);
        else
        if (event->x > body_rect.get_width())
          begin_scroll_slide(
            (event->x - body_rect.get_width()) / ScrollSlideRateDivisor);
        else end_scroll_slide();
      }
      
    apply_cursor();
  }
  
  
  bool
  IBeamTool::on_scroll_slide_timer()
  {
    const Gdk::Rectangle body_rect (get_body_rectangle());
    view_window().shift_view (body_rect.get_width(), scrollSlideRate);
    
    // Return true to keep the timer going
    return true;
  }
  
  
  void
  IBeamTool::set_leading_x (const int x)
  {
    shared_ptr<TimelineState> state = get_state();
    
    // The line below needs handled differently now;  ////////////TODO GTK-3
    //
    //const bool set_playback_period = dragType == Selection;
    
    TimeVar newStartPoint (state->getViewWindow().x_to_time(x));
    Offset selectionLength (pinnedDragTime, newStartPoint);
    
    if (newStartPoint > pinnedDragTime)
      newStartPoint=pinnedDragTime; // use the smaller one as selection start
    
    selectionControl (TimeSpan (newStartPoint, Duration(selectionLength)));
  }
  
  
  void
  IBeamTool::begin_scroll_slide (int scroll_slide_rate)
  {
    scrollSlideRate = scroll_slide_rate;
    if (!scrollSlideEvent.connected())
      scrollSlideEvent = Glib::signal_timeout().connect(
        sigc::mem_fun(this, &IBeamTool::on_scroll_slide_timer),
        ScrollSlideEventInterval);
  }
  
  
  void
  IBeamTool::end_scroll_slide()
  {
    scrollSlideRate = 0;
    if (scrollSlideEvent.connected())
      scrollSlideEvent.disconnect();
  }
  
  
  bool
  IBeamTool::is_mouse_in_start_drag_zone()  const
  {
    const int start_x = view_window().time_to_x(
      get_state()->getSelectionStart());
    
    return (mousePoint.get_x() <= start_x &&
      mousePoint.get_x() > start_x - DragZoneWidth);
  }
  
  
  bool
  IBeamTool::is_mouse_in_end_drag_zone()  const
  {
    const int end_x = view_window().time_to_x(
      get_state()->getSelectionEnd());
    
    return (mousePoint.get_x() >= end_x &&
      mousePoint.get_x() < end_x + DragZoneWidth);
  }
  
  
}}}// namespace gui::widget::timeline
