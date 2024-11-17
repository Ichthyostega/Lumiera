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
 ** @see stage::controller::PlaybackController
 ** @see steam::play::DummyPlayerService
 **  
 */


#ifndef STEAM_NODE_DUMMY_IMAGE_GENERATOR_H
#define STEAM_NODE_DUMMY_IMAGE_GENERATOR_H


#include "lib/error.hpp"
#include "include/display-facade.h"


namespace steam {
namespace node {

      
  class DummyImageGenerator
    {
      
      unsigned char buf_[320 * 240 * 3];         ///< working buffer for next frame
      
      unsigned char outFrame_A_[320 * 240 * 4];  ///< output frame 1
      unsigned char outFrame_B_[320 * 240 * 4];  ///< output frame 2

      uint current_;
      uint frame_;
      uint fps_;
      
      
    public:
      DummyImageGenerator(uint fps);
      
     ~DummyImageGenerator() { }
    
      /** generate the next frame and 
       *  occupy the alternate buffer.
       *  @return the buffer containing the new frame
       */
      LumieraDisplayFrame next();
      
      /** just re-return a pointer to the current frame
       *  without generating any new image data */
      LumieraDisplayFrame current();
    
      
    private:
      
    };
  
  
  
  
}} // namespace steam::node
#endif

