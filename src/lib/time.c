/*
  time.c  -  Utilities for handling time

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

#include <nobug.h>
#include "lib/time.h"
#include "lib/safeclib.h"

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
  
  time /= GAVL_TIME_SCALE / 1000;
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

