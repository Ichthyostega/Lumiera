/*
  timeline-view-window.hpp  -  Implementation of the timeline window object
 
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

#include "timeline-view-window.hpp"
#include "gui/widgets/timeline-widget.hpp"

using namespace Gtk;
using namespace gui::widgets;
using namespace lumiera;

namespace gui {
namespace widgets {
namespace timeline {

TimelineViewWindow::TimelineViewWindow (Offset offset, int64_t scale)
  : timeOffset(offset)
  , timeScale(scale)
{
  TODO("Create a function to limit timescale between 1 and MaxScale");
  TODO("TICKET #795 Some functions need to be private");
}

Offset
TimelineViewWindow::get_time_offset() const      /////////////////////TICKET #795: this function shouldn't be accessible from outside
{
  return Offset (timeOffset);
}

void
TimelineViewWindow::set_time_offset(TimeValue const& offset) /////////TICKET #795: this function shouldn't be accessible from outside
{
  timeOffset = offset;
  changedSignal.emit();
}

int64_t
TimelineViewWindow::get_time_scale() const       /////////////////////TICKET #795: this function shouldn't be accessible from outside
{
  return timeScale;
}

void
TimelineViewWindow::set_time_scale(int64_t scale)
{
  timeScale = scale;
  changedSignal.emit();
}

void
TimelineViewWindow::set_time_scale(double ratio)
{
  int64_t max = TimelineWidget::MaxScale;
  int64_t min = 1;

  if(ratio <= 0.0)
    {
      set_time_scale((int64_t)min);
      return;
    }

  if(ratio > 1.0)
    {
      ratio = 1.0;
    }

   set_time_scale((int64_t)(ratio * max));
}

void
TimelineViewWindow::zoom_view(int point, double time_scale_ratio)
{
  // Apply the smoothing factor
  int64_t new_time_scale =
      (int64_t)( pow(time_scale_ratio, TimelineWidget::ZoomSmoothing) *
          (double)TimelineWidget::MaxScale);

  /* Prevent Zooming in To Close and Far */
  if(new_time_scale < 1)
    new_time_scale = 1;

  if(new_time_scale > TimelineWidget::MaxScale)
    new_time_scale = TimelineWidget::MaxScale;

  // The view must be shifted so that the zoom is centred on the cursor
  TimeVar newStartPoint = get_time_offset();
  newStartPoint += TimeValue(point * (timeScale - new_time_scale));
  set_time_offset(newStartPoint);
    
  // Apply the new scale
  set_time_scale(new_time_scale);
}

void
TimelineViewWindow::shift_view(int view_width, int shift_size)
{
  set_time_offset(timeOffset + TimeValue(timeScale * shift_size * view_width / 256));
}

int
TimelineViewWindow::time_to_x(TimeValue const& time) const
{
  return int(_raw(time - timeOffset) / timeScale); //////TODO protect against values out-of range
}

Time
TimelineViewWindow::x_to_time(int x) const
{
  TimeValue time_in_view (timeScale * x);
  return timeOffset + time_in_view;
}

sigc::signal<void>
TimelineViewWindow::changed_signal() const
{
  return changedSignal;
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

