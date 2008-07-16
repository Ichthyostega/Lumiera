/*
  timeline-body.hpp  -  Declaration of the timeline body widget
 
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
 
*/
/** @file timeline-body.hpp
 ** This file contains the definition of timeline body widget
 */

#ifndef TIMELINE_BODY_HPP
#define TIMELINE_BODY_HPP

#include "../../gtk-lumiera.hpp"

namespace lumiera {
namespace gui {
namespace widgets {

class TimelineWidget;

namespace timeline {

class TimelineBody : public Gtk::DrawingArea
{
public:
  TimelineBody(lumiera::gui::widgets::TimelineWidget *timeline_widget);

  /* ===== Events ===== */
protected:
  void on_realize();

  void on_scroll();
  
  bool on_scroll_event(GdkEventScroll* event);
  
  bool on_button_press_event (GdkEventButton* event);
  
  bool on_button_release_event (GdkEventButton* event);
  
  bool on_motion_notify_event(GdkEventMotion *event);
  
  bool on_expose_event(GdkEventExpose* event);
  
  /* ===== Internals ===== */
private:
  void begin_shift_drag();
  
  int get_vertical_offset() const;
  
  void set_vertical_offset(int offset);

  void read_styles();
  
private:

  // Internal structures
  enum DragType
  {
    None,
    Shift
  };
  
  // UI State Data
  DragType dragType;
  double mouseDownX, mouseDownY;
  gavl_time_t beginShiftTimeOffset;
  int beginShiftVerticalOffset;  

  GdkColor background;
  
  lumiera::gui::widgets::TimelineWidget *timelineWidget;
};

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

#endif // TIMELINE_BODY_HPP
