/*
  DUMMY-IMAGE-GENERATOR.hpp  -  creating test output frames for simulated playback

   Copyright (C)
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>,
                        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file dummy-image-generator.hpp
 ** Generator for test dummy video frames to simulate playback of rendered output.
 ** As of 1/2009 the render engine and source reading functions are not ready yet.
 ** So, in order to learn how to build up the GUI/Playback interfaces, we use
 ** this dummy image generator to create visible output. First implemented
 ** by Joel within PlaybackController, then factored out into a separate
 ** dummy playback service.
 ** 
 ** @todo obsolete since 2010, can be removed once we have a real player in the UI
 ** @todo revived in 2025 for research of video output techniques; this is test code.
 ** @see stage::controller::PlaybackController
 ** @see steam::play::DummyPlayerService
 ** 
 */


#ifndef STEAM_NODE_DUMMY_IMAGE_GENERATOR_H
#define STEAM_NODE_DUMMY_IMAGE_GENERATOR_H


#include "lib/error.hpp"
#include "include/display-handles.hpp"


#include <array>


namespace steam {
namespace node {
  
  
  class DummyImageGenerator
    {
      uint fps_;
      bool useRGB_;
      
    public:
      static const uint W = 320;
      static const uint H = 240;
      
      DummyImageGenerator(uint fps);
      void configure (lumiera::DisplayerInput);
      
      
      /** generate the next frame and occupy the alternate buffer.
       *  @return the buffer containing the new frame */
      DummyFrame next();
      
      /** just re-return a pointer to the current frame
       *  without generating any new image data */
      DummyFrame current();
      
      
    private:
      static constexpr uint WORK_SIZ = W * H * 3;
      static constexpr uint BUFF_SIZ = W * H * 4;
      
      bool beat_;
      uint frame_;
      
      std::array<std::byte,WORK_SIZ> workBuf_;     ///< workspace for RGB calculation
      std::array<std::byte,BUFF_SIZ> outFrame_A_;  ///< alternating output buffers
      std::array<std::byte,BUFF_SIZ> outFrame_B_;
      
      void generateFrame (DummyFrame buffer);
      void generateNoise (DummyFrame buffer);
      void generateBars  (DummyFrame buffer);
    };
  
  
  
  
}} // namespace steam::node
#endif

