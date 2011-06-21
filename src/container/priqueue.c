/*
    priqueue - priority queue

  Copyright (C)
    2011                        Christian Thaeter <ct@pipapo.org>

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

#include "priqueue.h"


#include <nobug.h>
#include <stdint.h>


NOBUG_DEFINE_FLAG (priqueue);


PriQueue
priqueue_init (PriQueue self,
               size_t element_size,
               priqueue_cmp_fn cmpfn,
               priqueue_copy_fn copyfn,
               priqueue_resize_fn resizefn)
{
  NOBUG_INIT_FLAG (priqueue);
  TRACE (priqueue, "%p", self);

  REQUIRE (element_size);
  REQUIRE (cmpfn);

  if (self)
    {
      self->queue = NULL;
      self->element_size = element_size;
      self->used = self->high_water = self->low_water = 0;
      self->cmpfn = cmpfn;

      if (!copyfn)
        copyfn = memcpy;
      self->copyfn = copyfn;

      if (!resizefn)
        resizefn = priqueue_clib_resize;
      self->resizefn = resizefn;

      self = self->resizefn (self);
    }
  return self;
}



PriQueue
priqueue_destroy (PriQueue self)
{
  TRACE (priqueue, "%p", self);
  if (self)
    {
      WARN_IF (self->used, priqueue, "queue was not empty");
      self->used = 0;
      self = self->resizefn (self);
    }
  return self;
}


PriQueue
priqueue_reserve (PriQueue self, unsigned elements)
{
  TRACE (priqueue, "%p %d", self, elements);
  if (self)
    {
      if (self->used+elements >= self->high_water)
        {
          self->used += elements;
          self = self->resizefn (self);
          if (!self)
            {
              ERROR (priqueue, "resize failed");
              return NULL;
            }
          self->used -= elements;
        }
      self->low_water = 0;
    }

  return self;
}


/*
  supplied/default resize function based on realloc
*/
PriQueue
priqueue_clib_resize (PriQueue self)
{
  if (self)
    {
      if (!self->queue)
        {
          INFO (priqueue, "%p: initial alloc", self);
          self->queue = malloc (64*self->element_size);
          ERROR_IF (!self->queue, priqueue, "%p: allocation failed", self);
          if (!self->queue)
            return NULL;
          self->high_water = 64;
        }
      else
        {
          if (self->used)
            {
              if (self->used >= self->high_water)
                {
                  unsigned newwater = self->high_water;
                  while (self->used >= newwater)
                    newwater *= 2;

                  INFO (priqueue, "%p: resize %d -> %d", self, self->high_water, newwater);

                  void* newqueue = realloc (self->queue, self->element_size * newwater);
                  ERROR_IF (!newqueue, priqueue, "%p: allocation failed", self);
                  if (!newqueue)
                    return NULL;
                  self->queue = newqueue;
                  self->high_water = newwater;
                  self->low_water = self->high_water/8-8;
                  TRACE (priqueue, "%p: low_water: %d", self, self->low_water);
                }
              else
                {
                  INFO (priqueue, "%p: shrink %d -> %d", self, self->high_water, (self->low_water+8)*4);
                  void* newqueue = realloc (self->queue, self->element_size * (self->low_water+8)*4);

                  ERROR_IF (!newqueue, priqueue, "allocation failed");
                  if (!newqueue)
                    return NULL;
                  self->queue = newqueue;
                  self->high_water = (self->low_water+8)*4;
                  self->low_water = self->high_water/8-8;
                  TRACE (priqueue, "%p: low_water: %d", self, self->low_water);
                }
            }
          else
            {
              INFO (priqueue, "%p: freeing", self);
              free (self->queue);
              self->queue = NULL;
            }
        }
    }
  return self;
}



static inline void*
pq_index (PriQueue self, unsigned nth)
{
  return (char*)self->queue+self->element_size*nth;
}


static inline void
pq_up (PriQueue self, void* tmp)
{
  unsigned i = self->used;
  unsigned p = i/2;

  while (p && self->cmpfn (tmp, pq_index(self, p-1)) < 0)
    {
      self->copyfn (pq_index (self, i-1), pq_index (self, p-1), self->element_size);
      i=p; p=i/2;
    }

  self->copyfn (pq_index (self, i-1), tmp, self->element_size);
}



PriQueue
priqueue_insert (PriQueue self, void* element)
{
  TRACE (priqueue, "%p: insert %p", self, element);

  if (self && self->used >= self->high_water)
    self = self->resizefn (self);

  if (self)
    {
      ++self->used;
      pq_up (self, element);
    }
  return self;
}




static inline void
pq_down (PriQueue self, void* tmp)
{
  if (!self->used)
    return;

  unsigned i = 1;

  while (i <= self->used/2)
    {
      unsigned n=i+i;
      if (n<self->used && self->cmpfn (pq_index(self, n-1), pq_index(self, n)) >= 0)
        ++n;

      if (self->cmpfn (tmp, pq_index(self, n-1)) < 0)
        break;

      self->copyfn (pq_index (self, i-1), pq_index (self, n-1), self->element_size);
      i = n;
    }
  self->copyfn (pq_index (self, i-1), tmp, self->element_size);
}


PriQueue
priqueue_remove (PriQueue self)
{
  TRACE (priqueue, "%p: remove", self);

  if (self)
    {
      if (!self->used)
        return NULL;

      --self->used;
      pq_down (self, pq_index (self, self->used));

      if (self->used < self->low_water)
        self = self->resizefn (self);
    }

  return self;
}









#ifdef PRIQUEUE_TEST  /* testing */

#include <stdio.h>


void
nobug_priqueue_invariant (PriQueue self, int depth, const struct nobug_context invariant_context, void* extra)
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


int main()
{
  NOBUG_INIT;

  priqueue pq;

  PriQueue r;

  int data;

  r = priqueue_init (&pq,
                     sizeof (int),
                     cmpintptr,
                     NULL,
                     NULL);
  ENSURE (r==&pq);

#if 1
  data = 10;
  r = priqueue_insert (&pq, &data);
  ENSURE (r==&pq);
  ECHO("inserted %d", data);
#endif

#if 0
  data = 5;
  r = priqueue_insert (&pq, &data);
  ENSURE (r==&pq);
  ECHO("inserted %d", data);
#endif


#if 0
  data = 15;
  r = priqueue_insert (&pq, &data);
  ENSURE (r==&pq);
  ECHO("inserted %d", data);

  data = 20;
  r = priqueue_insert (&pq, &data);
  ENSURE (r==&pq);
  ECHO("inserted %d", data);
#endif




#if 1
  for (int i = 0; i < 100000; ++i)
    {
      data = i;
      r = priqueue_insert (&pq, &data);
      ENSURE (r==&pq);
      ECHO("inserted %d", data);
    }

#endif

#if 1
  for (int i = 0; i < 100000; ++i)
    {
      ECHO("PRE %d", i);
      data = rand()%100000;
      r = priqueue_insert (&pq, &data);
      ENSURE (r==&pq);
      ECHO("inserted %d", data);
    }
#endif

  NOBUG_INVARIANT(priqueue, &pq, 100, NULL);


#if 1
  for (int i = 0; pq.used; ++i)
    {
      ECHO ("TOP: %d", *(int*)priqueue_peek (&pq));
      r = priqueue_remove (&pq);
      ENSURE (r==&pq);
      ECHO("poped");
    }
#endif


  r = priqueue_destroy (&pq);
  ENSURE (r==&pq);

  return 0;
}




#endif
