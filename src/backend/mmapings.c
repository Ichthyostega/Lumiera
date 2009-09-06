/*
  mmapings.c  -  manage ranges of mmaped areas on a filedescriptor

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

#include "include/logging.h"
#include "lib/mutex.h"
#include "lib/safeclib.h"


#include "backend/mmapings.h"
#include "backend/mmapcache.h"


/**
 * @file
 *
 */


LumieraMMapings
lumiera_mmapings_init (LumieraMMapings self, LumieraFile file, size_t chunksize)
{
  TRACE (mmapings_dbg);
  REQUIRE (!file->descriptor->mmapings);

  llist_init (&self->mmaps);
  self->descriptor = file->descriptor;
  self->chunksize = chunksize;

  lumiera_mutex_init (&self->lock, "mmapings", &NOBUG_FLAG(mutex_dbg));

  return self;
}


LumieraMMapings
lumiera_mmapings_destroy (LumieraMMapings self)
{
  TRACE (mmapings_dbg);
  if (!self)
    return NULL;

  LLIST_WHILE_TAIL (&self->mmaps, node)
    {
      LumieraMMap mmap = LLIST_TO_STRUCTP (node, lumiera_mmap, searchnode);
      lumiera_mmap_delete (mmap);
    }

  lumiera_mutex_destroy (&self->lock, &NOBUG_FLAG(mutex_dbg));

  return self;
}


LumieraMMapings
lumiera_mmapings_new (LumieraFile file, size_t chunksize)
{
  TRACE (mmapings_dbg);
  LumieraMMapings self = lumiera_malloc (sizeof (*self));
  return lumiera_mmapings_init (self, file, chunksize);
}


void
lumiera_mmapings_delete (LumieraMMapings self)
{
  TRACE (mmapings_dbg);
  free (lumiera_mmapings_destroy (self));
}


LumieraMMap
lumiera_mmapings_mmap_acquire (LumieraMMapings self, LumieraFile file, LList acquirer, off_t start, size_t size)
{
  TRACE (mmapings_dbg);

  LumieraMMap ret = NULL;

  LUMIERA_MUTEX_SECTION (mutex_sync, &self->lock)
    {
      REQUIRE (llist_is_empty (acquirer));

      /* find first matching mmap, crude way */
      LLIST_FOREACH (&self->mmaps, node)
        {
          TODO ("improve this selection algorithm, choose mmaps by size, move mfu to head etc");

          LumieraMMap mmap = LLIST_TO_STRUCTP (node, lumiera_mmap, searchnode);

          if (mmap->size >= size && mmap->start <= start && mmap->start+mmap->size >= start+size)
            {
              ret = mmap;
              break;
            }
        }

      /* found? */
      if (ret)
        {
          if (!ret->refcnt)
            /* in cache, needs to me checked out */
            lumiera_mmapcache_checkout (lumiera_mcache, ret);
        }
      else
        {
          /* create new mmap */
          TRACE (mmapings_dbg, "mmap not found, creating");
          ret = lumiera_mmap_new (file, start, size);

          llist_insert_head (&self->mmaps, &ret->searchnode);

          TODO ("sort search list?");
        }

      llist_insert_head (&ret->cachenode, acquirer);
    }

  return ret;
}


void
lumiera_mmapings_release_mmap (LumieraMMapings self, LList acquirer, LumieraMMap map)
{
  TRACE (mmapings_dbg);

  LUMIERA_MUTEX_SECTION (mutex_sync, &self->lock)
    {
      llist_unlink (acquirer);
      if (llist_is_empty (&map->cachenode))
        {
          TRACE (mmapcache_dbg, "checkin");
          lumiera_mmapcache_checkin (lumiera_mcache, map);
        }
    }
}


/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
