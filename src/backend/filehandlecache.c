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

#include "backend/filehandlecache.h"

NOBUG_DEFINE_FLAG (lumiera_filehandlecache);

/* errors */

LUMIERA_ERROR_DEFINE (FILEHANDLECACHE_NOHANDLE, "No filehandle available");


/**/

LumieraFilehandlecache
lumiera_filehandlecache_init (LumieraFilehandlecache self, int max_entries)
{
  lumiera_mrucache_init (&self->cache, lumiera_filehandle_destroy_node);
  self->available =max_entries;
  lumiera_mutex_init (&self->lock);
  return self;
}

LumieraFilehandlecache
lumiera_filehandlecache_destroy (LumieraFilehandlecache self)
{
  self->available += self->cache.cached;
  lumiera_mrucache_destroy (&self->cache);
  lumiera_mutex_destroy (&self->lock);
  return self;
}


LumieraFilehandle
lumiera_filehandlecache_filehandle_acquire (LumieraFilehandlecache self)
{
  LumieraFilehandle ret;
  LUMIERA_MUTEX_SECTION (&self->lock)
  {
    if (self->available <= 0 && self->cache.cached)
      {
        /* pop a filehandle from cache */
        ret = lumiera_mrucache_pop (&self->cache);
        if (self->available < 0)
          /* try to free overallocated filehandles */
          self->available -= self->available + lumiera_mrucache_age (&self->cache, -self->available);
      }
    else
      {
        /* allocate new filehandle if we are below the limit or no cached handles where available (overallocating) */
        ret = lumiera_filehandle_new ();
        if (!ret)
          LUMIERA_ERROR_SET (lumiera_filehandlecache, FILEHANDLECACHE_NOHANDLE);
        else
          --self->available;
      }
  }
  return ret;
}

void
lumiera_filehandlecache_add (LumieraFilehandlecache self, LumieraFilehandle handle)
{
  UNIMPLEMENTED ("");
}
