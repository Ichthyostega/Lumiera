/*
  test.h  -  macros for running tests

  Copyright (C)         CinelerraCV
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

#ifndef TEST_H
#define TEST_H

#include <nobug.h>

#include "lib/error.h"

LUMIERA_ERROR_DEFINE (TEST, "test error");

#define TESTS_BEGIN                             \
int                                             \
main (int argc, char** argv)                    \
{                                               \
  NOBUG_INIT;                                   \
                                                \
  if (argc == 1)                                \
    return 1;

#define TEST(name) \
 else if (!strcmp(argv[1], name))


#define TESTS_END                               \
  else                                          \
    return 1;                                   \
                                                \
  return 0;                                     \
}


#endif
