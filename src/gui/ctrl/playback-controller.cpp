/*
  PlaybackController  -  playback controller object

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>

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


/** @file playback-controller.cpp
 ** Implementation parts of PlaybackController.
 ** @warning this is preliminary code and will be reworked, once
 **          we're able to connect to actual playback processes
 **          performed by the render engine.
 */


#include "gui/ctrl/playback-controller.hpp"
#include "gui/display-service.hpp"
#include "lib/error.hpp"
#include "include/logging.h"


namespace gui {
namespace controller { 
  
  
  
  PlaybackController::PlaybackController()
    : playing_(false)
    , viewerHandle_(0)
    { }
  
  
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
          WARN (gui, "failed to start playback: %s" ,err.what());
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
  
  
}} // namespace gui::ctrl

