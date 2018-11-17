/*
  TESTSTREAMTYPES.hpp  -  create test (stub) stream type information

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

*/

/** @file teststreamtypes.hpp
 ** Unit test helper to create fake streamtype information
 */


#ifndef LUMIERA_TEST_TESTSTREAMTYPES_H
#define LUMIERA_TEST_TESTSTREAMTYPES_H


//#include "lib/util.hpp"

#include "steam/streamtype.hpp"
#include "steam/control/stypemanager.hpp"
#include "lib/time/timevalue.hpp"

extern "C" {
#include <gavl/gavl.h>
}


namespace steam {
namespace test_format {
  
  using lib::time::TimeValue;
  
  namespace { // constants used to parametrise tests
  
    const int TEST_IMG_WIDTH = 40;
    const int TEST_IMG_HEIGHT = 30;
    
    const int TEST_FRAME_DUR = TimeValue::SCALE / 25;
  }
  
  Symbol GAVL = "GAVL";
  
  
  
  /** Helper: create an raw GAVL type descriptor
   *  usable for generating a Lumiera StreamType
   */
  inline gavl_video_format_t
  test_createRawType ()
  {
    gavl_video_format_t type;
    
    type.pixelformat = GAVL_RGB_24;
    type.interlace_mode = GAVL_INTERLACE_NONE;
    type.framerate_mode = GAVL_FRAMERATE_CONSTANT;
    type.chroma_placement = GAVL_CHROMA_PLACEMENT_DEFAULT;
    
    type.image_width  = TEST_IMG_WIDTH;   // Width of the image in pixels
    type.image_height = TEST_IMG_WIDTH;   // Height of the image in pixels
    type.frame_width  = TEST_IMG_WIDTH;   // Width of the frame buffer in pixels, might be larger than image_width 
    type.frame_height = TEST_IMG_WIDTH;   // Height of the frame buffer in pixels, might be larger than image_height
    
    type.pixel_width  = 1;                // Relative width of a pixel  (pixel aspect ratio is pixel_width/pixel_height)
    type.pixel_height = 1;                // Relative height of a pixel (pixel aspect ratio is pixel_width/pixel_height)
    
    type.frame_duration = TEST_FRAME_DUR; // Duration of a frame in timescale ticks.
    type.timescale = TimeValue::SCALE;    // Timescale in ticks per second  (is defined to be 1000000 as of 9/2008)
    
    return type;
  }
  
  
  /** Helper: create an implementation frame
   *  and build the corresponding streamtype
   */
  inline StreamType::ImplFacade const&
  test_createImplType ()
  {
    gavl_video_format_t rawType = test_createRawType();
    return control::STypeManager::instance().getImpl (GAVL, rawType);
  }
  
  
  
}} // namespace steam::test_format
#endif
