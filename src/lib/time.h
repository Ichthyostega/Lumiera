/*
  time.h  -  Time and frame calculations

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

#ifndef CINELERRA_TIME_H
#define CINELERRA_TIME_H

#include <sys/time.h>
#include <time.h>

#include "error.h"

/*
  this time functions are small macro like wrapers, they are all inlined for performance reasons
  time is passed around as pointers, this pointer must never be NULL

  timehandling is a delicate business, be careful of precision errors accumulating, TODO explain how to use time

*/

/* over or underflow (tried to make a movie which has negative length? or more than some hundreds days?) */
CINELERRA_ERROR_DECLARE(TIME_RANGE);

/*
  Note: we measure time starting from zero,
  time never becomes negative
  (I didnt checked if the time types are signed)
*/
typedef struct timeval cinelerra_time;
typedef cinelerra_time* CinelerraTime;

/**
 * framerates are defined as a rational number and a base time (usually 1sec (do we need this?))
 * for example NTSC with 29.97fps is {2997, 100, {1, 0}}
 */
struct cinelerra_framerate_struct
{
  unsigned numerator;
  unsigned denominator;
  cinelerra_time base;
};

typedef struct cinelerra_framerate_struct cinelerra_framerate;
typedef cinelerra_framerate CinelerraFramerate;

typedef unsigned long cinelerra_frame;




/** set a time value to zero.
 */
inline CinelerraTime
cinelerra_time_clear (CinelerraTime time)
{
  REQUIRE (time);
  time->tv_sec = 0;
  time->tv_usec = 0;
  return time;
}

/** get current time.
 */
inline CinelerraTime
cinelerra_time_current (CinelerraTime time)
{
  REQUIRE (time);
  /* gettime should never ever fail in a correct program */
  if (gettimeofday (time, 0))
    CINELERRA_DIE;

  return time;
}

/** init from floating point representation.
 */
inline CinelerraTime
cinelerra_time_set_double (CinelerraTime time, double fp)
{
  REQUIRE (time);
  REQUIRE (fp >= 0.0);

  time->tv_sec = fp;
  time->tv_usec = (fp - time->tv_sec) * 1000000.0;
  return time;
}

/** convert to floating point repesentation.
 */
inline double
cinelerra_time_double_get (CinelerraTime time)
{
  REQUIRE (time);
  double fp;

  fp = time->tv_sec;
  fp += time->tv_usec / 1000000.0;
  return fp;
}



/** convert from floating point repesentation.
 */
inline CinelerraTime
cinelerra_time_set_double (CinelerraTime time, double fp)
{
  REQUIRE (time);
  REQUIRE (fp >= 0.0);

  time->tv_sec = fp;
  time->tv_usec = (fp - time->tv_sec) * 1000000.0;
  return time;
}

/* internal function */
inline void
cinelerra_time_normalize (CinelerraTime time)
{
  if (time->tv_usec >= 1000000)
    {
      time->tv_sec += (time->tv_usec / 1000000);
      time->tv_usec = (time->tv_usec % 1000000);
    }
}

/** copy time
 */
inline CinelerraTime
cinelerra_time_copy (CinelerraTime dest, const CinelerraTime src)
{
  REQUIRE (dest);
  REQUIRE (src);
  dest->tv_sec = src->tv_sec;
  dest->tv_usec = src->tv_usec;
  return time;
}

/** add time.
 */
inline CinelerraTime
cinelerra_time_add (CinelerraTime dest, const CinelerraTime to_add)
{
  REQUIRE (dest);
  REQUIRE (to_add);
  REQUIRE (dest->tv_sec + to_add->tv_sec > dest->tv_sec, "time overflow");
  TODO ("handling overflow as error?");

  dest->tv_sec += to_add->tv_sec;
  time->tv_usec += to_add->tv_usec;

  cinelerra_time_normalize (dest);
  return dest;
}

/** substact time.
 */
inline CinelerraTime
cinelerra_time_sub (CinelerraTime dest, const CinelerraTime to_sub)
{
  REQUIRE (dest);
  REQUIRE (to_sub);
  REQUIRE (dest->tv_sec - to_sub->tv_sec < dest->tv_sec, "time underflow");
  TODO ("handling underflow as error?");

  dest->tv_sec -= to_sub->tv_sec;
  time->tv_usec -= to_sub->tv_usec;

  cinelerra_time_normalize (dest);
  return dest;
}


#endif
