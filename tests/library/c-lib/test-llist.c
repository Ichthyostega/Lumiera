/*
  TEST-LLIST  -  test the linked list lib

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file test-llist.c
 ** C unit test to cover a linked list library implementation
 ** @see llist.h
 */


#include <nobug.h>

#include "lib/llist.h"
#include "lib/test/test.h"

TESTS_BEGIN

TEST (basic)
{
  LLIST_AUTO (node1);

  llist node2;
  llist_init (&node2);

  printf ("%d\n", llist_is_empty (&node1));
  printf ("%d\n", llist_is_empty (&node2));
}

TEST (nodeinsert)
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
  printf ("%d\n", llist_count (&list));
}

TEST (predicates)
{
  LLIST_AUTO (list);
  LLIST_AUTO (node1);
  LLIST_AUTO (node2);
  LLIST_AUTO (node3);
  LLIST_AUTO (node4);
  LLIST_AUTO (nil);

  llist_insert_tail (&list, &node2);
  llist_insert_tail (&list, &node3);
  llist_insert_tail (&list, &node4);
  llist_insert_head (&list, &node1);

  printf ("%d\n", llist_is_head (&list, &node1));
  printf ("%d\n", llist_is_tail (&list, &node4));
  printf ("%d\n", llist_is_head (&list, &node4));
  printf ("%d\n", llist_is_tail (&list, &node1));
  printf ("%d\n", llist_is_end (&list, &list));
  printf ("%d\n", llist_is_member (&list, &node3));
  printf ("%d\n", llist_is_member (&list, &nil));

  printf ("%d\n", llist_is_before_after (&list, &node1, &node3));
  printf ("%d\n", llist_is_before_after (&list, &node3, &node1));
  printf ("%d\n", llist_is_before_after (&list, &node1, &nil));
}

TEST (unlink)
{
  LLIST_AUTO (list);
  LLIST_AUTO (node1);
  LLIST_AUTO (node2);
  LLIST_AUTO (node3);
  LLIST_AUTO (node4);
  LLIST_AUTO (nil);

  llist_insert_tail (&list, &node2);
  llist_insert_tail (&list, &node3);
  llist_insert_tail (&list, &node4);
  llist_insert_head (&list, &node1);

  LLIST_FOREACH_REV (&list, itr)
    {
      if(itr == &node1) printf ("node1 ");
      else if(itr == &node2) printf ("node2 ");
      else if(itr == &node3) printf ("node3 ");
      else if(itr == &node4) printf ("node4 ");
      else printf ("unknown ");
    }
  printf (".\n");

  llist_unlink (&nil);
  llist_unlink (&node2);
  llist_unlink (&node3);

  LLIST_FOREACH (&list, itr)
    {
      if(itr == &node1) printf ("node1 ");
      else if(itr == &node2) printf ("node2 ");
      else if(itr == &node3) printf ("node3 ");
      else if(itr == &node4) printf ("node4 ");
      else printf ("unknown ");
    }
  printf (".\n");
  printf ("%d\n", llist_is_empty (&node2));
  printf ("%d\n", llist_is_empty (&node3));
  printf ("%d\n", llist_is_empty (&nil));
}

TEST (whiles)
{
  LLIST_AUTO (list);
  LLIST_AUTO (node1);
  LLIST_AUTO (node2);
  LLIST_AUTO (node3);
  LLIST_AUTO (node4);
  LLIST_AUTO (nil);

  llist_insert_tail (&list, &node2);
  llist_insert_tail (&list, &node3);
  llist_insert_tail (&list, &node4);
  llist_insert_head (&list, &node1);

  LLIST_FOREACH_REV (&list, itr)
    {
      if(itr == &node1) printf ("node1 ");
      else if(itr == &node2) printf ("node2 ");
      else if(itr == &node3) printf ("node3 ");
      else if(itr == &node4) printf ("node4 ");
      else printf ("unknown ");
    }
  printf (".\n");

  LLIST_WHILE_HEAD (&list, head)
    llist_unlink (head);

  LLIST_FOREACH (&list, itr)
    {
      if(itr == &node1) printf ("node1 ");
      else if(itr == &node2) printf ("node2 ");
      else if(itr == &node3) printf ("node3 ");
      else if(itr == &node4) printf ("node4 ");
      else printf ("unknown ");
    }
  printf (".\n");

  llist_insert_tail (&list, &node2);
  llist_insert_tail (&list, &node3);
  llist_insert_tail (&list, &node4);
  llist_insert_head (&list, &node1);

  LLIST_WHILE_TAIL (&list, tail)
    llist_unlink (tail);

  LLIST_FOREACH (&list, itr)
    {
      if(itr == &node1) printf ("node1 ");
      else if(itr == &node2) printf ("node2 ");
      else if(itr == &node3) printf ("node3 ");
      else if(itr == &node4) printf ("node4 ");
      else printf ("unknown ");
    }
  printf (".\n");
}


TEST (relocate)
{
  llist source;
  llist_init (&source);

  llist something;
  llist_init (&something);

  llist_insert_head (&source, &something);

  llist target = {NULL,NULL};

  target = source;

  llist_relocate (&target);
  CHECK (llist_is_head (&target, &something));
}


TESTS_END
