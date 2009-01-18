/*
  timeline-track.cpp  -  Implementation of the timeline track object
 
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

#include "playback-controller.hpp"
#include "../gtk-lumiera.hpp"

namespace gui {
namespace controller { 

PlaybackController::PlaybackController() :
  finish_playback_thread(false),
  playing(false)
{
  start_playback_thread();
}

PlaybackController::~PlaybackController()
{
  mutex.lock();
  finish_playback_thread = true;
  mutex.unlock();
  thread->join();
}

void
PlaybackController::play()
{
  Glib::Mutex::Lock lock(mutex);
  playing = true;
}

void
PlaybackController::pause()
{
  Glib::Mutex::Lock lock(mutex);
  playing = false;
}

void
PlaybackController::stop()
{
  Glib::Mutex::Lock lock(mutex);
  playing = false;
}

bool
PlaybackController::is_playing()
{
  Glib::Mutex::Lock lock(mutex);
  return playing;
}

void
PlaybackController::start_playback_thread()
{
  dispatcher.connect(sigc::mem_fun(this, &PlaybackController::on_frame));
  thread = Glib::Thread::create (sigc::mem_fun(
    this, &PlaybackController::playback_thread), true);
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
        Glib::Mutex::Lock lock(mutex);
        if(finish_playback_thread)
          return;
      }
      
      if(is_playing())
        pull_frame();
        
      Glib::Thread::yield();
    }
}

void
PlaybackController::pull_frame()
{
  for(int i = 0; i < 320*240*4; i++)
    buffer[i] = rand();

  dispatcher.emit();
}

void
PlaybackController::on_frame()
{
  frame_signal.emit(buffer);
}

}   // namespace controller
}   // namespace gui

