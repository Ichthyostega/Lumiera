/*
  TIMELINE-ZOOM-SCALE.hpp  -  widget to control timeline zoom scale

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
 ** TODO timeline-zoom-scale.hpp
 */



#ifndef GUI_WIDGET_TIMELINE_ZOOM_SCALE_H
#define GUI_WIDGET_TIMELINE_ZOOM_SCALE_H

#include "gui/gtk-lumiera.hpp"
#include "gui/widget/mini-button.hpp"
#include "gui/widget/timeline-widget.hpp"
#include "gui/widget/timeline/timeline-state.hpp"

using namespace Gtk;
using namespace gui::widget;

namespace gui {
namespace widget {
namespace timeline {
  
  class TimelineZoomScale
    : public Gtk::HBox
    {
    public:
      TimelineZoomScale();
      
      sigc::signal<void, double> signal_zoom();
      
      void set_value(double val) { adjustment->set_value(val); }
      
      void wireTimelineState (shared_ptr<TimelineState> currentState,
                              TimelineWidget::TimelineStateChangeSignal);
      
      
      
    private: /* ===== Event Handlers ===== */
      
      /**
       * Update the slider position when the timeline state is changed.
       */
      void on_timeline_state_changed (shared_ptr<TimelineState> newState);
      
      /**
       * Event handler for when the zoomIn Button is clicked
       */
      void on_zoom_in_clicked();
      
      /**
       * Event handler for when the zoomIn Button is clicked
       */
      void on_zoom_out_clicked();
      
      /**
       * Event handler for when the adjustment value is changed
       */
      void on_zoom();
      
      /** access current timeline state */
      TimelineViewWindow& getViewWindow();
      
      /* Widgets */
      Glib::RefPtr<Gtk::Adjustment> adjustment;
      Gtk::HScale slider;
      MiniButton zoomIn;
      MiniButton zoomOut;
      
      
    private:
      sigc::signal<void, double> zoomSignal;
      const double button_step_size;
      shared_ptr<TimelineState> timelineState;
    };
  
  
}}}// namespace gui::widget::timeline
#endif /*GUI_WIDGET_TIMELINE_ZOOM_SCALE_H*/
