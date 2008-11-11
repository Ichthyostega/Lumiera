/*
  filehandlecache  -  filehandle management and caching

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

#include "backend/file.h"
#include "backend/filehandlecache.h"

NOBUG_DEFINE_FLAG_PARENT (filehandlecache, file_all);

/* errors */

LUMIERA_ERROR_DEFINE (FILEHANDLECACHE_NOHANDLE, "No filehandle available");


LumieraFilehandlecache lumiera_fhcache = NULL;


void
lumiera_filehandlecache_new (int max_entries)
{
  REQUIRE (!lumiera_fhcache, "Filehandlecache already initialized");

  NOBUG_INIT_FLAG (filehandlecache);
  lumiera_fhcache = lumiera_malloc (sizeof (lumiera_filehandlecache));
  lumiera_mrucache_init (&lumiera_fhcache->cache, lumiera_filehandle_destroy_node);
  lumiera_fhcache->available = max_entries;
  lumiera_fhcache->checked_out = 0;
  lumiera_mutex_init (&lumiera_fhcache->lock, "filehandlecache", &NOBUG_FLAG (filehandlecache));
}


void
lumiera_filehandlecache_delete (void)
{
  if (lumiera_fhcache)
    {
      REQUIRE (!lumiera_fhcache->checked_out, "Filehandles in use at shutdown");
      lumiera_mrucache_destroy (&lumiera_fhcache->cache);
      lumiera_mutex_destroy (&lumiera_fhcache->lock, &NOBUG_FLAG (filehandlecache));
      lumiera_free (lumiera_fhcache);
      lumiera_fhcache = NULL;
    }
}


LumieraFilehandle
lumiera_filehandlecache_handle_acquire (LumieraFilehandlecache self, LumieraFiledescriptor desc)
{
  TRACE (filehandlecache);
  LumieraFilehandle ret = NULL;

  LUMIERA_MUTEX_SECTION (filehandlecache, &self->lock)
  {
    if (self->available <= 0 && self->cache.cached)
      {
        /* pop a filehandle from cache */
        ret = lumiera_mrucache_pop (&self->cache);
        ret = lumiera_filehandle_init (lumiera_mrucache_pop (&self->cache), desc);
        if (self->available < 0)
          /* try to free overallocated filehandles */
          self->available -= self->available + lumiera_mrucache_age (&self->cache, -self->available);
      }
    else
      {
        /* allocate new filehandle if we are below the limit or no cached handles are available (overallocating) */
        NOTICE (filehandlecache, "overallocating filehandle");
        ret = lumiera_filehandle_new (desc);
        if (!ret)
          LUMIERA_ERROR_SET (filehandlecache, FILEHANDLECACHE_NOHANDLE);
        else
          --self->available;
      }
    desc->handle = ret;
    ++self->checked_out;
  }
  return ret;
}


LumieraFilehandle
lumiera_filehandlecache_checkout (LumieraFilehandlecache self, LumieraFilehandle handle)
{
  REQUIRE (self);
  REQUIRE (handle);
  /* This function is called with the associated descriptor locked, nothing can modify 'handle' */
  if (!handle->use_cnt)
    {
      /* lock cache and checkout */
      LUMIERA_MUTEX_SECTION (filehandlecache, &self->lock)
        {
          lumiera_mrucache_checkout (&self->cache, &handle->cachenode);
        }
    }
  ++handle->use_cnt;
  ++self->checked_out;

  return handle;
}

void
lumiera_filehandlecache_checkin (LumieraFilehandlecache self, LumieraFilehandle handle)
{
  REQUIRE (self);
  REQUIRE (handle);
  REQUIRE (handle->use_cnt);

  /* This function is called with the associated descriptor locked, nothing can modify 'self' */
  if (!--handle->use_cnt)
    {
      /* lock cache and checin */
      LUMIERA_MUTEX_SECTION (filehandlecache, &self->lock)
        {
          --self->checked_out;
          lumiera_mrucache_checkin (&self->cache, &handle->cachenode);
        }
    }
}

