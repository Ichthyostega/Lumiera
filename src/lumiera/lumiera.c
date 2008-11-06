/*
  lumiera.c  -  Lumiera main application shell

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

//TODO: Support library includes//


//TODO: Lumiera header includes//
#include "lumiera/lumiera.h"

//TODO: internal/static forward declarations//


//TODO: System includes//
#include <stdio.h>

/**
 * @file
 *
 */

NOBUG_DEFINE_FLAG (all);
NOBUG_DEFINE_FLAG_PARENT (lumiera_all, all);
NOBUG_DEFINE_FLAG_PARENT (lumiera, lumiera_all);


//code goes here//

static void
lumiera_init (void)
{
  NOBUG_INIT;
  NOBUG_INIT_FLAG (all);
  NOBUG_INIT_FLAG (lumiera_all);
  NOBUG_INIT_FLAG (lumiera);

}



int
main (int argc, char** argv)
{
  lumiera_init ();
  (void) argc;
  (void) argv;
  printf ("Lumiera is alive ...\n");
}


/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
