/*
  DummyImageGenerator  -  creating test output frames for simulated playback

  Copyright (C)         Lumiera.org
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>,
                        Hermann Vosseler <Ichthyostega@web.de>

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


/** @file dummy-image-generator.cpp
 ** Implementation of a generator for dummy video frames for a mockup
 ** @todo obsolete since 2010, can be removed once we have a real player in the UI
 */


#include "proc/engine/worker/dummy-image-generator.hpp"


namespace proc {
namespace node {

  
  
  namespace { // implementation details
  
    
    typedef unsigned char byte;
    
    inline int
    clamp (const int &val, const int &maxval, const int &minval)
    {
      if(val > maxval) return maxval;
      if(val < minval) return minval;
      return val;
    }
    
    inline void
    rgb_to_yuv (int r, int g, int b, byte &y, byte &u, byte &v)
    {
      // This code isn't great, but it does the job
      y = (byte)clamp((299 * r + 587 * g + 114 * b) / 1000, 235, 16);
      v = (byte)clamp((500 * r - 419 * g - 81 * b) / 1000 + 127, 255, 0);
      u = (byte)clamp((-169 * r - 331 * g + 500 * b) / 1000 + 127, 255, 0);
    }
    
    
    void
    rgb_buffer_to_yuy2 (unsigned char *in, unsigned char *out)
    {
      for (uint i = 0; i < 320*240*2; i+=4)
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
          out[i + 1] = u0;
          out[i + 2] = y1;
          out[i + 3] = v0;
    }   }
    
    
  } // (End) implementation details
  
  
  
  
  DummyImageGenerator::DummyImageGenerator(uint fps)
    : current_(0)
    , frame_(0)
    , fps_(fps)
  { }
  
  
  LumieraDisplayFrame
  DummyImageGenerator::next()
  {
    
    ++frame_;
    if(frame_ > 2 * fps_)
      frame_ = 0;
    
    if(frame_ < 1 * fps_)
      { 
        // create random snow...
        for(int i = 0; i < 320*240*3; i+=3)
          {
            byte value ( rand() );
            buf_[i]   = value;
            buf_[i+1] = value;
            buf_[i+2] = value;
          }
      }
    else
      { // create a colour strip pattern
        typedef unsigned char Row[320 * 3];
        
        unsigned char * row = buf_;
        
        // create a colour strip pattern in the first row...
        for(int x = 0; x < 320; ++x)
          {
            byte &r = row[x*3];
            byte &g = row[x*3+1];
            byte &b = row[x*3+2];
            
            if     (x < 1*320/7) r = 0xC0, g = 0xC0, b = 0xC0;
            else if(x < 2*320/7) r = 0xC0, g = 0xC0, b = 0x00;
            else if(x < 3*320/7) r = 0x00, g = 0xC0, b = 0xC0;
            else if(x < 4*320/7) r = 0x00, g = 0xC0, b = 0x00;
            else if(x < 5*320/7) r = 0xC0, g = 0x00, b = 0xC0;
            else if(x < 6*320/7) r = 0xC0, g = 0x00, b = 0x00;
            else r = 0x00, g = 0x00, b = 0xC0;
          }
        
        // fill remaining rows of the frame with the same pattern
        for(int y = 1; y < 240; ++y)
          memcpy(buf_ + y*sizeof(Row), row, sizeof(Row));
        
      }
    
    // select output buffer to return
    LumieraDisplayFrame outBuff;
    
    if (!current_)
      {
        outBuff = outFrame_A_;
        current_= 1;
      }
    else
      {
        outBuff = outFrame_B_;
        current_= 0;
      }
    
    rgb_buffer_to_yuy2(buf_, outBuff);
    return outBuff;
    
  }
  
  
  LumieraDisplayFrame
  DummyImageGenerator::current()
  {
    if (!current_) return outFrame_A_;
    else           return outFrame_B_;
  }



}} // namespace proc::node
