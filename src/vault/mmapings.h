/*
  MMAPINGS.h  -  manage ranges of mmaped areas on a file descriptor

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


/** @file mmapings.h
 ** Manage the mmap objects of a file.
 */

#ifndef VAULT_MMAPINGS_H
#define VAULT_MMAPINGS_H

#include "lib/mutex.h"
#include "lib/llist.h"

typedef struct lumiera_mmapings_struct lumiera_mmapings;
typedef lumiera_mmapings* LumieraMMapings;

#include "vault/filedescriptor.h"
#include "vault/mmap.h"
#include "vault/file.h"

#include <nobug.h>




struct lumiera_mmapings_struct
{
  /** mmaped ranges are kept in an list sorted by the size of the mmaping, might be improved to a tree someday **/
  llist mmaps;

  /**
   * chunksize is the smallest granularity which is used for mmapping files, it
   * should reflect the intended file usage, that is 'pagesize' for small or non growing
   * files and some MB for media files. Must be a 2's exponent of pagesize.
   **/
  size_t chunksize;

  /**
   * bias shifts the chunk begin to suppress headers for example
   **/
  size_t bias;

  LumieraFiledescriptor descriptor;
  lumiera_mutex lock;
};



/** initialise mmapings container */
LumieraMMapings
lumiera_mmapings_init (LumieraMMapings self, LumieraFile file, size_t chunksize, size_t bias);


/** destroy mmapings container and free all resources. */
LumieraMMapings
lumiera_mmapings_destroy (LumieraMMapings self);


/** allocate and initialise new mmapings container */
LumieraMMapings
lumiera_mmapings_new (LumieraFile file, size_t chunksize, size_t bias);


/** destroy and free mmapings container and all its resources */
void
lumiera_mmapings_delete (LumieraMMapings self);


/**
 * acquire a mmap which covers the given range
 * @param self mmapings where to search
 * @param start begin of the required range
 * @param size requested size
 * @return MMap object covering the requested range or NULL on error
 */
LumieraMMap
lumiera_mmapings_mmap_acquire (LumieraMMapings self, LumieraFile file, off_t start, size_t size);


/**
 * release a previously acquired MMap object
 * @param self mmapings to which the map belongs
 * @param map object to be released
 */
void
lumiera_mmapings_release_mmap (LumieraMMapings self, LumieraMMap map);




#endif /*VAULT_MMAPINGS_H*/
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
