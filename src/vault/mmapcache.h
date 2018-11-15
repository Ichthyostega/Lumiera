/*
  MMAPCACHE.h  -  handle aging of mmap objects

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

*/


/** @file mmapcache.h
 ** Lumiera's Mmapcache stores a MRU cache of all established mmaped memory regions which are currently not in use.
 ** The mmapcache also manages the upper limit about how much memory can be mmaped.
 */

#ifndef BACKEND_MMAPCACHE_H
#define BACKEND_MMAPCACHE_H

#include "lib/error.h"
#include "lib/mrucache.h"
#include "lib/mutex.h"


typedef struct lumiera_mmapcache_struct lumiera_mmapcache;
typedef lumiera_mmapcache* LumieraMMapcache;

#include "backend/mmap.h"

#include <nobug.h>




struct lumiera_mmapcache_struct
{
  lumiera_mrucache cache;
  size_t limit;
  size_t total;
  size_t cached;
  lumiera_mutex lock;
};


/**
 * Initialises the mmapcache.
 * @param limit the mmapcache will drop elements when the sum of all mmappings gives over limit
 */
void
lumiera_mmapcache_new (size_t limit);

/**
 * Delete the mmap cache.
 * No mmaps in the cache must be locked, this would be a fatal error.
 * The handles are closed automatically.
 */
void
lumiera_mmapcache_delete (void);

/**
 * Get a fresh mmap object.
 * when mmaped_limit is reached, the oldest mmap object gets dropped else
 * a new allocated object is returned
 * @return the new uninitialised mmap (void* because this is uninitialised)
 */
void*
lumiera_mmapcache_mmap_acquire (void);


/**
 * Announce a new mmap object to the cache quotas.
 * Update the statistics kept in the cache,
 * the map object is still considered to be checked out
 * @param map object to be announced
 */
void
lumiera_mmapcache_announce (LumieraMMap map);

/**
 * Remove a mmap object from the cache quotas.
 * Update the statistics kept in the cache, remove it from the cache.
 * @param map object to be removed
 */
void
lumiera_mmapcache_forget (LumieraMMap map);

/**
 * Destroy and free the nelem oldest elements.
 * Used to free up resources and memory.
 * @return nelem-(number of elements which got freed), that is 0 if all requested elements got freed
 */
int
lumiera_mmapcache_age (void);

/**
 * Remove a mmap from cache aging
 * Mmaps which are subject of cache aging must be checked out before they can be used.
 * @param handle the mmap to be checked out
 */
LumieraMMap
lumiera_mmapcache_checkout (LumieraMMap handle);

/**
 * Put a mmap into the cache
 * Mmaps which are checked in are subject of cache aging and might get destroyed and reused.
 * @param handle the mmap to be checked in
 */
void
lumiera_mmapcache_checkin (LumieraMMap handle);


#endif /*BACKEND_MMAPCACHE_H*/
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
