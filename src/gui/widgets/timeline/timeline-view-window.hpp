/*
  timeline-view-window.hpp  -  Declaration of the timeline window object

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

*/
/** @file widgets/timeline/timeline-view-window.hpp
 ** This file contains the definition of timeline view window helper
 * object
 */

#ifndef TIMELINE_VIEW_WINDOW_HPP
#define TIMELINE_VIEW_WINDOW_HPP

#include "../../gtk-lumiera.hpp"

namespace gui {
namespace widgets {

class TimelineWidget;
  
namespace timeline {

/**
 * TimelineViewWindow is a helper class for TimelineWidget which manages
 * the view window of the timeline: the zoom and shift. The class also
 * provides utility functions for handling time in the timeline.
 **/ 
class TimelineViewWindow
{
public:
 
  /**
   * Constructor
   * @param offset The initial view offset.
   * @param scale The initial scale.
   **/
  TimelineViewWindow(lumiera::Time offset, int64_t scale);
    
  /**
   * Gets the time offset. This is the time value displaid at the
   * left-hand edge of the timeline body area.
   */
  lumiera::Time get_time_offset() const;

  /**
   * Sets the time offset. This is the time value displaid at the
   * left-hand edge of the timeline body area.
   */
  void set_time_offset(lumiera::Time time_offset);
  
  /**
   * Gets the time scale value.
   * @return The scale factor, which is the number of microseconds per
   * screen pixel.
   */
  int64_t get_time_scale() const;
  
  /**
   * Sets the time scale value.
   * @param time_scale The scale factor, which is the number of
   * microseconds per screen pixel. This value must be greater than
   * zero
   */
  void set_time_scale(int64_t time_scale);
    
  /**
   * Zooms the view in or out as by a number of steps while keeping a 
   * given point on the timeline still.
   * @param zoom_size The number of steps to zoom by. The scale factor
   * is 1.25^(-zoom_size).
   **/
  void zoom_view(int point, int zoom_size);
  
  /**
   * Scrolls the view horizontally as a proportion of the view area.
   * @param shift_size The size of the shift in 1/256ths of the view
   * width.
   **/
  void shift_view(int view_width, int shift_size);
  
  /**
   * Converts time values to x coordinates in pixels.
   * @param time The time value to convert.
   * @return Returns the x-value as pixels from the left hand edge of
   * the timeline body.
   **/
  int time_to_x(int64_t time) const;
  
  /**
   * Converts x coordinates in pixels to time values.
   * @param x The x coordinte (as pixels from the left hand edge of
   * the timeline body) to convert.
   * @return Returns the time at the coordinate.
   **/
  lumiera::Time x_to_time(int x) const;
  
  /**
   * A signal to indicate that the scale or offset have been changed.
   **/
  sigc::signal<void> changed_signal() const; 

private:
  lumiera::Time timeOffset;
  int64_t timeScale;
  
  sigc::signal<void> changedSignal;
};

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

#endif // TIMELINE_VIEW_WINDOW_HPP
