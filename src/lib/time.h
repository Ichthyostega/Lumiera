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

#define NTSC_DROP_FRAME_FPS 29.97

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
 * @param millis number of milliseconds
 * @param secs number of seconds
 * @param mins number of minutes
 * @param hours number of hours
 */
gavl_time_t
lumiera_build_time (long millis, uint secs, uint mins, uint hours);

/**
 * Builds a time value by summing up the given components.
 * @param fps the frame rate to be used
 * @param frames number of frames
 * @param secs number of seconds
 * @param mins number of minutes
 * @param hours number of hours
 */
gavl_time_t
lumiera_build_time_fps (float fps, uint frames, uint secs, uint mins, uint hours);

/**
 * Builds a time value by summing up the given components.
 * The components are interpreted as a NTSC drop-frame timecode.
 * @param frames number of frames
 * @param secs number of seconds
 * @param mins number of minutes
 * @param hours number of hours
 * You must take care not to specify time codes that are illegal NTSC drop-frame times.
 */
gavl_time_t
lumiera_build_time_ntsc_drop (uint frames, uint secs, uint mins, uint hours);

/**
 * Get the hour part of given time.
 * @param time the time value to use
 */
int
lumiera_time_hours (gavl_time_t time);

/**
 * Get the minute part of given time.
 * @param time the time value to use
 */
int
lumiera_time_minutes (gavl_time_t time);

/**
 * Get the seconds part of given time.
 * @param time the time value to use
 */
int
lumiera_time_seconds (gavl_time_t time);

/**
 * Get the milliseconds part of given time.
 * @param time the time value to use
 */
int
lumiera_time_millis (gavl_time_t time);

/**
 * Get the frame part of given time, using the given fps.
 * @param time the time value to use
 * @param fps frame rate (float for now, but should be a rational)
 */
int
lumiera_time_frames (gavl_time_t time, float fps);

/**
 * Get the frame count for the given time, using the given fps.
 * @param time the time value to use
 * @param fps frame rate (float for now, but should be a rational)
 */
int
lumiera_time_frame_count (gavl_time_t time, float fps);

/**
 * Get the frame part of given time, using NTSC drop-frame timecode.
 * @param time the time value to use
 */
int
lumiera_time_ntsc_drop_frames (gavl_time_t time);

/**
 * Get the second part of given time, using NTSC drop-frame timecode.
 * @param time the time value to use
 */
int
lumiera_time_ntsc_drop_seconds (gavl_time_t time);

/**
 * Get the minute part of given time, using NTSC drop-frame timecode.
 * @param time the time value to use
 */
int
lumiera_time_ntsc_drop_minutes (gavl_time_t time);

/**
 * Get the hour part of given time, using NTSC drop-frame timecode.
 * @param time the time value to use
 */
int
lumiera_time_ntsc_drop_hours (gavl_time_t time);

#endif
