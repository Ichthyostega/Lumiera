/*
    A cuckoo hash implementation

  Copyright (C)
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

#include "cuckoo.h"

#include <string.h>

#define CUCKOO_GRANULARITY int

enum compact_state
  {
    COMPACTING_OFF,
    COMPACTING_AUTO,
    COMPACTED
  };

struct cuckoo_struct
{
  size_t size;          /* t1 = 4*size; t2 = 2*size; t3 = size */
  size_t itemsize;

  cuckoo_hashfunc h1;   /* hash function */
  uint32_t r1;          /* random, reset for each rehash */
  cuckoo_hashfunc h2;
  uint32_t r2;
  cuckoo_hashfunc h3;
  uint32_t r3;

  cuckoo_cmpfunc cmp;

  void* t1;
  void* t2;
  void* t3;

  unsigned maxloops;    /* sqrt (4 * size) */

  enum compact_state autocompact;
  size_t elements;

  cuckoo_dtorfunc dtor;
  cuckoo_cpyfunc cpy;
};


static inline uint32_t cuckoo_fast_prng ()
{
  static uint32_t rnd = 0xbabeface;
  return rnd = rnd<<1 ^ ((rnd>>30) & 1) ^ ((rnd>>2) & 1);
}

static inline int
iszero (void* mem, size_t size)
{
  while (size && !*(CUCKOO_GRANULARITY*)mem)
    {
      size -= sizeof (CUCKOO_GRANULARITY);
      mem += sizeof (CUCKOO_GRANULARITY);
    }
  return !size;
}

static inline void
xmemcpy (void* dst, const void* src, size_t size)
{
  while (size)
    {
      size -= sizeof (CUCKOO_GRANULARITY);
      *(CUCKOO_GRANULARITY*)(dst + size) = *(CUCKOO_GRANULARITY*)(src + size);
    }
}


Cuckoo
cuckoo_init (Cuckoo self,
             cuckoo_hashfunc h1,
             cuckoo_hashfunc h2,
             cuckoo_hashfunc h3,
             cuckoo_cmpfunc cmp,
             size_t itemsize,
             unsigned startsize,
             cuckoo_dtorfunc dtor,
             cuckoo_cpyfunc cpy)
{
  if (!self)
    return NULL;

  self->size = 1<<startsize;
  self->itemsize = (itemsize * sizeof (CUCKOO_GRANULARITY)
                    + sizeof (CUCKOO_GRANULARITY) - 1) / sizeof (CUCKOO_GRANULARITY); /* round up to next CUCKOO_GRANULARITY boundary */
  self->h1 = h1;
  self->r1 = cuckoo_fast_prng ();
  self->h2 = h2;
  self->r2 = cuckoo_fast_prng ();
  self->h3 = h3;
  self->r3 = cuckoo_fast_prng ();

  self->cmp = cmp;

  self->t1 = calloc (self->size * 4, itemsize);
  self->t2 = calloc (self->size * 2, itemsize);
  self->t3 = calloc (self->size, itemsize);
  if (!self->t1 || !self->t2 || !self->t3)
    {
      free (self->t1);
      free (self->t2);
      free (self->t3);
      return NULL;
    }

  self->maxloops = 1;
  while (self->maxloops * self->maxloops < self->size * 4)
    ++self->maxloops;

  self->autocompact = COMPACTING_AUTO;
  self->elements = 0;

  self->dtor = dtor;
  self->cpy = cpy ? cpy : xmemcpy;
  return self;
}

Cuckoo
cuckoo_new (cuckoo_hashfunc h1,
            cuckoo_hashfunc h2,
            cuckoo_hashfunc h3,
            cuckoo_cmpfunc cmp,
            size_t itemsize,
            unsigned startsize,
            cuckoo_dtorfunc dtor,
            cuckoo_cpyfunc cpy)
{
  Cuckoo self = malloc (sizeof (struct cuckoo_struct));
  if (!cuckoo_init (self, h1, h2, h3, cmp, itemsize, startsize, dtor, cpy))
    {
      free (self);
      return NULL;
    }
  return self;
}

Cuckoo
cuckoo_destroy (Cuckoo self)
{
  if (self)
    {

      if (self->dtor)
        {
          void* elem;
          for (elem = self->t1; elem < self->t1 + self->size * 4; elem += self->size)
            self->dtor (elem);
          for (elem = self->t2; elem < self->t1 + self->size * 2; elem += self->size)
            self->dtor (elem);
          for (elem = self->t3; elem < self->t1 + self->size; elem += self->size)
            self->dtor (elem);
        }

      free (self->t1);
      free (self->t2);
      free (self->t3);
    }
  return self;
}


void
cuckoo_delete (Cuckoo self)
{
  free (cuckoo_destroy (self));
}


static int
cuckoo_insert_internal_ (Cuckoo self, void* item)
{
  void* pos;
  char tmp[self->itemsize];

  for (unsigned n = 0; n < self->maxloops; ++n)
    {
      /* find nest */
      pos = self->t1 + self->itemsize * (self->h1 (item, self->r1) % (4*self->size));
      /* kick old egg out */
      self->cpy (tmp, pos, self->itemsize);
      /* lay egg */
      self->cpy (pos, item, self->itemsize);

      if (iszero (tmp, self->itemsize))
        return 1;

      /* find nest */
      pos = self->t2 + self->itemsize * (self->h2 (tmp, self->r2) % (2*self->size));
      /* kick old egg out */
      self->cpy (item, pos, self->itemsize);
      /* lay egg */
      self->cpy (pos, tmp, self->itemsize);

      if (iszero (item, self->itemsize))
        return 1;

      /* find nest */
      pos = self->t3 + self->itemsize * (self->h3 (item, self->r3) % self->size);
      /* kick old egg out */
      self->cpy (tmp, pos, self->itemsize);
      /* lay egg */
      self->cpy (pos, item, self->itemsize);

      if (iszero (tmp, self->itemsize))
        return 1;

      /* copy tmp to item, which will be reinserted on next interation / after rehashing */
      self->cpy (item, tmp, self->itemsize);
    }
  return 0;
}


size_t
cuckoo_nelements (Cuckoo self)
{
  return self->elements;
}


static void
cuckoo_rehash (Cuckoo self)
{
 retry1:

  self->r1 = cuckoo_fast_prng ();

  for (size_t i = 0; i < 4*self->size; ++i)
    {
      unsigned n;
      void* pos = self->t1 + self->itemsize * i;
      if (!iszero (pos, self->itemsize))
        {
          for (n = 0; n < self->maxloops; ++n)
            {
              unsigned hash = self->h1 (pos, self->r1) % (4*self->size);
              if (hash != i)
                {
                  char t[self->itemsize];
                  void* hpos = self->t1 + self->itemsize * hash;
                  self->cpy (t, hpos, self->itemsize);
                  self->cpy (hpos, pos, self->itemsize);
                  self->cpy (pos, t, self->itemsize);
                  if (iszero (t, self->itemsize))
                    break;
                }
              else
                break;
            }
          if (n == self->maxloops)
            goto retry1;
        }
    }

 retry2:
  self->r2 = cuckoo_fast_prng ();

  for (size_t i = 0; i < 2*self->size; ++i)
    {
      unsigned n;
      void* pos = self->t2 + self->itemsize * i;
      if (!iszero (pos, self->itemsize))
        {
          for (n = 0; n < self->maxloops; ++n)
            {
              unsigned hash = self->h2 (pos, self->r2) % (2*self->size);
              if (hash != i)
                {
                  char t[self->itemsize];
                  void* hpos = self->t2 + self->itemsize * hash;
                  self->cpy (t, hpos, self->itemsize);
                  self->cpy (hpos, pos, self->itemsize);
                  self->cpy (pos, t, self->itemsize);
                  if (iszero (t, self->itemsize))
                    break;
                }
              else
                break;
            }
          if (n == self->maxloops)
            goto retry2;
        }
    }

 retry3:
  self->r3 = cuckoo_fast_prng ();

  for (size_t i = 0; i < self->size; ++i)
    {
      unsigned n;
      void* pos = self->t3 + self->itemsize * i;
      if (!iszero (pos, self->itemsize))
        {
          for (n = 0; n < self->maxloops; ++n)
            {
              unsigned hash = self->h3 (pos, self->r3) % self->size;
              if (hash != i)
                {
                  char t[self->itemsize];
                  void* hpos = self->t3 + self->itemsize * hash;
                  self->cpy (t, hpos, self->itemsize);
                  self->cpy (hpos, pos, self->itemsize);
                  self->cpy (pos, t, self->itemsize);
                  if (iszero (t, self->itemsize))
                    break;
                }
              else
                break;
            }
          if (n == self->maxloops)
            goto retry3;
        }
    }
}

static int
cuckoo_grow (Cuckoo self)
{
  /* rotate hashfuncs, tables, randoms */
  cuckoo_hashfunc th = self->h3;
  self->h3 = self->h2;
  self->h2 = self->h1;
  self->h1 = th;

  uint32_t tr = self->r3;
  self->r3 = self->r2;
  self->r2 = self->r1;
  self->r1 = tr;

  void* tt = self->t3;
  self->t3 = self->t2;
  self->t2 = self->t1;

  /* double new base size */
  self->size *= 2;
  while (self->maxloops * self->maxloops < self->size * 4)
    ++self->maxloops;

  /* alloc new t1 */
  self->t1 = calloc (self->size * 4, self->itemsize);
  if (!self->t1)
    {
      self->t1 = tt;
      return 0;
    }

  /* reinsert tt */
  size_t ttsize = self->size / 2;
  for (size_t i = 0; i < ttsize; ++i)
    {
      void* pos = tt + i * self->itemsize;
      if (!iszero (pos, self->itemsize))
        {
          while (!cuckoo_insert_internal_ (self, pos))
            cuckoo_rehash (self);
        }
    }
  free (tt);

  self->autocompact = COMPACTING_AUTO;
  return 1;
}


int
cuckoo_reserve (Cuckoo self, size_t more)
{
  int ret = 1;
  if (more)
    while (self->elements+self->maxloops+more >= 6*self->size)
      ret = cuckoo_grow (self);

  self->autocompact = COMPACTING_OFF;
  return ret;
}


int
cuckoo_compact (Cuckoo self)
{
  if (self->autocompact == COMPACTED)
    return 1;

  if (self->size > 2 && self->elements < self->size * 3)
    {
      cuckoo_hashfunc th = self->h1;
      self->h1 = self->h2;
      self->h2 = self->h3;
      self->h3 = th;

      uint32_t tr = self->r1;
      self->r1 = self->r2;
      self->r2 = self->r3;
      self->r3 = tr;

      void* tt = self->t1;
      self->t1 = self->t2;
      self->t2 = self->t3;

      /* halve base size */
      self->size /= 2;
      while (self->maxloops * self->maxloops >= self->size * 4)
        --self->maxloops;

      /* alloc new t3 */
      self->t3 = calloc (self->size, self->itemsize);
      if (!self->t3)
        {
          self->t3 = tt;
          return 0;
        }

      /* reinsert tt */
      size_t ttsize = self->size * 8;
      for (size_t i = 0; i < ttsize; ++i)
        {
          void* pos = tt + i * self->itemsize;
          if (!iszero (pos, self->itemsize))
            {
              --self->elements;
              cuckoo_insert (self, pos);
            }
        }
      free (tt);
      self->autocompact = COMPACTED;
    }
  return 1;
}


int
cuckoo_insert (Cuckoo self, void* item)
{
  char tmp[self->itemsize];

  void* found;
  if ((found = cuckoo_find (self, item)))
    {
      self->cpy (found, item, self->itemsize);
      return 1;
    }

  self->cpy (tmp, item, self->itemsize);

  for (unsigned n = 6; n; --n) /* rehash/grow loop */
    {
      if (cuckoo_insert_internal_ (self, tmp))
        {
          ++self->elements;
          return 1;
        }

      if (self->elements > n*self->size)
        {
          n = 6;
          if (!cuckoo_grow (self))
            return 0;
        }
      else
        cuckoo_rehash (self);
    }
  return 0;
}


void*
cuckoo_find (Cuckoo self, void* item)
{
  void* pos;

  pos = self->t1 + self->itemsize * (self->h1 (item, self->r1) % (4*self->size));
  if (!iszero (pos, self->itemsize) && self->cmp (item, pos))
    return pos;

  pos = self->t2 + self->itemsize * (self->h2 (item, self->r2) % (2*self->size));
  if (!iszero (pos, self->itemsize) && self->cmp (item, pos))
    return pos;

  pos = self->t3 + self->itemsize * (self->h3 (item, self->r3) % self->size);
  if (!iszero (pos, self->itemsize) && self->cmp (item, pos))
    return pos;

  return NULL;
}


void
cuckoo_remove (Cuckoo self, void* item)
{
  if (item)
    {
      if (self->dtor)
        self->dtor (item);

      memset (item, 0, self->itemsize);
      --self->elements;

      if (self->autocompact == COMPACTING_AUTO && self->size > 2 && self->elements <= self->size*2)
        cuckoo_compact (self);
    }
}


/*
//      Local Variables:
//      mode: C
//      c-file-style: "gnu"
//      indent-tabs-mode: nil
//      End:
*/
