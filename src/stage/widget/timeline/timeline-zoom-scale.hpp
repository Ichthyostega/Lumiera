/*
  TIMELINE-ZOOM-SCALE.hpp  -  widget to control timeline zoom scale

   Copyright (C)
     2011,            Michael R. Fisher <mfisher31@gmail.com>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file timeline-zoom-scale.hpp
 ** Widget to control timeline zoom scale
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 ** @todo this was the »zoom slider« implementation from the old GTK-2 GUI of Lumiera.
 **       Since 2016, this was deactivated and since 3/23 it is no longer included anywhere,
 **       just left in tree to be re-integrated into the reworked GKT-3 Timeline UI
 **                               ///////////////////////////////////////////////////////////////////////////TICKET #74   : Zoom Slider
 **                               ///////////////////////////////////////////////////////////////////////////TICKET #1196 : draft zoom handling
 */



#ifndef STAGE_WIDGET_TIMELINE_ZOOM_SCALE_H
#define STAGE_WIDGET_TIMELINE_ZOOM_SCALE_H

#include "stage/gtk-base.hpp"
#include "stage/widget/mini-button.hpp"
//#include "stage/widget/timeline-widget.hpp"  //////////////////////////////////////////////////////////////TODO old GTK-2 UI is defunct (3/23)
#include "stage/widget/timeline/timeline-state.hpp"

using namespace Gtk;         ////////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace stage::widget;    ///////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

namespace stage {
namespace widget {
namespace timeline {
  
  class TimelineZoomScale
    : public Gtk::HBox
    {
    public:
      TimelineZoomScale();
      
      sigc::signal<void, double> signal_zoom();
      
      void set_value(double val) { adjustment->set_value(val); }
      
      void wireTimelineState (shared_ptr<TimelineState> currentState
//                           ,TimelineWidget::TimelineStateChangeSignal /////////////////////////////////////TODO old GTK-2 UI is defunct (3/23)
                             );
      
      
      
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
  
  
}}}// namespace stage::widget::timeline
#endif /*STAGE_WIDGET_TIMELINE_ZOOM_SCALE_H*/
