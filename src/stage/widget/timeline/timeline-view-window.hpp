/*
  TIMELINE-VIEW-WINDOW.hpp  -  helper to manage the timeline viewport

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


/** @file timeline-view-window.hpp
 ** Helper to manage the timeline window, which holds the main timeline display.
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */



#ifndef STAGE_WIDGET_TIMELINE_VIEW_WINDOW_H
#define STAGE_WIDGET_TIMELINE_VIEW_WINDOW_H

#include "stage/gtk-base.hpp"
#include "lib/time/timevalue.hpp"

namespace gui {
namespace widget {
  class TimelineWidget;
  
  namespace timeline {
  
  using lib::time::Time;
  using lib::time::TimeVar;
  using lib::time::TimeValue;
  using lib::time::TimeSpan;
  using lib::time::Duration;
  using lib::time::Offset;
  
  
  
  /**
   * TimelineViewWindow is a helper class for TimelineWidget which manages
   * the view window of the timeline: the zoom and shift. The class also
   * provides utility functions for handling time in the timeline.
   */
  class TimelineViewWindow
    {
    public:
      
      /**
       * The maximum scale for timeline display.
       * @remarks At MaxScale, every pixel on the timeline is
       *          equivalent to 30000000 lumiera::Time increments.
       */ 
      static const int64_t MaxScale;
      static const double ZoomSmoothing;
      static const double ZoomIncrement;
      
      
      /**
       * @param offset initial view offset.
       * @param scale  initial view scale.
       */
      TimelineViewWindow (Offset offset, int64_t scale);
      
      
      /**
       * Gets the time offset. This is the time value displayed at the
       * left-hand edge of the timeline body area.
       * @todo obviously this must be switched to use the relevant time grid
       *       from the session / current timeline to be displayed. It doesn't
       *       make sense to display raw time values here, as each timeline might
       *       turn out to have a different origin; this is the result of resolving
       *       a placement, and only the session has the necessary information...
       */
      Offset get_time_offset()  const;
      
      /**
       * Sets the time offset. This is the time value displayed at the
       * left-hand edge of the timeline body area.
       * @todo this is private detail that shouldn't be exposed to the outside //////////////////TICKET# 795
       */
      void set_time_offset (TimeValue const&);
      
      /**
       * Gets the time scale value.
       * @return The scale factor, which is the number of microseconds per
       * screen pixel.
       * @todo this is private detail that shouldn't be exposed to the outside //////////////////TICKET# 795
       */
      int64_t get_time_scale()  const;
      
      /**
       * Sets the time scale value.
       * @param time_scale The scale factor, which is
       *        the number of microseconds per screen pixel.
       *        This value must be greater than zero
       */
      void set_time_scale (int64_t time_scale);
      void set_time_scale (double ratio);
      
      /** get the current time scale with zoom smoothing applied */
      double get_smoothed_time_scale()  const;
      
      /**
       * Zooms the view in or out as by a number of steps
       * while keeping a given point on the timeline still.
       * @param new_time_scale The number of steps to zoom by.
       *        The scale factor is 1.25^(-new_time_scale).
       */
      void zoom_view (int point, double timescale_ratio);
      
      /**
       * Scrolls the view horizontally as a proportion of the view area.
       * @param shift_size size of the shift in 1/256ths of the view width.
       */
      void shift_view (int view_width, int shift_size);
      
      /**
       * Converts time values to x coordinates in pixels.
       * @param time The time value to convert.
       * @return x-value as pixels from the left hand edge of the timeline body.
       */
      int time_to_x (TimeValue const&)  const;
      
      /**
       * Converts x coordinates in pixels to time values.
       * @param x coordinate to convert (given in pixels from the left hand edge of the timeline body)
       * @return the time at the given coordinate.
       * 
       * @todo 5/11 this is good for now, but on the long run I'm tinking
       *       we rather should treat that like a special frame grid
       *       (display coordinate system) and then use the same framework
       *       we use for timecodes and frame counts.
       */
      Time x_to_time (int x)  const;
      
      /**
       * A signal to indicate that the scale or offset have been changed.
       */
      sigc::signal<void> changed_signal()  const;
      
      
    private:
      TimeVar timeOffset;
      
      /**
       * The scale of the Timeline Body.
       * @remarks This value represents the time span that is visible in
       *    the TimelineBodyWidget. Smaller numbers here will "zoom in"
       *    while larger numbers will "zoom out"
       */
      int64_t timeScale;
      
      sigc::signal<void> changedSignal;
    };
  
  
}}}// namespace gui::widget::timeline
#endif /*STAGE_WIDGET_TIMELINE_VIEW_WINDOW_H*/
