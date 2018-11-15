/*
  MMAP  -  memory mapped access to files

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


/** @file mmap.h
 ** MMap objects cover a memory mapped range in a file.
 ** They are managed through a global mmap registry/cache.
 */

#ifndef BACKEND_MMAP_H
#define BACKEND_MMAP_H

#include "lib/llist.h"


typedef struct lumiera_mmap_struct lumiera_mmap;
typedef lumiera_mmap* LumieraMMap;


#include "backend/file.h"
#include "backend/filedescriptor.h"


#include <nobug.h>
#include <sys/mman.h>






/**
 * Descriptor of a memory mapped area
 */
struct lumiera_mmap_struct
{
  /** used for the mrucache when checked in the cache **/
  llist cachenode;

  /** all mmaps regarding a file are chained in this list, used to find ranges **/
  llist searchnode;

  off_t start;
  size_t size;
  void* address;

  /** accumulated references, this is 0 when checked into the cache **/
  long refcnt;

  /** array with a refcounter per chunk **/
  short* refmap;       ///////////////////// TODO make this a flexible array?
};


/**
 * Initialise a MMap object.
 * The mmap objects are aligned and shifted by the chunksize and bias defined for the file
 * @param self the mmap object to be initialised
 * @param file file from which to map
 * @param start offset in file which must be part of the mmaped region
 * @param size minimum size after start to map
 * @return self on success or NULL on error
 */
LumieraMMap
lumiera_mmap_init (LumieraMMap self, LumieraFile file, off_t start, size_t size);

/**
 * Initialise a MMap object.
 * Maps exactly the given range
 * @param self the mmap object to be initialised
 * @param file file from which to map
 * @param start offset in file which must be part of the mmaped region
 * @param size minimum size after start to map
 * @return self on success or NULL on error
 */
LumieraMMap
lumiera_mmap_init_exact (LumieraMMap self, LumieraFile file, off_t start, size_t size);


LumieraMMap
lumiera_mmap_new (LumieraFile file, off_t start, size_t size);

LumieraMMap
lumiera_mmap_new_exact (LumieraFile file, off_t start, size_t size);


/**
 * Translate a 'external' offset to a address in memory
 * @param self MMap object to query
 * @param offset position on the mmaped file to get
 * @return address in memory which relates to offset
 */
static inline void*
lumiera_mmap_address (LumieraMMap self, off_t offset)
{
  REQUIRE_IF (self, offset >= self->start, "offset before mmaped region");
  REQUIRE_IF (self, offset < self->start + (off_t)self->size, "offset after mmaped region");
  return self?(self->address + (offset - self->start)):NULL;
}

void
lumiera_mmap_delete (LumieraMMap self);


void*
lumiera_mmap_destroy_node (LList node);


#endif /*BACKEND_MMAP_H*/
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
