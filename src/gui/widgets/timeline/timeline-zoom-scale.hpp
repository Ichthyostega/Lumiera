/*
  timeline-zoom-scale.hpp  -  Declaration of the zoom scale widget

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

*/
/** @file timeline-zoom-scale.hpp
 ** This file contains the definition of the zoom scale widget
 */

#ifndef TIMELINE_ZOOM_SCALE_HPP
#define TIMELINE_ZOOM_SCALE_HPP

#include "gui/gtk-lumiera.hpp"
#include "gui/widgets/mini-button.hpp"
#include "gui/widgets/timeline-widget.hpp"
#include "gui/widgets/timeline/timeline-state.hpp"
#include "gui/widgets/timeline/timeline-view-window.hpp"

#include <boost/shared_ptr.hpp>

using namespace Gtk;
using namespace gui::widgets;

namespace gui {
namespace widgets {
namespace timeline {

class TimelineZoomScale : public Gtk::HBox
{
public:
  /**
   * Constructor
   */
  TimelineZoomScale();

  /**
   * Accessor method to the zoomSignal
   * @return the zoomSignal
   */
  sigc::signal<void, double> signal_zoom();

  void wireTimelineState (boost::shared_ptr<TimelineState> currentState,
                          TimelineWidget::TimelineStateChangeSignal);

private:
  /* Event Handlers */
  
  /**
   * Update the slider position when the timeline state
   * is changed.
   */
  void on_timeline_state_changed (boost::shared_ptr<TimelineState> newState);         ////////////////////TICKET #796 : should use std::tr1::shared_ptr

  /**
   * Event handler for when the zoomIn Button
   * is clicked
   */
  void on_zoom_in_clicked();

  /**
   * Event handler for when the zoomIn Button
   * is clicked
   */
  void on_zoom_out_clicked();

  /**
   * Event handler for when the adjustment
   * value is changed
   */
  void on_zoom();

  /** access current timeline state */
  TimelineViewWindow& getViewWindow();

  /* Widgets */
  Gtk::Adjustment adjustment;
  Gtk::HScale slider;
  MiniButton zoomIn;
  MiniButton zoomOut;

private:
  /* Signals */
  sigc::signal<void, double> zoomSignal;

  const double button_step_size;

  boost::shared_ptr<TimelineState> timelineState;
};

} // namespace gui
} // namespace widgets
} // namespace timeline

#endif /* TIMELINE_ZOOM_SCALE_HPP */
