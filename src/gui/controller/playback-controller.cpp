/*
  playback-controller.cpp  -  Implementation of the playback controller object

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


#include "gui/controller/playback-controller.hpp"
#include "gui/display-service.hpp"
#include "lib/error.hpp"

#include <nobug.h>

namespace gui {
namespace controller { 



PlaybackController::PlaybackController() :
  playing(false),
  viewerHandle_(0)
{ }


void
PlaybackController::play()
{
  if (playHandle)
    {
      playHandle.play(true);
      playing = true;
    }
  else if (viewerHandle_)
    try
      {
        playHandle =  lumiera::DummyPlayer::facade().start (viewerHandle_);
        playing = true;
      }
    catch (lumiera::error::State& err)
      {
        WARN (gui, "failed to start playback: %s" ,err.what());
        lumiera_error();
        playing = false;
      }
}

void
PlaybackController::pause()
{
  if (playHandle)
    playHandle.play(false);
  playing = false;
}

void
PlaybackController::stop()
{
  playHandle.close();
  playing = false;
}

bool
PlaybackController::is_playing()
{
  return playing;
}



void
PlaybackController::use_display (LumieraDisplaySlot display)
{
  viewerHandle_ = display;
}


}   // namespace controller
}   // namespace gui

