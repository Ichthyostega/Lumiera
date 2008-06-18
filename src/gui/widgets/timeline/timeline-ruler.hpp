/*
  timeline-ruler.hpp  -  Declaration of the time ruler widget
 
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
/** @file timeline-ruler.hpp
 ** This file contains the declaration of the time ruler widget
 ** widget
 */
 
#ifndef TIMELINE_RULER_HPP
#define TIMELINE_RULER_HPP

#include "../../gtk-lumiera.hpp"

namespace lumiera {
namespace gui {
namespace widgets {
namespace timeline {

class TimelineRuler : public Gtk::Widget
  {
  public:
    TimelineRuler();
  
    /* ===== Events ===== */
  protected:
    bool on_expose_event(GdkEventExpose* event);
    
    /* ===== Internals ===== */
  private:
    void read_styles();

    int64_t timeOffset;
    int timeScale;
  };

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

#endif // TIMELINE_RULER_HPP

