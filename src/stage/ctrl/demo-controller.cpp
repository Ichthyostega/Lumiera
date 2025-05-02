/*
  DemoController  -  playback controller object

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file demo-controller.cpp
 ** Implementation parts of PlaybackController.
 ** @todo 5/2025 now used for a research project for XV video display    ////////////////////////////////////TICKET #1403 : attempt to upgrade the XV displayer
 */


#include "stage/ctrl/demo-controller.hpp"
#include "stage/display-service.hpp"
#include "lib/error.hpp"
#include "include/logging.h"


namespace stage {
namespace ctrl {
  
  namespace error = lumiera::error;
  
  
  
  DemoController::DemoController()
    : playing_(false)
    , viewerHandle_(0)
    { 
      instance = this;                               ////////////////////////////////////////////////////////TICKET #1067 shitty workaround to allow disentangling of top-level
    }
  
  DemoController::~DemoController()
    { 
      instance = nullptr;                            ////////////////////////////////////////////////////////TICKET #1067 shitty workaround to allow disentangling of top-level
    }
  
  
  DemoController* DemoController::instance;  ////////////////////////////////////////////////////////TICKET #1067 shitty workaround to allow disentangling of top-level
  
  DemoController&
  DemoController::get()                          ////////////////////////////////////////////////////////TICKET #1067 shitty workaround to allow disentangling of top-level
  {
    if (not instance)
      throw error::Logic ("GTK UI is not in running state"
                         , LERR_(LIFECYCLE));
    
    return *instance;
  }
  
  void
  DemoController::play()
  {
    if (playHandle_)
      {
        playHandle_.play(true);
        playing_ = true;
      }
    else if (viewerHandle_)
      try
        {
          playHandle_ =  lumiera::DummyPlayer::facade().start (viewerHandle_);
          playing_ = true;
        }
      catch (lumiera::error::State& err)
        {
          WARN (stage, "failed to start playback: %s" ,err.what());
          lumiera_error();
          playing_ = false;
        }
  }
  
  void
  DemoController::pause()
  {
    if (playHandle_)
      playHandle_.play(false);
    playing_ = false;
  }
  
  void
  DemoController::stop()
  {
    playHandle_.close();
    playing_ = false;
  }
  
  bool
  DemoController::is_playing()
  {
    return playing_;
  }
  
  
  
  void
  DemoController::useDisplay (LumieraDisplaySlot display)
  {
    viewerHandle_ = display;
  }
  
  
}} // namespace stage::ctrl

