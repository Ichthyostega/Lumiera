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

typedef unsigned char byte;

inline int
clamp(const int &val, const int &maxval, const int &minval)
{
  if(val > maxval) return maxval;
  if(val < minval) return minval;
  return val;
}

inline void
rgb_to_yuv(int r, int g, int b, byte &y, byte &u, byte &v)
{
  // This code isn't great, but it does the job
  y = (byte)clamp((299 * r + 587 * g + 114 * b) / 1000, 235, 16);
  v = (byte)clamp((500 * r - 419 * g - 81 * b) / 1000 + 127, 255, 0);
  u = (byte)clamp((-169 * r - 331 * g + 500 * b) / 1000 + 127, 255, 0);
}

void rgb_buffer_to_yuy2(unsigned char *in, unsigned char *out)
{
  for(int i = 0; i < 320*240*2; i+=4)
    {
      byte y0, u0, v0;
      const byte r0 = *(in++);
      const byte g0 = *(in++);
      const byte b0 = *(in++);
      rgb_to_yuv(r0, g0, b0, y0, u0, v0);
      
      byte y1, u1, v1;
      const byte r1 = *(in++);
      const byte g1 = *(in++);
      const byte b1 = *(in++);
      rgb_to_yuv(r1, g1, b1, y1, u1, v1);
      
      out[i] = y0;
      out[i + 1] = (byte)(((int)u0 + (int)u1) / 2);
      out[i + 2] = y1;
      out[i + 3] = (byte)(((int)v0 + (int)v1) / 2);
    }
}

void
PlaybackController::pull_frame()
{
  static int frame = 0;
  unsigned char in[320 * 240 * 3];
  
  frame--;
  
  if(frame <= 0)
    frame = 200;
  
  if(frame > 150)
  {
    for(int i = 0; i < 320*240*3; i+=3)
      {
        byte value = (byte)rand();
        in[i] = value;
        in[i+1] = value;
        in[i+2] = value;
      }
  }
  else
  {  
    unsigned char row[320 * 3];

    
    for(int x = 0; x < 320; x++)
      {
        byte &r = row[x*3];
        byte &g = row[x*3+1];
        byte &b = row[x*3+2];
        
        if(x < 1*320/7) r = 0xC0, g = 0xC0, b = 0xC0;
        else if(x < 2*320/7) r = 0xC0, g = 0xC0, b = 0x00;
        else if(x < 3*320/7) r = 0x00, g = 0xC0, b = 0xC0;
        else if(x < 4*320/7) r = 0x00, g = 0xC0, b = 0x00;
        else if(x < 5*320/7) r = 0xC0, g = 0x00, b = 0xC0;
        else if(x < 6*320/7) r = 0xC0, g = 0x00, b = 0x00;
        else r = 0x00, g = 0x00, b = 0xC0;
      }
    
    for(int y = 0; y < 240; y++)
    {
      memcpy(in + y*320*3, row, sizeof(row));
    }
  }
  
  rgb_buffer_to_yuy2(in, buffer);

  dispatcher.emit();
}

void
PlaybackController::on_frame()
{
  frame_signal.emit(buffer);
}

}   // namespace controller
}   // namespace gui

