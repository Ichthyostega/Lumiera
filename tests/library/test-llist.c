/*
  test-llist.c  -  test the linked lis lib

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

//#include <stdio.h>
//#include <string.h>

#include "lib/llist.h"
#include "lib/framerate.h"


CINELERRA_ERROR_DEFINE(TEST, "test error");

int
main (int argc, char** argv)
{
  NOBUG_INIT;

  if (argc == 1)
    return 0;

  if (!strcmp(argv[1], "basic"))
    {
      LLIST_AUTO (node1);

      llist node2;
      llist_init (&node2);

      printf ("%d\n", llist_is_empty (&node1));
      printf ("%d\n", llist_is_empty (&node2));
    }
  else if (!strcmp(argv[1], "nodeinsert"))
    {
      LLIST_AUTO (list);
      LLIST_AUTO (node1);
      LLIST_AUTO (node2);
      LLIST_AUTO (node3);

      llist_insert_next (&list, &node1);
      printf ("%d\n", llist_is_empty (&list));
      printf ("%d\n", llist_is_empty (&node1));
      printf ("%d\n", llist_is_single (&node1));
      llist_insert_next (&node1, &node2);
      printf ("%d\n", llist_is_single (&node1));
      llist_insert_prev (&node1, &node3);
      printf ("%d\n", llist_next (&list) == &node3);
      printf ("%d\n", llist_next (&node3) == &node1);
      printf ("%d\n", llist_next (&node1) == &node2);
      printf ("%d\n", llist_prev (&list) == &node2);
      
    }
  else
    return 1;

  return 0;
}
