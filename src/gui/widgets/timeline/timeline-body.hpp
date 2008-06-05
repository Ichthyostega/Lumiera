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

#include <gtkmm.h>

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
    void on_scroll();

    virtual bool on_expose_event(GdkEventExpose* event);
    
    /* ===== Internals ===== */
  private:
    void read_styles();
    
  private:
    GdkColor track_background;
    
    lumiera::gui::widgets::TimelineWidget *timelineWidget;
  };

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

#endif // TIMELINE_BODY_HPP
