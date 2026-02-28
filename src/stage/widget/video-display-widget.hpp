/*
  VIDEO-DISPLAY-WIDGET.hpp  -  GUI widget for displaying video

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file video-display-widget.hpp
 ** Widget to create a video display embedded into the UI
 ** @warning WIP 2025 — will be reworked as part of the »Playback Vertical Slice«
 **          to establish an actual connection to the Render Engine
 */




#ifndef STAGE_WIDGET_VIDEO_DISPLAY_WIDGET_H
#define STAGE_WIDGET_VIDEO_DISPLAY_WIDGET_H

#include "stage/gtk-base.hpp"
#include "stage/output/displayer.hpp"

#include <memory>

namespace stage {
namespace widget {
  
  using stage::output::Displayer;
  using vessel::spine::DisplayerInput;
  
  
  /**
   * @todo the first UI draft included a video displayer widget library implementation,
   *       Unfortunately, this became defunct with the switch to GTK-3. And a fun fact is,
   *       even while Lumiera is a video editing application, we did not yet reach the state
   *       as to care for video display ourselves. Someone (TM) need to care for this!
   */
  class VideoDisplayWidget
    : public Gtk::Image
    {
      std::unique_ptr<Displayer> displayer_;
      
    public:
      VideoDisplayWidget();
      
      /** signal slot to display the next frame */
      void pushFrame (void* const);
      
      /** signal to configure the image generation format */
      sigc::signal<void(DisplayerInput)> signal_activate;
      
    private:
      virtual void on_realize()  override;
      
      
    private:
      void setupDisplayer(uint videoWidth, uint videoHeight);
    };
  
  
}}// stage::widget
#endif /*STAGE_WIDGET_VIDEO_DISPLAY_WIDGET_H*/
