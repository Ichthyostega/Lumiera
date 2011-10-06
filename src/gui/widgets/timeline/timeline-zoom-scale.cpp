/*
  timeline-zoom-scale.cpp  -  Implementation of the zoom scale widget

  Copyright (C)         Lumiera.org
    2011,               Michael R. Fisher <mfisher31@gmail.com>

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

#include "gui/widgets/timeline/timeline-zoom-scale.hpp"
#include "gui/widgets/timeline-widget.hpp"

using namespace Gtk;

namespace gui {
namespace widgets {

class TimelineWidget;

namespace timeline {

TimelineZoomScale::TimelineZoomScale()
  : HBox()
  , adjustment(0.5, 0.0, 1.0, 0.000001)
  , slider()
  , zoomIn(Stock::ZOOM_IN)
  , zoomOut(Stock::ZOOM_OUT)
  , smoothing_factor(9.0)
  , button_step_size(0.03)
{
  /* Setup the Slider Control */
  slider.set_adjustment(adjustment);
  slider.set_size_request(123,10);
  slider.set_digits(6);
  slider.set_inverted(true);
  slider.set_draw_value(false);

  /* Make our connections */
  zoomIn.signal_clicked().
      connect(sigc::mem_fun(this, &TimelineZoomScale::on_zoom_in_clicked));

  zoomOut.signal_clicked().
      connect(sigc::mem_fun(this, &TimelineZoomScale::on_zoom_out_clicked));

  adjustment.signal_value_changed().
      connect(sigc::mem_fun(this, &TimelineZoomScale::on_zoom));

  /* Add Our Widgets and show them */
  pack_start(zoomOut,PACK_SHRINK);
  pack_start(slider,PACK_SHRINK);
  pack_start(zoomIn,PACK_SHRINK);

  show_all();
}

void
TimelineZoomScale::on_zoom_in_clicked()
{
  double newValue = adjustment.get_value() - button_step_size;
  adjustment.set_value(newValue);
}

void
TimelineZoomScale::on_zoom_out_clicked()
{
  double newValue = adjustment.get_value() + button_step_size;
  adjustment.set_value(newValue);
}

void
TimelineZoomScale::on_zoom()
{
  zoomSignal.emit(calculate_zoom_scale()) ;
}

sigc::signal<void, int64_t>
TimelineZoomScale::signal_zoom()
{
  return zoomSignal;
}

int64_t
TimelineZoomScale::calculate_zoom_scale()
{
  int64_t zoom_scale = 0;

  double smoothed = pow(adjustment.get_value(), smoothing_factor);
  zoom_scale = (int64_t)( smoothed * (double)TimelineWidget::MaxScale);

  /* Prevent Zooming in To Close and Far */
  if(zoom_scale < 1)
    zoom_scale = 1;

  if(zoom_scale > TimelineWidget::MaxScale)
    zoom_scale = TimelineWidget::MaxScale;

  return zoom_scale;
}

} // namespace gui
} // namespace widgets
} // namespace timeline
