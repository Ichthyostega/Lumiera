/*
  framerate.h  -  framerate calculations

  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>

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

#ifndef CINELERRA_FRAMERATE_H
#define CINELERRA_FRAMERATE_H

#include <limits.h>

#include "lib/error.h"
#include "lib/time.h"

/**
 * framerates are defined as a rational number
 * for example NTSC with 30000/1001fps
 */
struct cinelerra_framerate_struct
{
  unsigned n; //numerator
  unsigned d; //denominator
};

typedef struct cinelerra_framerate_struct cinelerra_framerate;
typedef cinelerra_framerate* CinelerraFramerate;

typedef signed long cinelerra_framepos;

CINELERRA_ERROR_DECLARE(FRAMERATE_ILLEGAL_TIME);
CINELERRA_ERROR_DECLARE(FRAMERATE_ILLEGAL_FRAME);

#define CINELERRA_FRAMEPOS_ERROR LONG_MIN

/**
 * Get the frame number of a given time at a given frame rate.
 * frame indexing starts with 1
 * @param framerate is a pointer to the framerate used, defined as rational number. Must be given.
 * @param time is a pointer to a cinelerra_time which shall be converted.
 * @return frame at the given time or CINELERRA_FRAMEPOS_ERROR on error.
 */
static inline cinelerra_framepos
cinelerra_framerate_frame_get_time (const CinelerraFramerate framerate, CinelerraTime time)
{
  REQUIRE (framerate);
  if (!time || time->tv_sec == (time_t)-1)
    {
      cinelerra_error_set(CINELERRA_ERROR_FRAMERATE_ILLEGAL_TIME);
      return CINELERRA_FRAMEPOS_ERROR;
    }

  /* we add a magic microsecond for rounding, because of integer truncation frames may be calculated at most 1us earlier,
     the idea is to compensate odd framerates which fall out off microsecond precision.
   */
  return ((time->tv_sec * 1000000ULL + time->tv_usec + /*magic*/1) * framerate->n)/(framerate->d * 1000000ULL) + 1;
}

/**
 * Get the start time for a frame.
 * frame indexing starts with 1
 * @param framerate is a pointer to the framerate used, defined as rational number. Must be given.
 * @param time is a pointer to a cinelerra_time which shall take the result.
 * @param frame frame number to be converted to time. This frame number must be greater than 0.
 * @return the pointer given in time or NULL on error (or when it was given as time).
 */
static inline CinelerraTime
cinelerra_framerate_time_get_time_frame (const CinelerraFramerate framerate,
                                         CinelerraTime time,
                                         cinelerra_framepos frame)
{
  REQUIRE (framerate);
  if (time)
    {
      if (frame < 1)
        {
          cinelerra_error_set(CINELERRA_ERROR_FRAMERATE_ILLEGAL_FRAME);
          return NULL;
        }

      unsigned long long usec = ((frame-1) * framerate->d * 1000000ULL - (/*magic*/frame>1?1:0)) / framerate->n;

      time->tv_sec = usec / 1000000;
      time->tv_usec = usec % 1000000;
    }
  return time;
}

#endif
