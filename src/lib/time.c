/*
  time.c  -  Utilities for handling time

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

#include <nobug.h>
#include "lib/time.h"
#include "lib/tmpbuf.h"

/* GAVL_TIME_SCALE is the correct factor or dividend when using gavl_time_t for
 * units of whole seconds from gavl_time_t.  Since we want to use milliseconds,
 * we need to multiply or divide by 1000 to get correct results. */
#define GAVL_TIME_SCALE_MS (GAVL_TIME_SCALE / 1000)

char*
lumiera_tmpbuf_print_time (gavl_time_t time)
{
  int milliseconds, seconds, minutes, hours;
  int negative;
  
  if(time < 0)
    {
      negative = 1;
      time = -time;
    }
  else negative = 0;
  
  time /= GAVL_TIME_SCALE_MS;
  milliseconds = time % 1000;
  time /= 1000;
  seconds = time % 60;
  time /= 60;
  minutes = time % 60;
  time /= 60;
  hours = time;
  
  char *buffer = lumiera_tmpbuf_snprintf(64, "%s%02d:%02d:%02d.%03d",
    negative ? "-" : "", hours, minutes, seconds, milliseconds);
    
  ENSURE(buffer != NULL);
  return buffer;
}

gavl_time_t
lumiera_build_time(long millis, uint secs, uint mins, uint hours)
{
  gavl_time_t time = millis
                   + 1000 * secs
                   + 1000 * 60 * mins
                   + 1000 * 60 * 60 * hours;
  time *= GAVL_TIME_SCALE_MS;
  return time;
}

int
lumiera_time_hours(gavl_time_t time)
{
  return time / GAVL_TIME_SCALE_MS / 1000 / 60 / 60;
}

int
lumiera_time_minutes(gavl_time_t time)
{
  return (time / GAVL_TIME_SCALE_MS / 1000 / 60) % 60;
}

int
lumiera_time_seconds(gavl_time_t time)
{
  return (time / GAVL_TIME_SCALE_MS / 1000) % 60;
}

int
lumiera_time_millis(gavl_time_t time)
{
  return (time / GAVL_TIME_SCALE_MS) % 1000;
}

int
lumiera_time_frames(gavl_time_t time, float fps)
{
  return (fps * (lumiera_time_millis(time))) / 1000;
}

int
lumiera_time_frame_count(gavl_time_t time, float fps)
{
  int ms = (time / GAVL_TIME_SCALE_MS);
  return fps * ms / 1000;
}
