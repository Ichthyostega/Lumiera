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

#include "tests/test.h"
#include "lib/time.h"

#include <nobug.h>

TESTS_BEGIN

const int MILLIS = 700;
const int SECONDS = 20;
const int MINUTES = 55;
const int HOURS = 3;

/*
 * 1. Basic functionality
 */

TEST (basic) {
  // Zero
  gavl_time_t t = lumiera_build_time(0,0,0,0);

  CHECK ((gavl_time_t) t                    == 0);
  CHECK (lumiera_time_millis(t)             == 0);
  CHECK (lumiera_time_seconds(t)            == 0);
  CHECK (lumiera_time_minutes(t)            == 0);
  CHECK (lumiera_time_hours(t)              == 0);

  ECHO ("%s", lumiera_tmpbuf_print_time(t));

  // Non-zero
  t = lumiera_build_time(MILLIS, SECONDS, MINUTES, HOURS);

  CHECK (lumiera_time_millis(t)             == MILLIS);
  CHECK (lumiera_time_seconds(t)            == SECONDS);
  CHECK (lumiera_time_minutes(t)            == MINUTES);
  CHECK (lumiera_time_hours(t)              == HOURS);

  ECHO ("%s", lumiera_tmpbuf_print_time(t));
}

TESTS_END
