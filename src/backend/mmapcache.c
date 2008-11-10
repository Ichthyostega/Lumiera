/*
  mmapcache.c  -  handle aging of mmap objects

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

#include "lib/safeclib.h"

#include "backend/mmapcache.h"

/**
 * @file
 *
 */

NOBUG_DEFINE_FLAG_PARENT (mmapcache, mmap_all);

LumieraMMapcache lumiera_mcache = NULL;


void
lumiera_mmapcache_new (size_t limit)
{
  TRACE (mmapcache);
  lumiera_mcache = lumiera_malloc (sizeof (*lumiera_mcache));

  lumiera_mrucache_init (&lumiera_mcache->cache, lumiera_mmap_destroy_node);

  lumiera_mcache->limit = limit;
  lumiera_mcache->total = 0;
  lumiera_mcache->cached = 0;

  lumiera_mutex_init (&lumiera_mcache->lock, "mmapcache", &NOBUG_FLAG (mmapcache));
}


void
lumiera_mmapcache_delete (void)
{
  TRACE (mmapcache);
  if (lumiera_mcache)
    {
      REQUIRE (lumiera_mcache->total == lumiera_mcache->cached, "MMaps still checked out at shutdown");
      lumiera_mrucache_destroy (&lumiera_mcache->cache);
      lumiera_mutex_destroy (&lumiera_mcache->lock, &NOBUG_FLAG (mmapcache));
      free (lumiera_mcache);
      lumiera_mcache = NULL;
    }
}


void*
lumiera_mmapcache_mmap_acquire (LumieraMMapcache self)
{
  void* map = NULL;

  LUMIERA_MUTEX_SECTION (mmapcache, &self->lock)
    {
      map = lumiera_mrucache_pop (&self->cache);
    }

  if (!map)
    {
      map = lumiera_malloc (sizeof (*self));
      TRACE (mmapcache, "allocated new mmap");
    }
  else
    {
      TRACE (mmapcache, "poped mmap from cache");
    }

  return map;
}


void
lumiera_mmapcache_announce (LumieraMMapcache self, LumieraMMap map)
{
  LUMIERA_MUTEX_SECTION (mmapcache, &self->lock)
    {
      self->total += map->size;
    }
}


void
lumiera_mmapcache_forget (LumieraMMapcache self, LumieraMMap map)
{
  LUMIERA_MUTEX_SECTION (mmapcache, &self->lock)
    {
      if (!llist_is_empty (&map->cachenode))
        {
          TODO ("cached stats");
          llist_unlink (&map->cachenode);
        }
      self->total -= map->size;
    }
}

#if 0
int
lumiera_mmapcache_age (LumieraMMapcache self)
{
  TRACE (mmapcache);
  int ret = 0;

  LUMIERA_MUTEX_SECTION (mmapcache, &self->lock)
    {
      ret = lumiera_mrucache_age (&self->cache, 10);      TODO ("age nelem == 20%(configureable) of the cache");
    }

  return ret;
}
#endif

LumieraMMap
lumiera_mmapcache_checkout (LumieraMMapcache self, LumieraMMap handle)
{
  TRACE (mmapcache);

  LUMIERA_MUTEX_SECTION (mmapcache, &self->lock)
    {
      TODO ("cached stats");
      lumiera_mrucache_checkout (&self->cache, &handle->cachenode);
    }

  return handle;
}


void
lumiera_mmapcache_checkin (LumieraMMapcache self, LumieraMMap handle)
{
  TRACE (mmapcache);

  LUMIERA_MUTEX_SECTION (mmapcache, &self->lock)
    {
      TODO ("cached stats");
      lumiera_mrucache_checkin (&self->cache, &handle->cachenode);
    }
}


/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
