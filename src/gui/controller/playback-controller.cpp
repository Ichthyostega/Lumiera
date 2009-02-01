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

//dispatcher.connect(sigc::mem_fun(this, &PlaybackController::on_frame));


void
PlaybackController::attach_viewer(
  const sigc::slot<void, void*>& on_frame)
{
  frame_signal.connect(on_frame);
}


/*
void
PlaybackController::pull_frame()
{
  REQUIRE (is_playing());
  REQUIRE (playHandle);
  
  unsigned char * newBuffer = reinterpret_cast<unsigned char*> (playHandle.getFrame());
  
  if (newBuffer != currentBuffer)
    {
      currentBuffer = newBuffer; 
      dispatcher.emit();
    }
  else
    {
      TRACE (render, "frame dropped?");
    }
}
*/


void
PlaybackController::on_frame()
{
  frame_signal.emit(currentBuffer);
}

}   // namespace controller
}   // namespace gui

