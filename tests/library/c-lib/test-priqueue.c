/*
  TEST_PRIQUEUE  -  test the heap based priority queue implementation

  Copyright (C)         Lumiera.org
    2011,               Christian Thaeter <ct@pipapo.org>

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

/** @file §§§
 ** unit test §§TODO§§
 */


#include "lib/test/test.h"
#include "lib/priqueue.h"
#include "include/logging.h"


/* @note internal helper copied from priqueue.c */
static inline void*
pq_index (LumieraPriQueue self, unsigned nth)
{
  return (char*)self->queue+self->element_size*nth;
}



void
nobug_priqueue_invariant (LumieraPriQueue self, int depth, const struct nobug_context invariant_context, void* extra)
{
  intptr_t n = 1+(intptr_t)extra;

  intptr_t m=n+n;

  if (self && depth && m <= self->used)
    {
      INVARIANT_ASSERT (self->cmpfn (pq_index(self, n-1), pq_index(self, m-1)) <= 0, "%d %d", (int)n-1, (int)m-2);
      nobug_priqueue_invariant (self, depth-1, invariant_context, (void*)m-1);

      if (m<self->used)
        {
          INVARIANT_ASSERT (self->cmpfn (pq_index(self, n-1), pq_index(self, m)) <= 0, "%d %d", (int)n-1, (int)m-1);
          nobug_priqueue_invariant (self, depth-1, invariant_context, (void*)m);
        }
    }
}


static int
cmpintptr (void* a, void* b)
{
  return *(int*)a - *(int*)b;
}









TESTS_BEGIN

  lumiera_priqueue pq;

  LumieraPriQueue r;

  int data, prev, curr;

  r = lumiera_priqueue_init (&pq,
                             sizeof (int),
                             cmpintptr,
                             NULL,
                             NULL);
  ENSURE (r==&pq);


  data = 10;
  r = lumiera_priqueue_insert (&pq, &data);
  ENSURE (r==&pq);
  TRACE (test, "inserted %d", data);


  data = 5;
  r = lumiera_priqueue_insert (&pq, &data);
  ENSURE (r==&pq);
  TRACE (test, "inserted %d", data);


  data = 15;
  r = lumiera_priqueue_insert (&pq, &data);
  ENSURE (r==&pq);
  TRACE (test, "inserted %d", data);

  data = 20;
  r = lumiera_priqueue_insert (&pq, &data);
  ENSURE (r==&pq);
  TRACE (test, "inserted %d", data);




  for (int i = 0; i < 100000; ++i)
    {
      data = i;
      r = lumiera_priqueue_insert (&pq, &data);
      ENSURE (r==&pq);
      TRACE (test, "inserted %d", data);
    }


  for (int i = 0; i < 100000; ++i)
    {
      data = rand()%1000000;
      r = lumiera_priqueue_insert (&pq, &data);
      ENSURE (r==&pq);
      TRACE (test, "inserted %d", data);
    }

  NOBUG_INVARIANT(priqueue, &pq, 100, NULL);


  prev = 0;
  for (int i = 0; pq.used; ++i)
    {
      curr = *(int*)lumiera_priqueue_peek (&pq);
      TRACE (test, "TOP: %d", curr);
      CHECK (prev <= curr, "priority ordering broken");
      prev = curr;

      r = lumiera_priqueue_remove (&pq);
      ENSURE (r==&pq);
    }


  r = lumiera_priqueue_destroy (&pq);
  ENSURE (r==&pq);



TESTS_END
