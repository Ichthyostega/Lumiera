/*
    test-mpool.c - memory pool for constant sized objects

  Copyright (C)         Lumiera.org
    2009,               Christian Thaeter <ct@pipapo.org>

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
*/

#include "lib/test/test.h"
#include "lib/mpool.h"

struct teststruct
{
  llist node;
  void* ptr[2];
};


static inline uint32_t mpool_fast_prng ()
{
  static uint32_t rnd=0xbabeface;
  return rnd = rnd<<1 ^ ((rnd >> 30) & 1) ^ ((rnd>>2) & 1);
}

static void
dtor (void* o)
{
  ECHO("%d @%p", *(int*)o, o);
}



TESTS_BEGIN

TEST (basic)
{
  mpool mypool;
  mpool_init (&mypool, sizeof(void*), 10, dtor);
  ECHO ("initialized");

  void* element;
  element = mpool_alloc (&mypool);
  ECHO ("allocated %p", element);
  *(int*)element = 0xdeadbabe;

  DUMP(NOBUG_ON, mpool, &mypool, 4, NULL);

  mpool_free (&mypool, element);
  ECHO ("freed");

  DUMP(NOBUG_ON, mpool, &mypool, 4, NULL);

  mpool_destroy (&mypool);
  ECHO ("destroyed");
}


TEST (destroy)
{
  mpool mypool;
  mpool_init (&mypool, sizeof(void*), 10, dtor);
  ECHO ("initialized");

  void* element;
  element = mpool_alloc (&mypool);
  ECHO ("allocated %p", element);
  *(int*)element = 0xbabeface;

  DUMP(NOBUG_ON, mpool, &mypool, 4, NULL);

  mpool_destroy (&mypool);
  ECHO ("destroyed");
}


TEST (clusters)
{
  mpool mypool;
  mpool_init (&mypool, sizeof(void*), 2, dtor);
  ECHO ("initialized");

  for (int i = 1; i <= 5; ++i)
    {
      void* element;
      element = mpool_alloc (&mypool);
      ECHO ("allocated %p", element);
      *(int*)element = i;
    }

  DUMP(NOBUG_ON, mpool, &mypool, 4, NULL);

  mpool_destroy (&mypool);
  ECHO ("destroyed");
}


TEST (clusters_big)
{
  mpool mypool;
  mpool_init (&mypool, sizeof(void*), 200, dtor);
  ECHO ("initialized");

  for (int i = 1; i <= 700; ++i)
    {
      void* element;
      element = mpool_alloc (&mypool);
      ECHO ("allocated %p", element);
      *(int*)element = i;
    }

  DUMP(NOBUG_ON, mpool, &mypool, 4, NULL);

  mpool_destroy (&mypool);
  ECHO ("destroyed");
}


TEST (alloc_free)
{
  mpool mypool;
  mpool_init (&mypool, 24, 4, dtor);
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
  DUMP(NOBUG_ON, mpool, &mypool, 4, NULL);

  mpool_destroy (&mypool);
  ECHO ("destroyed");
}


TEST (alloc_free_big)
{
  mpool mypool;
  mpool_init (&mypool, 24, 4, dtor);
  ECHO ("initialized");

  void* elem[2000];

  for (int i = 1; i <= 2000; ++i)
    {
      elem[i] = mpool_alloc (&mypool);
      *(int*)(elem[i]) = i;
    }
  ECHO ("allocated");

  for (int i = 1; i <= 2000; i+=3)
    {
      mpool_free (&mypool, elem[i]);
    }
  ECHO ("freed some");
  DUMP(NOBUG_ON, mpool, &mypool, 4, NULL);

  DUMP(NOBUG_ON, mpool, &mypool, 4, NULL);

  mpool_destroy (&mypool);
  ECHO ("destroyed");
}



TEST (reserve)
{
}


/*
  benchmark mpool vs malloc, first only the allocation/free itself with some necessary llist ops
*/
TEST (bench_mpool)
{
  mpool mypool;
  mpool_init (&mypool, sizeof(struct teststruct), 2000, NULL);
  ECHO ("initialized");

  llist list;
  llist_init (&list);

  for (int j = 1; j<=100; ++j)
    {
      for (int i = 1; i <= 50000; ++i)
        {
          struct teststruct* element =  mpool_alloc (&mypool);
          llist_insert_tail (&list, llist_init (&element->node));
        }

      LLIST_WHILE_HEAD (&list, element)
        {
          llist_unlink_fast_ (element);
          mpool_free (&mypool, element);
        }
    }

  mpool_destroy (&mypool);
  ECHO ("destroyed");
}


TEST (bench_malloc)
{
  mpool mypool;
  mpool_init (&mypool, sizeof(llist), 2000, NULL);
  ECHO ("initialized");

  llist list;
  llist_init (&list);

  for (int j = 100; j; --j)
    {
      for (int i = 1; i <= 50000; ++i)
        {
          struct teststruct* element = malloc (sizeof(*element));
          llist_insert_tail (&list, llist_init (&element->node));
        }

      LLIST_WHILE_HEAD (&list, element)
        {
          llist_unlink_fast_ (element);
          free (element);
        }
    }

  mpool_destroy (&mypool);
  ECHO ("destroyed");
}


/*
  benchmark mpool vs malloc, try to simulate some slightly more realistic application usage
  - allocate list nodes which have 2 data members as payload
  - there is a 25% chance at each alloc that the head of the list gets deleted
*/
TEST (bench_mpool_sim)
{
  mpool mypool;
  mpool_init (&mypool, sizeof(struct teststruct), 2000, NULL);
  ECHO ("initialized");

  llist list;
  llist_init (&list);

  for (int j = 1; j<=100; ++j)
    {
      for (int i = 1; i <= 50000; ++i)
        {
          struct teststruct* element =  mpool_alloc (&mypool);
          llist_insert_tail (&list, llist_init (&element->node));
          element->ptr[0] = malloc(100+(mpool_fast_prng()%500));
          element->ptr[1] = malloc(100+(mpool_fast_prng()%500));

          if (!(mpool_fast_prng()%4))
            {
              struct teststruct* element = (struct teststruct*)llist_head (&list);
              llist_unlink_fast_ (&element->node);
              free(element->ptr[0]);
              free(element->ptr[1]);
              mpool_free (&mypool, element);
            }
        }

      LLIST_WHILE_HEAD (&list, element)
        {
          llist_unlink_fast_ (element);
          free(((struct teststruct*)element)->ptr[0]);
          free(((struct teststruct*)element)->ptr[1]);
          mpool_free (&mypool, element);
        }
    }

  mpool_destroy (&mypool);
  ECHO ("destroyed");
}


TEST (bench_malloc_sim)
{
  mpool mypool;
  mpool_init (&mypool, sizeof(llist), 2000, NULL);
  ECHO ("initialized");

  llist list;
  llist_init (&list);

  for (int j = 100; j; --j)
    {
      for (int i = 1; i <= 50000; ++i)
        {
          struct teststruct* element = malloc (sizeof(*element));
          llist_insert_tail (&list, llist_init (&element->node));
          element->ptr[0] = malloc(100+(mpool_fast_prng()%500));
          element->ptr[1] = malloc(100+(mpool_fast_prng()%500));

          if (!(mpool_fast_prng()%4))
            {
              struct teststruct* element = (struct teststruct*)llist_head (&list);
              llist_unlink_fast_ (&element->node);
              free(element->ptr[0]);
              free(element->ptr[1]);
              free (element);
            }
        }

      LLIST_WHILE_HEAD (&list, element)
        {
          llist_unlink_fast_ (element);
          free(((struct teststruct*)element)->ptr[0]);
          free(((struct teststruct*)element)->ptr[1]);
          free (element);
        }
    }

  mpool_destroy (&mypool);
  ECHO ("destroyed");
}


TESTS_END
