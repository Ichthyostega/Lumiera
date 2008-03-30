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
#ifndef LUMIERA_FILEHANDLECACHE_H
#define LUMIERA_FILEHANDLECACHE_H

#include "lib/error.h"
#include "lib/mrucache.h"
#include "lib/mutex.h"

#include "backend/filehandle.h"

/**
 * @file Filehandle management and caching
 * The number of filehandles a program can held open is usually limited, since we want to support
 * using a less limited number of files and closing/opening for each operation is expensive, we
 * provide a cache to keep the most frequent used files open and gracefully close/recycle unused filehandles.
 */



/*
  File handle cache manages file handles
 */
struct lumiera_filehandlecache_struct
{
  lumiera_mrucache cache;
  int available;
  lumiera_mutex lock;
};
typedef struct lumiera_filehandlecache_struct lumiera_filehandlecache;
typedef lumiera_filehandlecache* LumieraFilehandlecache;


/**
 * Initializes a filehandle cache.
 * @param self pointer to the cache to be initialized
 * @param max_entries number how much filehandles shall be managed
 * @return self as given
 * The number of elements the cache can hold is static and should be
 * determined by sysconf (_SC_OPEN_MAX) minus some safety margin.
 */
LumieraFilehandlecache
lumiera_filehandlecache_init (LumieraFilehandlecache self, int max_entries);

/**
 * Destroy a filehandle cache.
 * @param self the cache to be destroyed
 * @return self as given to the now uninitialized! cache
 * No filehandles in the cache must be locked, this would be a fatal error.
 * The handles are closed automatically.
 */
LumieraFilehandlecache
lumiera_filehandlecache_destroy (LumieraFilehandlecache self);

/**
 * Get a fresh filehandle.
 * @param self pointer to the cache
 * @return the new filehandle
 * one must call lumiera_mutexacquirer_unlock (&lock) to free the lock on the handle when done
 */
LumieraFilehandle
lumiera_filehandlecache_filehandle_acquire (LumieraFilehandlecache self);

/**
 * Add filehande back to cache.
 * @param self pointer to the cache
 * @param handle filehandle to be put back
 */
void
lumiera_filehandlecache_add (LumieraFilehandlecache self, LumieraFilehandle handle);

/**
 * 
 */
LumieraFilehandle
lumiera_filehandlecache_remove (LumieraFilehandlecache self, int fd);


#endif
