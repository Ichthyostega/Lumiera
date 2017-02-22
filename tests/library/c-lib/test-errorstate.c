/*
  TEST-ERRORSTATE  -  test handling of error flags

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

* *****************************************************/

/** @file test-errorstate.c
 ** C unit test to cover basic error handling in C code
 ** We manage an _errorstate variable_ in thread local storage,
 ** which allows to set an global error state per thread.
 */


#include <stdio.h>
#include <string.h>

#include "lib/error.h"


LUMIERA_ERROR_DEFINE(TEST, "test error");

int
main (int argc, char** argv)
{
  if (argc == 1)
    return 0;

  if (!strcmp(argv[1], "set"))
    {
      lumiera_error_set (LUMIERA_ERROR_TEST, NULL);
    }

  if (!strcmp(argv[1], "get_no"))
    {
      const char* err;
      err = lumiera_error ();
      printf ("%p\n", err);
    }

  if (!strcmp(argv[1], "get"))
    {
      lumiera_error_set (LUMIERA_ERROR_TEST, NULL);
      const char* err;
      err = lumiera_error ();
      printf ("%s\n", err);
    }

  if (!strcmp(argv[1], "get2"))
    {
      lumiera_error_set (LUMIERA_ERROR_TEST, NULL);
      const char* err;
      err = lumiera_error ();
      printf ("%s\n", err);
      err = lumiera_error ();
      printf ("%p\n", err);
    }

  return 0;
}
