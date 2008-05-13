/*
  test-uuid.c  -  test the uuid lib

  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>

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
//#include "cinelerra-config.h"

//#include <stdio.h>


#include "lib/uuid.h"

#include <nobug.h>

//CINELERRA_ERROR_DEFINE(TEST, "test error");

int
main (int argc, char** argv)
{
  NOBUG_INIT;

  if (argc == 1)
    return 0;

  if (!strcmp(argv[1], "uuidgen_2"))
    {
      lumiera_uuid uuid1;
      lumiera_uuid uuid2;

      lumiera_uuid_gen (&uuid1);
      lumiera_uuid_gen (&uuid2);

      printf ("%d\n", lumiera_uuid_eq (&uuid2, &uuid1));
    }
  else if (!strcmp(argv[1], "uuidgen_copy"))
    {
      lumiera_uuid uuid1;
      lumiera_uuid uuid2;

      lumiera_uuid_gen (&uuid1);

      lumiera_uuid_copy (&uuid2, &uuid1);

      printf ("%d\n", lumiera_uuid_eq (&uuid2, &uuid1));
    }
  else if (!strcmp(argv[1], "ptrs"))
    {
      lumiera_uuid uuid;

      lumiera_uuid_set_ptr (&uuid, &uuid);

      printf ("%d\n", lumiera_uuid_ptr_get (&uuid) == &uuid);
    }
  else
    return 1;

  return 0;
}
