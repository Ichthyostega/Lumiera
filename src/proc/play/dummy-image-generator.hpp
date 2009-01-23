/*
  DUMMY-IMAGE-GENERATOR.hpp  -  creating test output frames for simulated playback
 
  Copyright (C)         Lumiera.org
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>,
                        Hermann Vosseler <Ichthyostega@web.de>
 
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
 
*/

/** @file dummy-image-generator.hpp
 ** Generator for test dummy video frames to simulate playback of rendered output.
 ** As of 1/2009 the render engine and source reading functions are not ready yet.
 ** So, in order to learn how to build up the GUI/Playback interfaces, we use
 ** this dummy image generator to create visible output. First implemented
 ** by Joel within PlaybackController, then factored out into a separate
 ** dummy playback service.
 ** 
 ** @see gui::controller::PlaybackController
 ** @see proc::play::DummyPlayerService
 **  
 */


#ifndef PROC_PLAY_DUMMY_IMAGE_GENERATOR_H
#define PROC_PLAY_DUMMY_IMAGE_GENERATOR_H


#include "lib/error.hpp"


namespace proc {
  namespace play {
  
        
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
        void * const next();
      
        
      private:
        
      };
    
    
    
    
  } // namespace play

} // namespace proc
#endif // PROC_PLAY_DUMMY_IMAGE_GENERATOR_H

