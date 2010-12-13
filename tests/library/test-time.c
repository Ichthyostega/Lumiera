/*
 *  test-time.c  -  test the time conversion lib
 *
 *  Copyright (C)         Lumiera.org
 *    2010                Stefan Kangas <skangas@skangas.se>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

typedef unsigned int uint;

#include "lib/time.h"
#include "tests/test.h"

#include <nobug.h>

TESTS_BEGIN

const int FRAMES = 15;
const int MILLIS = 700;
const int SECONDS = 20;
const int MINUTES = 55;
const int HOURS = 3;
const float FPS = 24.0;

/*
 * 1. Basic functionality
 */

TEST (basic)
{
  // Zero
  gavl_time_t t = lumiera_build_time (0,0,0,0);

  CHECK ((gavl_time_t) t                     == 0);
  CHECK (lumiera_time_millis (t)             == 0);
  CHECK (lumiera_time_seconds (t)            == 0);
  CHECK (lumiera_time_minutes (t)            == 0);
  CHECK (lumiera_time_hours (t)              == 0);
  CHECK (lumiera_time_frames (t, FPS)        == 0);
  CHECK (lumiera_time_frames (t, FPS+5)      == 0);
  CHECK (lumiera_time_frame_count (t, FPS)   == 0);
  CHECK (lumiera_time_frame_count (t, FPS+5) == 0);

  ECHO ("%s", lumiera_tmpbuf_print_time (t));

  // Non-zero
  t = lumiera_build_time (MILLIS, SECONDS, MINUTES, HOURS);

  CHECK (lumiera_time_millis (t)             == MILLIS);
  CHECK (lumiera_time_seconds (t)            == SECONDS);
  CHECK (lumiera_time_minutes (t)            == MINUTES);
  CHECK (lumiera_time_hours (t)              == HOURS);
  CHECK (lumiera_time_frames (t, FPS)        == (int)((FPS * MILLIS) / 1000));
  CHECK (lumiera_time_frames (t, FPS+5)      == (int)(((FPS+5) * MILLIS) / 1000));
  CHECK (lumiera_time_frame_count (t, FPS)   == 338897);
  CHECK (lumiera_time_frame_count (t, FPS+5) == 409500);

  ECHO ("%s", lumiera_tmpbuf_print_time (t));
}

/*
 * 2. Frame rate dependent calculations.
 */

TEST (fps)
{
  gavl_time_t t = lumiera_build_time_fps (FPS, FRAMES, SECONDS, MINUTES, HOURS);

  CHECK (lumiera_time_millis (t)             == (int)(FRAMES * (1000.0 / FPS)));
  CHECK (lumiera_time_seconds (t)            == SECONDS);
  CHECK (lumiera_time_minutes (t)            == MINUTES);
  CHECK (lumiera_time_hours (t)              == HOURS);
  CHECK (lumiera_time_frames (t, FPS)        == FRAMES);
  CHECK (lumiera_time_frames (t, FPS+5)      == (int)(((FPS+5) * 625) / 1000));
  CHECK (lumiera_time_frame_count (t, FPS)   == 338895);
  CHECK (lumiera_time_frame_count (t, FPS+5) == 409498);
}

/*
 * 3. NTSC drop-frame calculations.
 */

TEST (ntsc_drop_frame)
{
  // Make sure frame 0 begins at 0
  gavl_time_t t = lumiera_build_time_ntsc_drop (0, 0, 0, 0);

  CHECK ((gavl_time_t) t                     == 0);
  CHECK (lumiera_time_millis (t)             == 0);
  CHECK (lumiera_time_seconds (t)            == 0);
  CHECK (lumiera_time_minutes (t)            == 0);
  CHECK (lumiera_time_hours (t)              == 0);
  CHECK (lumiera_time_frames (t, FPS)        == 0);
  CHECK (lumiera_time_frames (t, FPS+5)      == 0);
  CHECK (lumiera_time_frame_count (t, FPS)   == 0);
  CHECK (lumiera_time_frame_count (t, FPS+5) == 0);

  // Use some arbitrary value to test with
  const int FRAMES = 15;
  const int SECONDS = 20;
  const int MINUTES = 55;
  const int HOURS = 3;
  const float FPS = 24.0;

  t = lumiera_build_time_ntsc_drop (FRAMES, SECONDS, MINUTES, HOURS);

  CHECK (lumiera_time_millis (t)             == 487);
  CHECK (lumiera_time_seconds (t)            == SECONDS);
  CHECK (lumiera_time_minutes (t)            == MINUTES);
  CHECK (lumiera_time_hours (t)              == HOURS);

  // Check standard frame calculations to verify build_time_ntsc_drop
  CHECK (lumiera_time_frames (t, FPS)        == 11);
  CHECK (lumiera_time_frames (t, FPS+5)      == 14);
  CHECK (lumiera_time_frame_count (t, FPS)   == 338892);
  CHECK (lumiera_time_frame_count (t, FPS+5) == 409494);

  // Frames for NTSC drop
  CHECK (lumiera_time_ntsc_drop_frames (t)                 == 15);
  CHECK (lumiera_time_frame_count (t, NTSC_DROP_FRAME_FPS) == 423191);  

  // Make sure we get back the same times.  These tests are perhaps overly exhaustive.
  int min;
  int sec;
  int frame;
  int hrs;
  for (hrs = 0; hrs <= 24; hrs += 6)
    for (min = 0; min <= 59; min += 8)
      for (sec = 0; sec <= 59; sec += 8)
        for (frame = 0; frame <= 29; frame++)
          {
            // Skip dropped frames
            if (min % 10 && sec == 0 && frame < 2)
              continue;

            t = lumiera_build_time_ntsc_drop(frame, sec, min, hrs);

            /* ECHO ("%02d:%02d:%02d.%02d", */
            /*       lumiera_time_ntsc_drop_hours (t), */
            /*       lumiera_time_ntsc_drop_minutes (t), */
            /*       lumiera_time_ntsc_drop_seconds (t), */
            /*       lumiera_time_ntsc_drop_frames (t)); */

            CHECK (lumiera_time_ntsc_drop_frames (t)  == frame);
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
