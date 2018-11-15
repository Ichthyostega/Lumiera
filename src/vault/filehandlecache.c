/*
  FileHandleCache  -  filehandle management and caching

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


/** @file filehandlecache.c
 ** Implementation of caching for filehandle representation data
 ** @todo development in this area is stalled since 2010
 */


#include "include/logging.h"
#include "lib/safeclib.h"

#include "vault/file.h"
#include "vault/filehandlecache.h"



LUMIERA_ERROR_DEFINE (FILEHANDLECACHE_NOHANDLE, "No filehandle available");


/**
 * the global cache for file handles.
 */
LumieraFilehandlecache lumiera_fhcache = NULL;



void
lumiera_filehandlecache_new (int max_entries)
{
  REQUIRE (!lumiera_fhcache, "Filehandlecache already initialised");

  lumiera_fhcache = lumiera_malloc (sizeof (lumiera_filehandlecache));
  lumiera_mrucache_init (&lumiera_fhcache->cache, lumiera_filehandle_destroy_node);
  lumiera_fhcache->available = max_entries;
  lumiera_fhcache->checked_out = 0;
  lumiera_mutex_init (&lumiera_fhcache->lock, "filehandlecache", &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);
}


void
lumiera_filehandlecache_delete (void)
{
  if (lumiera_fhcache)
    {
      REQUIRE (!lumiera_fhcache->checked_out, "Filehandles in use at shutdown");
      lumiera_mrucache_destroy (&lumiera_fhcache->cache);
      lumiera_mutex_destroy (&lumiera_fhcache->lock, &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);
      lumiera_free (lumiera_fhcache);
      lumiera_fhcache = NULL;
    }
}


LumieraFilehandle
lumiera_filehandlecache_handle_acquire (LumieraFiledescriptor desc)
{
  TRACE (filehandlecache_dbg);
  LumieraFilehandle ret = NULL;

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_fhcache->lock)
  {
    if (lumiera_fhcache->available <= 0 && lumiera_fhcache->cache.cached)
      {
        /* pop a filehandle from cache */
        ret = lumiera_mrucache_pop (&lumiera_fhcache->cache);
        ret = lumiera_filehandle_init (lumiera_mrucache_pop (&lumiera_fhcache->cache), desc);
        if (lumiera_fhcache->available < 0)
          /* try to free overallocated filehandles */
          lumiera_fhcache->available -= lumiera_fhcache->available + lumiera_mrucache_age (&lumiera_fhcache->cache, -lumiera_fhcache->available);
      }
    else
      {
        /* allocate new filehandle if we are below the limit or no cached handles are available (overallocating) */
        NOTICE (file, "overallocating filehandle");
        ret = lumiera_filehandle_new (desc);
        TODO ("use resourcecollector here");
        if (!ret)
          LUMIERA_ERROR_SET_ALERT (file, FILEHANDLECACHE_NOHANDLE, lumiera_filedescriptor_name (desc));
        else
          --lumiera_fhcache->available;
      }
    desc->handle = ret;
    ++lumiera_fhcache->checked_out;
  }
  return ret;
}


LumieraFilehandle
lumiera_filehandlecache_checkout (LumieraFilehandle handle)
{
  TRACE (filehandlecache_dbg);
  REQUIRE (handle);
  /* This function is called with the associated descriptor locked, nothing can modify 'handle' */
  if (!handle->use_cnt)
    {
      /* lock cache and checkout */
      LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_fhcache->lock)
        {
          lumiera_mrucache_checkout (&lumiera_fhcache->cache, &handle->cachenode);
          ++lumiera_fhcache->checked_out;
        }
    }
  ++handle->use_cnt;

  return handle;
}


void
lumiera_filehandlecache_checkin (LumieraFilehandle handle)
{
  TRACE (filehandlecache_dbg);
  REQUIRE (handle);
  REQUIRE (handle->use_cnt);

  /* This function is called with the associated descriptor locked, nothing can modify the fhcache */
  if (!--handle->use_cnt)
    {
      /* lock cache and checin */
      LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_fhcache->lock)
        {
          --lumiera_fhcache->checked_out;
          lumiera_mrucache_checkin (&lumiera_fhcache->cache, &handle->cachenode);
        }
    }
}

