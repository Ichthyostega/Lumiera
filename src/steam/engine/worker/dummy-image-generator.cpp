/*
  DummyImageGenerator  -  creating test output frames for simulated playback

   Copyright (C)
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>,
                        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file dummy-image-generator.cpp
 ** Implementation of a generator for dummy video frames for a mockup
 ** @todo obsolete since 2010, can be removed once we have a real player in the UI
 */


#include "steam/engine/worker/dummy-image-generator.hpp"

#include <algorithm>


namespace steam {
namespace node {

  using std::byte;
  using std::clamp;
  
  namespace { // implementation details
    
    using Trip = std::array<byte,3>;
    
    /** slightly simplified conversion from RGB components to Y'CbCr with Rec.601 (MPEG style) */
    inline Trip
    rgb_to_yuv (Trip const& rgb)
    {
      auto r = int(rgb[0]);
      auto g = int(rgb[1]);
      auto b = int(rgb[2]);
      Trip yuv;
      auto& [y,u,v] = yuv;
      y = byte(clamp (  0 + ( 299 * r + 587 * g + 114 * b) / 1000, 16,235));   // Luma clamped to MPEG scan range
      u = byte(clamp (128 + (-169 * r - 331 * g + 500 * b) / 1000, 0, 255));   // Chroma components mapped according to Rec.601
      v = byte(clamp (128 + ( 500 * r - 419 * g -  81 * b) / 1000, 0, 255));   // (but with integer arithmetics and slightly rounded coefficients)
      return yuv;
    }
    
    
    void
    rgb_buffer_to_yuy2 (byte* in, byte* out, uint cntPix)
    {
      REQUIRE (cntPix %2 == 0);
      for (uint i = 0; i < cntPix; i += 2)
        {// convert and interleave 2 pixels in one step
          uint ip = i * 3;                           // Input uses (RGB)-triples
          uint op = i * 2;                           // Output packed in groups with 2 bytes
          Trip& rgb0 = reinterpret_cast<Trip&> (in[ip]);
          Trip& rgb1 = reinterpret_cast<Trip&> (in[ip+3]);
          Trip yuv0 = rgb_to_yuv (rgb0);
          Trip yuv1 = rgb_to_yuv (rgb1);
          
          auto& [y0,u0,v0] = yuv0;
          auto& [y1,_u,_v] = yuv1;                   // note: this format discards half of the chroma information
          
          out[op    ] = y0;
          out[op + 1] = u0;
          out[op + 2] = y1;
          out[op + 3] = v0;
    }   }
    
    
  } // (End) implementation details
  
  
  
  
  DummyImageGenerator::DummyImageGenerator(uint fps)
    : fps_{fps}
    , beat_{false}
    , frame_{0}
  { }
  
  
  void
  DummyImageGenerator::generateFrame (DummyFrame buffer)
  {
    ++frame_;
    if(frame_ > 2 * fps_)
      frame_ = 0;
    
    if(frame_ < 1 * fps_)
      {
        // create random snow...
        for (uint i = 0; i < W*H*3; i+=3)
          {
            auto value = byte(rand());
            buffer[i]   = value;
            buffer[i+1] = value;
            buffer[i+2] = value;
          }
      }
    else
      { // create a colour strip pattern
        typedef byte Row[W * 3];
        
        byte* row = buffer;
        
        // create a colour strip pattern in the first row...
        for (uint x = 0; x < W; ++x)
          {
            byte& r = row[x*3];
            byte& g = row[x*3+1];
            byte& b = row[x*3+2];
            
            const byte ON = byte(0xC0);
            const byte OFF{0};
            
            if     (x < 1*W/7) r = ON,  g = ON,  b = ON;
            else if(x < 2*W/7) r = ON,  g = ON,  b = OFF;
            else if(x < 3*W/7) r = OFF, g = ON,  b = ON;
            else if(x < 4*W/7) r = OFF, g = ON,  b = OFF;
            else if(x < 5*W/7) r = ON,  g = OFF, b = ON;
            else if(x < 6*W/7) r = ON,  g = OFF, b = OFF;
            else               r = OFF, g = OFF, b = ON;
          }
        
        // fill remaining rows of the frame with the same pattern
        for (uint y = 1; y < H; ++y)
          memcpy(buffer + y*sizeof(Row), row, sizeof(Row));
        
      }
  }
  
  DummyFrame
  DummyImageGenerator::next()
  {
    beat_ = not beat_;
    DummyFrame outBuff = current();
    // next output buffer to return
    
    generateFrame (workBuf_.data());
    
    rgb_buffer_to_yuy2(workBuf_.data(), outBuff, W*H);
    return outBuff;
    
  }
  
  
  DummyFrame
  DummyImageGenerator::current()
  {
    return beat_? outFrame_A_.data() : outFrame_B_.data();
  }



}} // namespace steam::node
