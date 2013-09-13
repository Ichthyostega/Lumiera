/*
  PriQueue  -  simple heap based priority queue

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


#include "lib/priqueue.h"
#include "include/logging.h"

#include <stdint.h>
#include <nobug.h>





LumieraPriQueue
lumiera_priqueue_init (LumieraPriQueue self,
                       size_t element_size,
                       lumiera_priqueue_cmp_fn cmpfn,
                       lumiera_priqueue_copy_fn copyfn,
                       lumiera_priqueue_resize_fn resizefn)
{
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
        resizefn = lumiera_priqueue_clib_resize;
      self->resizefn = resizefn;

      self = self->resizefn (self);
    }
  return self;
}



LumieraPriQueue
lumiera_priqueue_destroy (LumieraPriQueue self)
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



LumieraPriQueue
lumiera_priqueue_reserve (LumieraPriQueue self, unsigned elements)
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



LumieraPriQueue
lumiera_priqueue_clib_resize (LumieraPriQueue self)
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
pq_index (LumieraPriQueue self, unsigned nth)
{
  return (char*)self->queue+self->element_size*nth;
}


static inline void
pq_up (LumieraPriQueue self, void* tmp)
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



LumieraPriQueue
lumiera_priqueue_insert (LumieraPriQueue self, void* element)
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
pq_down (LumieraPriQueue self, void* tmp)
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


LumieraPriQueue
lumiera_priqueue_remove (LumieraPriQueue self)
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
