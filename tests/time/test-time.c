/*
  test-time.c  -  test time and framerate functions

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

#include <stdio.h>
#include <string.h>

#include "lib/time.h"
#include "lib/framerate.h"


LUMIERA_ERROR_DEFINE(TEST, "test error");

int
main (int argc, char** argv)
{
  NOBUG_INIT;

  if (argc == 1)
    return 0;

  if (!strcmp(argv[1], "init"))
    {
      lumiera_time time;

      lumiera_time_init (&time, atol (argv[2]), atol(argv[3]));

      printf ("%lu %lu\n", (long)lumiera_time_sec (&time), (long)lumiera_time_usec (&time));
    }

  if (!strcmp(argv[1], "todouble"))
    {
      lumiera_time time;

      lumiera_time_init (&time, atol (argv[2]), atol(argv[3]));

      printf ("%g\n", lumiera_time_double_get (&time));
    }

  if (!strcmp(argv[1], "todoublenull"))
    {
      printf ("%g\n", lumiera_time_double_get (NULL));
    }

  if (!strcmp(argv[1], "fromdouble"))
    {
      lumiera_time time;

      lumiera_time_set_double (&time, atof (argv[2]));

      printf ("%lu %lu\n", (long)lumiera_time_sec (&time), (long)lumiera_time_usec (&time));
    }

  if (!strcmp(argv[1], "currenttime"))
    {
      lumiera_time time;

      lumiera_time_current (&time);

      printf ("%lu %lu\n", (long)lumiera_time_sec (&time), (long)lumiera_time_usec (&time));
    }

  if (!strcmp(argv[1], "add"))
    {
      lumiera_time time1, time2;

      lumiera_time_init (&time1, 0, atol (argv[2]));
      lumiera_time_init (&time2, 0, atol (argv[3]));
      lumiera_time_add (&time1, &time2);

      printf ("%lu %lu\n", (long)lumiera_time_sec (&time1), (long)lumiera_time_usec (&time1));
    }

  if (!strcmp(argv[1], "sub"))
    {
      lumiera_time time1, time2;

      lumiera_time_init (&time1, 0, atol (argv[2]));
      lumiera_time_init (&time2, 0, atol (argv[3]));
      lumiera_time_sub (&time1, &time2);

      printf ("%lu %lu\n", (long)lumiera_time_sec (&time1), (long)lumiera_time_usec (&time1));
    }

  if (!strcmp(argv[1], "ntscframefromtime"))
    {
      lumiera_framerate ntsc = {30000, 1001};
      lumiera_time time;

      lumiera_time_init (&time, atol (argv[2]), atol (argv[3]));

      printf ("%lu\n", (long)lumiera_framerate_frame_get_time (&ntsc, &time));
    }

  if (!strcmp(argv[1], "ntscframestart"))
    {
      lumiera_framerate ntsc = {30000, 1001};
      lumiera_time time;

      if(lumiera_framerate_time_get_time_frame (&ntsc, &time, atol (argv[2])))
        printf ("%lu %lu\n", (long)lumiera_time_sec(&time), (long)lumiera_time_usec(&time));
    }

  if (!strcmp(argv[1], "ntscframecheck"))
    {
      lumiera_framerate ntsc = {30000, 1001};
      lumiera_time time1;
      lumiera_time time2;
      lumiera_framepos frame;

      lumiera_framepos frame1;
      lumiera_framepos frame2;

      frame = atol (argv[2]);


      if (lumiera_framerate_time_get_time_frame (&ntsc, &time1, frame))
        {
          printf("frame %lu ", frame1 = lumiera_framerate_frame_get_time (&ntsc, &time1));

          lumiera_time_init (&time2, 0, 1);
          lumiera_time_sub (&time1, &time2);
          printf("%lu\n", frame2 = lumiera_framerate_frame_get_time (&ntsc, &time1));
          ENSURE (frame1 == frame2+1);
        }

    }

  return 0;
}
