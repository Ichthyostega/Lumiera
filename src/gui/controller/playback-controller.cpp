/*
  playback-controller.cpp  -  Implementation of the playback controller object
 
  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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
  playing(false)
{ }


void
PlaybackController::play()
{
  if (playHandle)
    {
      playHandle.play(true);
      playing = true;
    }
  else
    try
      {
        playHandle =  proc::play::DummyPlayer::facade().start();
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
PlaybackController::attach_viewer (FrameDestination const& outputDestination)
{
  /////////////////////TODO: unsolved problem: how to access the display-service from /within/ the GUI??
  DisplayService& displayService = do_something_magic(); 
  
  viewerHandle_ = displayService.setUp (outputDestination);
}


}   // namespace controller
}   // namespace gui

