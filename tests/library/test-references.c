/*
  test-references.c  -  test strong and weak references

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
//#include <string.h>

#include "lib/references.h"


LUMIERA_ERROR_DEFINE(TEST, "test error");


struct example
{
  int foo;
};

void
example_dtor(void* o)
{
  printf ("destruct: %d\n", ((struct example*)o)->foo);
  ((struct example*)o)->foo = 0;

  // free(o)
}

int
main (int argc, char** argv)
{
  NOBUG_INIT;

  if (argc == 1)
    return 0;

  if (!strcmp(argv[1], "basic"))
    {
      struct example test;
      test.foo = 123;

      lumiera_reference hold;

      lumiera_reference_strong_init_once (&hold, &test, example_dtor);


      struct example* r = lumiera_reference_get (&hold);

      printf ("got: %d\n", r->foo);

      lumiera_reference_destroy (&hold);
    }
  else if (!strcmp(argv[1], "nodeinsert"))
    {

    }
  else
    return 1;

  return 0;
}
