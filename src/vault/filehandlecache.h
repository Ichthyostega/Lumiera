/*
  FILEHANDLECACHE  -  filehandle management and caching

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


/** @file filehandlecache.h
 ** Caching and management of filehandles.
 ** The number of filehandles a program can held open is usually limited, since we want to support
 ** using a less limited number of files and closing/opening for each operation is expensive, we
 ** provide a cache to keep the most frequent used files open and gracefully close/recycle unused filehandles.
 ** The filehandle cache defined here protects all operations on the cache with a mutex.
 */

#ifndef BACKEND_FILEHANDLECACHE_H
#define BACKEND_FILEHANDLECACHE_H

#include "lib/error.h"
#include "lib/mrucache.h"
#include "lib/mutex.h"

typedef struct lumiera_filehandlecache_struct lumiera_filehandlecache;
typedef lumiera_filehandlecache* LumieraFilehandlecache;

#include "vault/filehandle.h"



/**  File handle cache manages file handles */
struct lumiera_filehandlecache_struct
{
  lumiera_mrucache cache;
  int available;
  int checked_out;
  lumiera_mutex lock;
};

/**
 * Initialises the filehandle cache.
 * @param max_entries number how much filehandles shall be managed
 * The number of elements the cache can hold is static and should be
 * determined by sysconf (_SC_OPEN_MAX) minus some (big) safety margin.
 */
void
lumiera_filehandlecache_new (int max_entries);

/**
 * Delete the filehandle cache.
 * No filehandles in the cache must be locked, this would be a fatal error.
 * The handles are closed automatically.
 */
void
lumiera_filehandlecache_delete (void);

/**
 * Get a fresh filehandle.
 * @param self pointer to the cache
 * @return the new filehandle
 */
LumieraFilehandle
lumiera_filehandlecache_handle_acquire (LumieraFiledescriptor desc);

/**
 * Remove a filehandle from cache aging
 * Filehandles which are subject of cache aging must be checked out before they can be used.
 * @param self the filehandlecache
 * @param handle the filehandle to be checked out
 */
LumieraFilehandle
lumiera_filehandlecache_checkout (LumieraFilehandle handle);

/**
 * Put a filehandle into the cache
 * Filehandles which are checked in are subject of cache aging and might get destroyed and reused.
 * @param handle the filehandle to be checked in
 */
void
lumiera_filehandlecache_checkin (LumieraFilehandle handle);



#endif /*BACKEND_FILEHANDLECACHE_H*/
