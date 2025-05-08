/*
  VideoDisplayWidget  -  Implementation of the video viewer widget

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file video-display-widget.cpp
 ** Implementation of video display, embedded into the UI.
 ** @deprecated defunct since the transition to GTK-3
 */


#include "stage/gtk-base.hpp"
#include "stage/output/xv-displayer.hpp"
#include "stage/output/pixbuf-displayer.hpp"
#include "stage/output/null-displayer.hpp"
#include "stage/widget/video-display-widget.hpp"
#include "stage/style-scheme.hpp"


namespace stage {
namespace widget {
  
  namespace {
    const uint VIDEO_WIDTH = 320;
    const uint VIDEO_HEIGHT = 240;   ////////////////////////////////////////////////////////////////////////TICKET #1289 : these should not be hard coded, but negotiated with the OutputManager
  }
  
  
  using std::make_unique;
  using stage::output::XvDisplayer;
  using stage::output::PixbufDisplayer;
  using stage::output::NullDisplayer;
  
  VideoDisplayWidget::VideoDisplayWidget()
  {
      get_style_context()->add_class (CLASS_background);     // Style to ensure an opaque backdrop
      get_style_context()->add_class (CLASS_videodisplay);
      this->show_all();
  }
  
  
  void
  VideoDisplayWidget::pushFrame (void* const buffer)
  {
    REQUIRE(displayer_);
    displayer_->put(buffer);
  }
  
  
  void
  VideoDisplayWidget::on_realize()
  {
    // invoke base implementation
    Gtk::Widget::on_realize ();
    setupDisplayer (VIDEO_WIDTH, VIDEO_HEIGHT);
  }
  
  
  void
  VideoDisplayWidget::setupDisplayer(uint videoWidth, uint videoHeight)
  {
    REQUIRE (videoWidth > 0);
    REQUIRE (videoHeight > 0);
    
    displayer_ = make_unique<XvDisplayer> (*this, videoWidth, videoHeight);
    if (displayer_->usable())
      return;
    
    displayer_ = make_unique<PixbufDisplayer> (*this, videoWidth, videoHeight);
    if (displayer_->usable())
      return;
    
    displayer_ = make_unique<NullDisplayer> (*this, videoWidth, videoHeight);
    ENSURE (displayer_->usable());
  }
  
  
}}// stage::widget
