/*
    test-mpool.c - memory pool for constant sized objects

  Copyright (C)         Lumiera.org
    2009,               Christian Thaeter <ct@pipapo.org>

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

#include "tests/test.h"
#include "lib/mpool.h"


static void
dtor (void* o)
{
  ECHO("%x @%p", *(int*)o, o);
}

static void
move (void* a, void* b)
{
  ECHO ("%p (%x) to %p ", b, *(int*)b, a);
  *(int*)a = *(int*)b;
  *(void**)b = NULL;
}

TESTS_BEGIN

TEST ("basic")
{
  mpool mypool;
  mpool_init (&mypool, sizeof(void*), 10, move, dtor);
  ECHO ("initialized");

  void* element;
  element = mpool_alloc (&mypool);
  ECHO ("allocated %p", element);
  *(int*)element = 0xdeadbabe;

  DUMP(NOBUG_ON, mpool, &mypool, 4);

  mpool_free (&mypool, element);
  ECHO ("freed");

  DUMP(NOBUG_ON, mpool, &mypool, 4);

  mpool_destroy (&mypool);
  ECHO ("destroyed");
}

TEST ("destroy")
{
  mpool mypool;
  mpool_init (&mypool, sizeof(void*), 10, move, dtor);
  ECHO ("initialized");

  void* element;
  element = mpool_alloc (&mypool);
  ECHO ("allocated %p", element);
  *(int*)element = 0xbabeface;

  DUMP(NOBUG_ON, mpool, &mypool, 4);

  mpool_destroy (&mypool);
  ECHO ("destroyed");
}

TEST ("clusters")
{
  mpool mypool;
  mpool_init (&mypool, sizeof(void*), 2, move, dtor);
  ECHO ("initialized");

  for (int i = 1; i <= 5; ++i)
    {
      void* element;
      element = mpool_alloc (&mypool);
      ECHO ("allocated %p", element);
      *(int*)element = i;
    }

  DUMP(NOBUG_ON, mpool, &mypool, 4);

  mpool_destroy (&mypool);
  ECHO ("destroyed");
}


TEST ("clusters_big")
{
  mpool mypool;
  mpool_init (&mypool, sizeof(void*), 200, move, dtor);
  ECHO ("initialized");

  for (int i = 1; i <= 700; ++i)
    {
      void* element;
      element = mpool_alloc (&mypool);
      ECHO ("allocated %p", element);
      *(int*)element = i;
    }

  DUMP(NOBUG_ON, mpool, &mypool, 4);

  mpool_destroy (&mypool);
  ECHO ("destroyed");
}


TEST ("collect")
{
  mpool mypool;
  mpool_init (&mypool, 24, 4, move, dtor);
  ECHO ("initialized");

  void* elem[32];

  for (int i = 1; i <= 15; ++i)
    {
      elem[i] = mpool_alloc (&mypool);
      *(int*)(elem[i]) = i;
    }
  ECHO ("allocated");

  for (int i = 1; i <= 15; i+=3)
    {
      mpool_free (&mypool, elem[i]);
    }
  ECHO ("freed some");
  DUMP(NOBUG_ON, mpool, &mypool, 4);

  mpool_collect (&mypool, 0);
  ECHO ("collected");

  DUMP(NOBUG_ON, mpool, &mypool, 4);

  mpool_destroy (&mypool);
  ECHO ("destroyed");
}

TEST ("collect_no_move")
{
  mpool mypool;
  mpool_init (&mypool, 24, 4, NULL, dtor);
  ECHO ("initialized");

  void* elem[32];

  for (int i = 1; i <= 15; ++i)
    {
      elem[i] = mpool_alloc (&mypool);
      *(int*)(elem[i]) = i;
    }
  ECHO ("allocated");

  for (int i = 1; i <= 15; i+=3)
    {
      mpool_free (&mypool, elem[i]);
    }
  ECHO ("freed some");
  DUMP(NOBUG_ON, mpool, &mypool, 4);

  mpool_collect (&mypool, 0);
  ECHO ("collected");

  DUMP(NOBUG_ON, mpool, &mypool, 4);

  mpool_destroy (&mypool);
  ECHO ("destroyed");
}


TEST ("reserve")
{
}


TESTS_END
