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
  thread(0),
  finish_playback_thread(false),
  playing(false)
{ }


PlaybackController::~PlaybackController()
{
  end_playback_thread();
}

void
PlaybackController::play()
{
  if (playing && thread && playHandle)
    {
      playHandle.play(true);
      return;
    }
  if (thread)
    end_playback_thread();
  
  {
    Lock sync(this);
    try
      {
        playHandle =  proc::play::DummyPlayer::facade().start();
        start_playback_thread();
        playing = true;
      }
    catch (lumiera::error::State& err)
      {
        WARN (gui, "failed to start playback: %s" ,err.what());
        lumiera_error();
        playing = false;
      }
  }
}

void
PlaybackController::pause()
{
  Lock sync(this);
  playing = false;
  if (playHandle)
    playHandle.play(false);
}

void
PlaybackController::stop()
{
  {
    Lock sync(this);
    playing = false;
    playHandle.close();
    // TODO: stop player somehow?
  }
  end_playback_thread();
}

bool
PlaybackController::is_playing()
{
  Lock sync(this);
  return playing;
}

void
PlaybackController::start_playback_thread()
{
  dispatcher.connect(sigc::mem_fun(this, &PlaybackController::on_frame));
  finish_playback_thread = false;
  thread = Glib::Thread::create (sigc::mem_fun(
    this, &PlaybackController::playback_thread), true);
}

void
PlaybackController::end_playback_thread()
{
  {
    Lock sync(this);
    finish_playback_thread = true;
    playing = false;
  }
  if (thread)
    thread->join();
  thread = 0;
  finish_playback_thread = false;
}

void
PlaybackController::attach_viewer(
  const sigc::slot<void, void*>& on_frame)
{
  frame_signal.connect(on_frame);
}

void
PlaybackController::playback_thread()
{  
  for(;;)
    {
      {
        Lock sync(this);
        if(finish_playback_thread)
          return;
      }
      
      if(is_playing())
        pull_frame();
      
      usleep(40000); // ca 25 frames pre second
    }
}


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


void
PlaybackController::on_frame()
{
  frame_signal.emit(currentBuffer);
}

}   // namespace controller
}   // namespace gui

