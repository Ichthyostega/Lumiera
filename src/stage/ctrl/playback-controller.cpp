/*
  PlaybackController  -  playback controller object

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file playback-controller.cpp
 ** Implementation parts of PlaybackController.
 ** @warning this is preliminary code and will be reworked, once
 **          we're able to connect to actual playback processes
 **          performed by the render engine.
 */


#include "stage/ctrl/playback-controller.hpp"
#include "stage/display-service.hpp"
#include "lib/error.hpp"
#include "include/logging.h"


namespace stage {
namespace ctrl {
  
  namespace error = lumiera::error;
  
  
  
  PlaybackController::PlaybackController()
    : playing_(false)
    , viewerHandle_(0)
    { 
      instance = this;                               ////////////////////////////////////////////////////////TICKET #1067 shitty workaround to allow disentangling of top-level
    }
  
  PlaybackController::~PlaybackController()
    { 
      instance = nullptr;                            ////////////////////////////////////////////////////////TICKET #1067 shitty workaround to allow disentangling of top-level
    }
  
  
  PlaybackController* PlaybackController::instance;  ////////////////////////////////////////////////////////TICKET #1067 shitty workaround to allow disentangling of top-level
  
  PlaybackController&
  PlaybackController::get()                          ////////////////////////////////////////////////////////TICKET #1067 shitty workaround to allow disentangling of top-level
  {
    if (not instance)
      throw error::Logic ("GTK UI is not in running state"
                         , LERR_(LIFECYCLE));
    
    return *instance;
  }
  
  void
  PlaybackController::play()
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
  PlaybackController::pause()
  {
    if (playHandle_)
      playHandle_.play(false);
    playing_ = false;
  }
  
  void
  PlaybackController::stop()
  {
    playHandle_.close();
    playing_ = false;
  }
  
  bool
  PlaybackController::is_playing()
  {
    return playing_;
  }
  
  
  
  void
  PlaybackController::useDisplay (LumieraDisplaySlot display)
  {
    viewerHandle_ = display;
  }
  
  
}} // namespace stage::ctrl

