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
      y = byte(clamp (  0 + (    299 * r +    587 * g +    114 * b) /    1000, 16,235));   // Luma clamped to MPEG scan range
      u = byte(clamp (128 + (-168736 * r - 331264 * g + 500000 * b) / 1000000, 0, 255));   // Chroma components mapped according to Rec.601
      v = byte(clamp (128 + ( 500000 * r - 418688 * g -  81312 * b) / 1000000, 0, 255));   // (but with integer arithmetics and slightly rounded coefficients)
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
    , useRGB_{false}
    , beat_{false}
    , frame_{0}
  { }
  
  void
  DummyImageGenerator::configure (lumiera::DisplayerInput displayFormat)
    {
      INFO (steam, "ImageGen: use format %d", displayFormat);
      REQUIRE (  displayFormat == lumiera::DISPLAY_NONE
              or displayFormat == lumiera::DISPLAY_YUV
              or displayFormat == lumiera::DISPLAY_RGB);
      
      useRGB_ = (displayFormat == lumiera::DISPLAY_RGB);
    }
  
  void
  DummyImageGenerator::generateFrame (DummyFrame buffer)
  {
    ++frame_;
    if(frame_ > 2 * fps_)
      frame_ = 0;
    
    if(frame_ < 1 * fps_)
      generateNoise (buffer);
    else
      generateBars (buffer);
  }
  
  void
  DummyImageGenerator::generateNoise (DummyFrame buffer)
  {  // create random snow...
    for (uint i = 0; i < W*H*3; i+=3)
      {
        auto value = byte(rand());
        buffer[i]   = value;
        buffer[i+1] = value;
        buffer[i+2] = value;
      }
  }
  
  /** fill the RGB-Buffer with the well-known NTSC colour-bar pattern */
  void
  DummyImageGenerator::generateBars (DummyFrame buffer)
  {
    using Row = std::array<Trip, W>;
    using Img = std::array<Row,  H>;
    
    Img& img = reinterpret_cast<Img&> (*buffer);
    Row& row = img[0];
    
    const byte ON = byte(0xC0);
    const byte OFF{0};

    // classic NTSC colour bars  --R---G---B--
    std::array<Trip, 7> bars = {{{ ON, ON, ON}
                                ,{ ON, ON,OFF}
                                ,{OFF, ON, ON}
                                ,{OFF, ON,OFF}
                                ,{ ON,OFF, ON}
                                ,{ ON,OFF,OFF}
                                ,{OFF,OFF, ON}
                               }};
    
    // create a colour strip pattern in the first row...
    for (uint x = 0; x < W; ++x)
      {  //  quantise into 7 columns
        uint col = x  * 7/W;
        row[x] = bars[col];
      }
    
    // fill remaining rows of the frame with the same pattern
    for (uint y = 1; y < H; ++y)
      img[y] = row;
  }
  
  
  DummyFrame
  DummyImageGenerator::next()
  {
    beat_ = not beat_;
    DummyFrame outBuff = current();
    // next output buffer to return
    
    DummyFrame workspace = useRGB_? outBuff : workBuf_.data();
    generateFrame (workspace);
    if (not useRGB_)
      rgb_buffer_to_yuy2(workBuf_.data(), outBuff, W*H);
    
    return outBuff;
    
  }
  
  
  DummyFrame
  DummyImageGenerator::current()
  {
    return beat_? outFrame_A_.data() : outFrame_B_.data();
  }



}} // namespace steam::node
