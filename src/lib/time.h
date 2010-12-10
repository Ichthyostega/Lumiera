/*
  time.h  -  Utilities for handling time

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

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

#ifndef LUMIERA_TIME_H
#define LUMIERA_TIME_H

#include <inttypes.h>
#include <gavl/gavl.h>

/**
 * Formats a time in a safeclib tmpbuf in HH:MM:SS:mmm format.
 * @param size maximal length for the string
 * @param the time value to print
 * @return safeclib temporary buffer containing the constructed of the string
 */
char*
lumiera_tmpbuf_print_time (gavl_time_t time);

/**
 * Builds a time value by summing up the given components.
 */
gavl_time_t
lumiera_build_time (long millis, uint secs, uint mins, uint hours);

/**
 * Get the hour part of given time.
 */
int lumiera_time_hours(gavl_time_t time);

/**
 * Get the minute part of given time.
 */
int lumiera_time_minutes(gavl_time_t time);

/**
 * Get the seconds part of given time.
 */
int lumiera_time_seconds(gavl_time_t time);

/**
 * Get the milliseconds part of given time.
 */
int lumiera_time_millis(gavl_time_t time);

/**
 * Get the frame part of given time, using the given number of frames.
 * @param gavl_time_t the time we are interested in converting
 * @param fps Frame rate (float for now, but should be a rational)
 */
int lumiera_time_frames(gavl_time_t time, float fps);

/**
 * Get the frame count for the given time.
 * @param gavl_time_t the time we are interested in converting
 * @param fps Frame rate (float for now, but should be a rational)
 */
int lumiera_time_frame_count(gavl_time_t time, float fps);

#endif

