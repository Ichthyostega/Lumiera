/*
  timeline-widget.hpp  -  Declaration of the timeline widget
 
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
/** @file timeline-widget.hpp
 ** This file contains the definition of timeline widget
 */

#ifndef TIMELINE_WIDGET_HPP
#define TIMELINE_WIDGET_HPP

#include "../gtk-lumiera.hpp"
#include "timeline/header-container.hpp"
#include "timeline/timeline-body.hpp"
#include "timeline/timeline-ruler.hpp"
#include "timeline/track.hpp"
#include "timeline/video-track.hpp"

#include <vector>

namespace lumiera {
namespace gui {
namespace widgets {

class TimelineWidget : public Gtk::Table
  {
  public:
    TimelineWidget();

    ~TimelineWidget();

    /* ===== Events ===== */
  protected:
    void on_scroll();
    
    void on_size_allocate(Gtk::Allocation& allocation);

    /* ===== Internals ===== */
  protected:
  
    void update_tracks();
    
    void update_scroll();
    
    int get_y_scroll_offset() const;

  protected:
    int totalHeight;

    timeline::VideoTrack video1;
    timeline::VideoTrack video2;
    std::vector<timeline::Track*> tracks;

    timeline::HeaderContainer *headerContainer;
    timeline::TimelineBody *body;
    timeline::TimelineRuler ruler;

    Gtk::Adjustment horizontalAdjustment, verticalAdjustment;
    Gtk::HScrollbar horizontalScroll;
    Gtk::VScrollbar verticalScroll;
    
    /* ===== Constants ===== */
  protected:
    static const int TrackPadding;
    static const int HeaderWidth;

    friend class timeline::TimelineBody;
    friend class timeline::HeaderContainer;
  };

}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

#endif // TIMELINE_WIDGET_HPP

