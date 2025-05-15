/*
  TimelineZoomScale  -  widget to control timeline zoom scale

   Copyright (C)
     2011,            Michael R. Fisher <mfisher31@gmail.com>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file timeline-zoom-scale.cpp
 ** Widget to control timeline zoom scale
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 ** @todo 2025 this was the »zoom slider« implementation from the old GTK-2 GUI of Lumiera.
 **       Since 2016, this was deactivated and since 3/23 it is no longer included anywhere,
 **       just left in tree to be re-integrated into the reworked GKT-3 Timeline UI
 */


#include "stage/widget/timeline-zoom-scale.hpp"


namespace stage {
namespace widget {
  
  /**
   * A widget to control the timeline zoom state.
   * @todo 2023 / 2025 the remainder of the old GTK-2 Timeline UI was removed.
   * @todo 2025 much is broken and unclear on a design level here...
   * The TimelineWidget maintains a TimelineState, which in turn
   * owns the TimelineViewWindow. Now, the problem is: when we
   * switch to another Sequence (View), then this TimelineState
   * gets switched too, causing also a entirely different TimelineViewWindow
   * to become effective. Thus
   * - how can we manage to be notified from that switch?
   * - problem is: TimelineZoomScale widget is owned by the TimelinePannel.
   *   Likewise, TimelineWidget is owned by the TimelinePannel. But the
   *   state handling/switching logic is embedded within TimelineWidget
   * - and finally: how can we translate the actual scale (in time units),
   *   as maintained within TimelineViewWindow, back into the adjustment
   *   used here (which uses a relative scale 0...1.0 )
   */
  
  TimelineZoomScale::TimelineZoomScale()
    : HBox()
    , adjustment(Gtk::Adjustment::create(0.5, 0.0, 1.0, 0.000001))
    , slider()
    , zoomIn(Gtk::Stock::ZOOM_IN)
    , zoomOut(Gtk::Stock::ZOOM_OUT)             /////////////////////////////////////////////////////////////TICKET #1030 StockItems are deprecated and will be removed with GTK-4
    , button_step_size(0.03)
    {
      /* Setup the Slider Control */
      slider.set_adjustment (adjustment);
      slider.set_size_request (123,10);
      slider.set_digits (6);
      
      /* Inverted because smaller values "zoom in" */
      slider.set_inverted (true);
      
      slider.set_draw_value (false);
      
      /* Make our connections */
      zoomIn.signal_clicked().
          connect (sigc::mem_fun(this, &TimelineZoomScale::on_zoom_in_clicked));
      zoomOut.signal_clicked().
          connect (sigc::mem_fun(this, &TimelineZoomScale::on_zoom_out_clicked));
      adjustment->signal_value_changed().
          connect (sigc::mem_fun(this, &TimelineZoomScale::on_zoom));
      
      /* Add Our Widgets and show them */
      pack_start (zoomOut,Gtk::PACK_SHRINK);
      pack_start (slider, Gtk::PACK_SHRINK);
      pack_start (zoomIn, Gtk::PACK_SHRINK);
      
      show_all();
    }
  
  
  void
  TimelineZoomScale::wireTimelineState (/* TODO what to pass here? */)   ////////////////////////////////////TICKET #1264 : how to integrate with the new GTK-3 timeline structure?
  {
//  stateChangeSignal.connect (
//      sigc::mem_fun(this, &TimelineZoomScale::on_timeline_state_changed));
  }
  
  
  void
  TimelineZoomScale::on_timeline_state_changed (/* TODO what to pass here? */)
  {
//  adjustment->set_value (/* TODO how to represent the statte? */);
  }
  
  
  void
  TimelineZoomScale::on_zoom_in_clicked()
  {
    double newValue = adjustment->get_value() - button_step_size;
    adjustment->set_value(newValue);
  }
  
  
  void
  TimelineZoomScale::on_zoom_out_clicked()
  {
    double newValue = adjustment->get_value() + button_step_size;
    adjustment->set_value(newValue);
  }
  
  
  void
  TimelineZoomScale::on_zoom()
  {
    zoomSignal.emit (adjustment->get_value());
  }
  
  
  sigc::signal<void, double>
  TimelineZoomScale::signal_zoom()
  {
    return zoomSignal;
  }
  
  
}}// namespace stage::widget
