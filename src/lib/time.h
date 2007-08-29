/*
  time.h  -  Time calculations

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
#include <math.h>

#include "lib/error.h"

/*
  this time functions are small macro like wrapers, they are all inlined for performance reasons
  time is passed around as pointers, this pointer must never be NULL

  timehandling is a delicate business, be careful of precision errors accumulating, TODO explain how to use time

*/

/* over or underflow (tried to make a movie which has negative length? or more than some hundreds days?) */
CINELERRA_ERROR_DECLARE(TIME_OVERFLOW);
CINELERRA_ERROR_DECLARE(TIME_UNDERFLOW);
CINELERRA_ERROR_DECLARE(TIME_NEGATIVE);

/*
  Note: we measure time starting from zero,
  time never becomes negative
  (I didnt checked if the time types are signed)
*/
typedef struct timeval cinelerra_time;
typedef cinelerra_time* CinelerraTime;

/**
 * normalize time after operations.
 * used internally
 */
static inline void
cinelerra_time_normalize (CinelerraTime time)
{
  REQUIRE (time);
  if (time->tv_usec >= 1000000)
    {
      time->tv_sec += (time->tv_usec / 1000000);
      time->tv_usec = (time->tv_usec % 1000000);
    }
}


/**
 * set a time value to zero.
 */
static inline CinelerraTime
cinelerra_time_clear (CinelerraTime time)
{
  if(time)
    {
      time->tv_sec = 0;
      time->tv_usec = 0;
    }
  return time;
}

/**
 * get current time.
 */
static inline CinelerraTime
cinelerra_time_current (CinelerraTime time)
{
  if (time)
    {
      /* gettime should never ever fail in a correct program */
      if (gettimeofday (time, NULL))
        CINELERRA_DIE;
    }
  return time;
}

/**
 * init from floating point representation.
 */
static inline CinelerraTime
cinelerra_time_set_double (CinelerraTime time, double fp)
{
  if (time)
    {
      if (fp >= 0.0)
        {
          time->tv_sec = fp;
          time->tv_usec = round((fp - time->tv_sec) * 1000000.0);
          return time;
        }
      else
        {
          time->tv_sec = (time_t)-1;
          time->tv_usec = (suseconds_t)-1;
          cinelerra_error_set(CINELERRA_ERROR_TIME_NEGATIVE);
        }
    }
  return NULL;
}

/**
 * initialize with seconds and microseconds.
 */
static inline CinelerraTime
cinelerra_time_init (CinelerraTime time, time_t sec, suseconds_t usec)
{
  if (time)
    {
      time->tv_sec = sec;
      time->tv_usec = usec;
      cinelerra_time_normalize (time);
    }
  return time;
}

/**
 * get the seconds part from a time.
 */
static inline time_t
cinelerra_time_sec (CinelerraTime time)
{
  if (time)
    return time->tv_sec;
  else
    return (time_t)-1;
}

/**
 * get the microseconds part of a time.
 */
static inline suseconds_t
cinelerra_time_usec (CinelerraTime time)
{
  if (time)
    return time->tv_usec;
  else
    return (suseconds_t)-1;
}

/**
 * convert to floating point repesentation.
 */
static inline double
cinelerra_time_double_get (CinelerraTime time)
{
  if (time)
    {
      double fp;

      fp = time->tv_sec;
      fp += time->tv_usec / 1000000.0;
      return fp;
    }
  return NAN;
}


/**
 * copy time
 */
static inline CinelerraTime
cinelerra_time_copy (CinelerraTime dest, const CinelerraTime src)
{
  if (dest && src)
    {
      dest->tv_sec = src->tv_sec;
      dest->tv_usec = src->tv_usec;
    }
  return dest;
}

/**
 * add time.
 */
static inline CinelerraTime
cinelerra_time_add (CinelerraTime dest, const CinelerraTime src)
{
  if (dest && src)
    {
      time_t t = dest->tv_sec;

      dest->tv_sec += src->tv_sec;
      dest->tv_usec += src->tv_usec;

      cinelerra_time_normalize (dest);

      if (dest->tv_sec < t)
        {
          cinelerra_error_set (CINELERRA_ERROR_TIME_OVERFLOW);
          return NULL;
        }
    }
  return dest;
}

/**
 * substact time.
 */
static inline CinelerraTime
cinelerra_time_sub (CinelerraTime dest, const CinelerraTime src)
{
  if (dest && src)
    {
      time_t t = dest->tv_sec;

      dest->tv_sec -= src->tv_sec;
      if (dest->tv_usec >= src->tv_usec)
        dest->tv_usec -= src->tv_usec;
      else
        {
          --dest->tv_sec;
          dest->tv_usec += 1000000 - src->tv_usec;
        }

      cinelerra_time_normalize (dest);

      if (dest->tv_sec > t)
        {
          cinelerra_error_set (CINELERRA_ERROR_TIME_UNDERFLOW);
          return NULL;
        }
    }
  return dest;
}


#endif
