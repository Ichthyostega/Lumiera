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
#include "stage/output/xvdisplayer.hpp"
#include "stage/output/gdkdisplayer.hpp"

#include "video-display-widget.hpp"

namespace stage {
namespace widget {
  
  VideoDisplayWidget::VideoDisplayWidget()
    : displayer_(NULL)
    { }
  
  
  VideoDisplayWidget::~VideoDisplayWidget()
  {
    if (displayer_) delete displayer_;
  }
  
  
  Displayer*
  VideoDisplayWidget::getDisplayer()  const
  {
    return displayer_;
  }
  
  
  void
  VideoDisplayWidget::on_realize()
  {
    // invoke base implementation
    Gtk::Widget::on_realize ();
    
    // Set colours
    //modify_bg (Gtk::STATE_NORMAL, Gdk::Color ("black"));
    
    if (displayer_) delete displayer_;
    displayer_ = createDisplayer (this, 320, 240);
    
    add_events (Gdk::ALL_EVENTS_MASK);
  }
  
  
  Displayer*
  VideoDisplayWidget::createDisplayer (Gtk::Widget *drawingArea, int width, int height)
  {
    REQUIRE (drawingArea != NULL);
    REQUIRE (width > 0 && height > 0);
    
    Displayer *displayer = NULL;
    
    displayer = new XvDisplayer (drawingArea, width, height);
    if (!displayer->usable())
      {
        delete displayer;
        displayer = NULL;
      }
    
    if (!displayer)
      {
        displayer = new GdkDisplayer (drawingArea, width, height);
        ///////////////////////////////////////////////////////////////////////////////////////TICKET #950 : new solution for video display
      }
    
    return displayer;
  }
  
  
}}// stage::widget
