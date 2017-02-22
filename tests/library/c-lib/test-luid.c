/*
  TEST-LUID  -  test the Lumiera Uniform Identifier

  Copyright (C)         Lumiera.org
    2007, 2008          Christian Thaeter <ct@pipapo.org>

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

/** @file test-luid.c
 ** C unit test to cover generation of Lumiera Uniform Identifier hashes
 ** @see luid.h
 ** @see hash-indexed.hpp
 */


#include "lib/luid.h"

#include <nobug.h>


int
main (int argc, char** argv)
{
  NOBUG_INIT;

  if (argc == 1)
    return 0;

  if (!strcmp(argv[1], "luidgen_2"))
    {
      lumiera_uid luid1;
      lumiera_uid luid2;

      lumiera_uid_gen (&luid1);
      lumiera_uid_gen (&luid2);

      printf ("%d\n", lumiera_uid_eq (&luid2, &luid1));
    }
  else if (!strcmp(argv[1], "luidgen_copy"))
    {
      lumiera_uid luid1;
      lumiera_uid luid2;

      lumiera_uid_gen (&luid1);

      lumiera_uid_copy (&luid2, &luid1);

      printf ("%d\n", lumiera_uid_eq (&luid2, &luid1));
    }
  else if (!strcmp(argv[1], "ptrs"))
    {
      lumiera_uid luid;

      lumiera_uid_set_ptr (&luid, &luid);

      printf ("%d\n", lumiera_uid_ptr_get (&luid) == &luid);
    }
  else
    return 1;

  return 0;
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
