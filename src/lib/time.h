/*
  time.h  -  Time calculations

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

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

#ifndef LUMIERA_TIME_H
#define LUMIERA_TIME_H

#include <sys/time.h>
#include <time.h>
#include <math.h>

#include "lib/error.h"

/*
 * @file Time calculations.
  this time functions are small macro like wrapers, they are all inlined for performance reasons
  time is passed around as pointers, this pointer must never be NULL.

  timehandling is a delicate business, be careful of precision errors accumulating

  lumiera_time is starting from zero, never becomes negative.
  
  TODO explain how to use time

*/

/* over or underflow (tried to make a movie which has negative length? or more than some hundreds days?) */
LUMIERA_ERROR_DECLARE(TIME_OVERFLOW);
LUMIERA_ERROR_DECLARE(TIME_UNDERFLOW);
LUMIERA_ERROR_DECLARE(TIME_NEGATIVE);

/*
  Note: we measure time starting from zero,
  time never becomes negative
  (I didnt checked if the time types are signed)
*/
typedef struct timeval lumiera_time;
typedef lumiera_time* LumieraTime;

/**
 * normalize time after operations.
 * used internally
 */
static inline void
lumiera_time_normalize (LumieraTime time)
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
 * @param time Time to clear
 * @return time as given
 */
static inline LumieraTime
lumiera_time_clear (LumieraTime time)
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
 * @param time Time to put current time into.
 * @return time as given
 */
static inline LumieraTime
lumiera_time_current (LumieraTime time)
{
  if (time)
    {
      /* gettime should never ever fail in a correct program */
      if (gettimeofday (time, NULL))
        LUMIERA_DIE;
    }
  return time;
}

/**
 * init from floating point representation.
 * @param time The time to be set
 * @param fp Time in double
 * @return time as given upon success, NULL if double time given was negative or given time didn't point
 * anywhere
 */
static inline LumieraTime
lumiera_time_set_double (LumieraTime time, double fp)
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
          lumiera_error_set(LUMIERA_ERROR_TIME_NEGATIVE);
        }
    }
  return NULL;
}

/**
 * initialize with seconds and microseconds.
 * @param time Time to set
 * @param sec Seconds to set
 * @param usec Microseconds to set
 * @param Time as given
 */
static inline LumieraTime
lumiera_time_init (LumieraTime time, time_t sec, suseconds_t usec)
{
  if (time)
    {
      time->tv_sec = sec;
      time->tv_usec = usec;
      lumiera_time_normalize (time);
    }
  return time;
}

/**
 * get the seconds part from a time.
 * @param time Time to get seconds from
 * @return Seconds elapsed, -1 on error
 */
static inline time_t
lumiera_time_sec (LumieraTime time)
{
  if (time)
    return time->tv_sec;
  else
    return (time_t)-1;
}

/**
 * get the microseconds part of a time.
 * @param time Time to get microseconds from
 * @return Microseconds elapsed, -1 on error
 */
static inline suseconds_t
lumiera_time_usec (LumieraTime time)
{
  if (time)
    return time->tv_usec;
  else
    return (suseconds_t)-1;
}

/**
 * convert to floating point repesentation.
 * @param time Time to get floating point representation from
 * @return Floating point representation of time. NAN on error.
 */
static inline double
lumiera_time_double_get (LumieraTime time)
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
 * @param dest Time-pointer to copy to
 * @param src Time-source to copy from
 * @return dest as given
 */
static inline LumieraTime
lumiera_time_copy (LumieraTime dest, const LumieraTime src)
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
 * @param dest The result of the add
 * @param src Time to add to dest
 * @return dest as given, or NULL on overflow.
 */
static inline LumieraTime
lumiera_time_add (LumieraTime dest, const LumieraTime src)
{
  if (dest && src)
    {
      time_t t = dest->tv_sec;

      dest->tv_sec += src->tv_sec;
      dest->tv_usec += src->tv_usec;

      lumiera_time_normalize (dest);

      if (dest->tv_sec < t)
        {
          lumiera_error_set (LUMIERA_ERROR_TIME_OVERFLOW);
          return NULL;
        }
    }
  return dest;
}

/**
 * substact time.
 * @param dest The result of subtract
 * @param src Time to subtract from dest
 * @return dest as given, or NULL on underflow.
 */
static inline LumieraTime
lumiera_time_sub (LumieraTime dest, const LumieraTime src)
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

      lumiera_time_normalize (dest);

      if (dest->tv_sec > t)
        {
          lumiera_error_set (LUMIERA_ERROR_TIME_UNDERFLOW);
          return NULL;
        }
    }
  return dest;
}


#endif
