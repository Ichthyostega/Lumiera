/*
  mmapings.h  -  manage ranges of mmaped areas on a filedescriptor

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

#ifndef LUMIERA_MMAPINGS_H
#define LUMIERA_MMAPINGS_H

#include "lib/mutex.h"
#include "lib/llist.h"

typedef struct lumiera_mmapings_struct lumiera_mmapings;
typedef lumiera_mmapings* LumieraMMapings;

#include "backend/filedescriptor.h"
#include "backend/mmap.h"

#include <nobug.h>


/**
 * @file
 * Keeps all mmaped areas
 *
 */

struct lumiera_mmapings_struct
{
  /** mmaped ranges are kept in an list sorted by the size of the mmaping, might be improved to a tree someday **/
  llist mmaps;

  /** sum of all mmaped areas, areas might be overlapping **/
  //  size_t vsz;

  /**
   * chunkssize is the smallest granularity which is used for mmapping files, it
   * should reflect the intended file usage, that is 'pagesize' for small or non growing
   * files and some MB for media files. Must be a 2's exponent of pagesize.
   **/
  size_t chunksize;

  LumieraFiledescriptor descriptor;
  lumiera_mutex lock;
  RESOURCE_HANDLE (rh);
};

/**
 * initialize mmapings container
 *
 */
LumieraMMapings
lumiera_mmapings_init (LumieraMMapings self, LumieraFile file, size_t chunksize);

/**
 * destroy mmapings container and free all resources.
 *
 */
LumieraMMapings
lumiera_mmapings_destroy (LumieraMMapings self);

/**
 * allocate and initialize new mmapings container
 *
 */
LumieraMMapings
lumiera_mmapings_new (LumieraFile file, size_t chunksize);

/**
 * destroy and free mmapings container and all its resources
 *
 */
void
lumiera_mmapings_delete (LumieraMMapings self);

/**
 * acquire a mmap which covers the given range
 * @param self mmapings where to search
 * @param acquirer list node of the new owner which will registered in the mmap
 * @param start begin of the required range
 * @param size requested size
 * @return MMap object covering the requested range or NULL on error
 */
LumieraMMap
lumiera_mmapings_mmap_acquire (LumieraMMapings self, LumieraFile file, LList acquirer, off_t start, size_t size);

/**
 * release a previously acquired MMap object
 * @param self mmapings to which the map belongs
 * @param acquirer holding node
 * @param map object to be released
 */
void
lumiera_mmapings_release_mmap (LumieraMMapings self, LList acquirer, LumieraMMap map);

#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
