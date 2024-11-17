/*
  TEST-TIME  -  test the time conversion lib

   Copyright (C)
    2010                Stefan Kangas <skangas@skangas.se>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file test-time.c
 ** C unit test to cover the basic low-level time handling operations
 ** @see time.h
 ** @see timevalue.hpp
 ** @see TimeValue_test high-level time entities
 */


#include "lib/test/test.h"
#include "lib/time.h"

#include <ctype.h>
#include <nobug.h>

static int
calculate_framecount (gavl_time_t t, uint fps)
{
  return lumiera_quantise_frames_fps (t,0,fps);
}


TESTS_BEGIN

const int FRAMES = 15;
const int MILLIS = 700;
const int SECONDS = 20;
const int MINUTES = 55;
const int HOURS = 3;
const int FPS = 24;

/*
 * 1. Basic functionality
 */

TEST (basic)
{
  // Zero
  gavl_time_t t = lumiera_build_time (0,0,0,0);

  CHECK ((gavl_time_t) t                 == 0);
  CHECK (lumiera_time_millis (t)         == 0);
  CHECK (lumiera_time_seconds (t)        == 0);
  CHECK (lumiera_time_minutes (t)        == 0);
  CHECK (lumiera_time_hours (t)          == 0);
  CHECK (lumiera_time_frames (t, FPS)    == 0);
  CHECK (lumiera_time_frames (t, FPS+5)  == 0);
  CHECK (calculate_framecount (t,FPS)    == 0);
  CHECK (calculate_framecount (t, FPS+5) == 0);

  ECHO ("%s", lumiera_tmpbuf_print_time (t));

  // Non-zero
  t = lumiera_build_time (MILLIS, SECONDS, MINUTES, HOURS);

  CHECK (lumiera_time_millis (t)         == MILLIS);
  CHECK (lumiera_time_seconds (t)        == SECONDS);
  CHECK (lumiera_time_minutes (t)        == MINUTES);
  CHECK (lumiera_time_hours (t)          == HOURS);
  CHECK (lumiera_time_frames (t, FPS)    == FPS * MILLIS / 1000);
  CHECK (lumiera_time_frames (t, FPS+5)  == (FPS+5) * MILLIS / 1000);
  CHECK (calculate_framecount (t, FPS)   == 338896);
  CHECK (calculate_framecount (t, FPS+5) == 409500);

  ECHO ("%s", lumiera_tmpbuf_print_time (t));
}

/*
 * 2. Frame rate dependent calculations.
 */

TEST (fps)
{
  gavl_time_t t = lumiera_build_time_fps (FPS, FRAMES, SECONDS, MINUTES, HOURS);

  CHECK (lumiera_time_millis (t)         == FRAMES * 1000 / FPS);
  CHECK (lumiera_time_seconds (t)        == SECONDS);
  CHECK (lumiera_time_minutes (t)        == MINUTES);
  CHECK (lumiera_time_hours (t)          == HOURS);
  CHECK (lumiera_time_frames (t, FPS)    == FRAMES);
  CHECK (lumiera_time_frames (t, FPS+5)  == FRAMES * (FPS+5)/FPS); 
  CHECK (calculate_framecount (t, FPS)   == 338895);
  CHECK (calculate_framecount (t, FPS+5) == 409498);
}

/*
 * 3. NTSC drop-frame calculations.
 */

TEST (ntsc_drop_frame)
{
  // Make sure frame 0 begins at 0
  gavl_time_t t = lumiera_build_time_ntsc_drop (0, 0, 0, 0);

  CHECK ((gavl_time_t) t                 == 0);
  CHECK (lumiera_time_millis (t)         == 0);
  CHECK (lumiera_time_seconds (t)        == 0);
  CHECK (lumiera_time_minutes (t)        == 0);
  CHECK (lumiera_time_hours (t)          == 0);
  CHECK (lumiera_time_frames (t, FPS)    == 0);
  CHECK (lumiera_time_frames (t, FPS+5)  == 0);
  CHECK (calculate_framecount (t, FPS)   == 0);
  CHECK (calculate_framecount (t, FPS+5) == 0);
  
  
  
  t = lumiera_build_time_ntsc_drop (FRAMES, SECONDS, MINUTES, HOURS);
  
  // Calculate manually what result to expect....
  int frames = FRAMES + 30*SECONDS + 30*60*MINUTES + 30*60*60*HOURS;   // sum up using nominal 30fps
  int minutes_to_drop_frames = (MINUTES - MINUTES/10) + (HOURS * 54);  // but every minute, with the exception of every 10 minutes...
  frames -= 2*minutes_to_drop_frames;                                  // ...drop 2 frames
  int64_t expectedMillis = 1000LL * frames  * 1001/30000;              // now convert frames to time, using the real framerate
  
  expectedMillis %= 1000;                                              // look at the remainder..
  CHECK (lumiera_time_millis (t)  == expectedMillis);
  
  CHECK (lumiera_time_seconds (t) == SECONDS);                         // while all other components should come out equal as set
  CHECK (lumiera_time_minutes (t) == MINUTES);
  CHECK (lumiera_time_hours (t)   == HOURS);
  
  // Reverse calculate frames for NTSC drop
//CHECK (lumiera_quantise_frames (t, 0, dropFrameDuration) == frames); // the total nominal frames
  CHECK (lumiera_time_ntsc_drop_frames (t) == FRAMES);        // maximum one frame off due to rounding
  
  // Cover the whole value range;
  // Manually construct a drop-frame timecode
  // Make sure our library function returns the same times.
  int min;
  int sec;
  int frame;
  int hrs;
  for (hrs = 0; hrs <= 24; hrs += 6)
    for (min = 0; min <= 59; min += 1)
      for (sec = 0; sec <= 59; sec += 10)
        for (frame = 0; frame <= 29; frame++)
          {
            // Skip dropped frames
            if (min % 10 && sec == 0 && frame < 2)
              continue;

            t = lumiera_build_time_ntsc_drop(frame, sec, min, hrs);
            /*
            ECHO ("%02d:%02d:%02d;%02d"
                 , lumiera_time_ntsc_drop_hours (t)
                 , lumiera_time_ntsc_drop_minutes (t)
                 , lumiera_time_ntsc_drop_seconds (t)
                 , lumiera_time_ntsc_drop_frames (t)
                 );
            */
            CHECK (lumiera_time_ntsc_drop_frames (t) == frame);
            CHECK (lumiera_time_ntsc_drop_seconds (t) == sec);
            CHECK (lumiera_time_ntsc_drop_minutes (t) == min);
            CHECK (lumiera_time_ntsc_drop_hours (t)   == hrs % 24);
          }

  // Make sure we do not get non-existent frames
  int i;
  for (i = 0; i < 59; i++)
    {
      int frame = (i % 10 == 0) ? 0 : 2;
      t = lumiera_build_time_ntsc_drop (frame, 0, i, 0);
      CHECK (lumiera_time_ntsc_drop_frames (t) == frame);
    }
}

TESTS_END
