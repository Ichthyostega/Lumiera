/*
  MMAP-Cache  -  handle aging of mmap objects

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

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


/** @file mmapcache.c
 ** Implementation of caching for currently unused memory mapped file regions
 ** @todo development in this area is stalled since 2010
 */


#include "include/logging.h"
#include "lib/safeclib.h"

#include "backend/mmapcache.h"



LumieraMMapcache lumiera_mcache = NULL;


void
lumiera_mmapcache_new (size_t limit)
{
  TRACE (mmapcache_dbg);
  lumiera_mcache = lumiera_malloc (sizeof (*lumiera_mcache));

  lumiera_mrucache_init (&lumiera_mcache->cache, lumiera_mmap_destroy_node);

  lumiera_mcache->limit = limit;
  lumiera_mcache->total = 0;
  lumiera_mcache->cached = 0;

  lumiera_mutex_init (&lumiera_mcache->lock, "mmapcache", &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);
}


void
lumiera_mmapcache_delete (void)
{
  TRACE (mmapcache_dbg);
  if (lumiera_mcache)
    {
      REQUIRE (lumiera_mcache->total == lumiera_mcache->cached, "MMaps still checked out at shutdown");
      lumiera_mrucache_destroy (&lumiera_mcache->cache);
      lumiera_mutex_destroy (&lumiera_mcache->lock, &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);
      free (lumiera_mcache);
      lumiera_mcache = NULL;
    }
}


void*
lumiera_mmapcache_mmap_acquire (void)
{
  TRACE (mmapcache_dbg);
  void* map = NULL;

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_mcache->lock)
    {
      map = lumiera_mrucache_pop (&lumiera_mcache->cache);
    }

  if (!map)
    {
      map = lumiera_malloc (sizeof (lumiera_mmap));
      TRACE (mmapcache_dbg, "allocated new mmap");
    }
  else
    {
      TRACE (mmapcache_dbg, "Popped mmap from cache");
    }

  return map;
}


void
lumiera_mmapcache_announce (LumieraMMap map)
{
  TRACE (mmapcache_dbg);
  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_mcache->lock)
    {
      lumiera_mcache->total += map->size;
    }
}


void
lumiera_mmapcache_forget (LumieraMMap map)
{
  TRACE (mmapcache_dbg);
  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_mcache->lock)
    {
      if (!llist_is_empty (&map->cachenode))
        {
          TODO ("cached stats");
          REQUIRE (llist_is_member (&lumiera_mcache->cache.cache_list, &map->cachenode), "Map object not in cache");
          llist_unlink (&map->cachenode);
        }
      lumiera_mcache->total -= map->size;
    }
}

#if 0
int
lumiera_mmapcache_age (void)
{
  TRACE (mmapcache_dbg);
  int ret = 0;

  LUMIERA_MUTEX_SECTION (mmapcache, &lumiera_mcache->lock)
    {
      ret = lumiera_mrucache_age (&lumiera_mcache->cache, 10);      TODO ("age nelem == 20%(configurable) of the cache");
    }

  return ret;
}
#endif

LumieraMMap
lumiera_mmapcache_checkout (LumieraMMap handle)
{
  TRACE (mmapcache_dbg);
  REQUIRE (handle->refcnt == 0);

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_mcache->lock)
    {
      ////////////////////TODO cached stats
      lumiera_mrucache_checkout (&lumiera_mcache->cache, &handle->cachenode);
    }

  return handle;
}


void
lumiera_mmapcache_checkin (LumieraMMap handle)
{
  TRACE (mmapcache_dbg);
  REQUIRE (handle->refcnt == 0);

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_mcache->lock)
    {
      ////////////////////TODO cached stats
      lumiera_mrucache_checkin (&lumiera_mcache->cache, &handle->cachenode);
    }
}


/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
