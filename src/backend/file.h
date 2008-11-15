/*
  file.h  -  interface to files by filename

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

#ifndef LUMIERA_FILE_H
#define LUMIERA_FILE_H


#include "lib/mutex.h"
#include "lib/llist.h"
#include "lib/error.h"

NOBUG_DECLARE_FLAG (file);

LUMIERA_ERROR_DECLARE(FILE_CHANGED);
LUMIERA_ERROR_DECLARE(FILE_NOCHUNKSIZE);

/**
 * @file
 * File management
 * Handling Files is splitted into different classes:
 * 1. The 'lumiera_file' class which acts as interface to the outside for managing files.
 *    'lumiera_file' is addressed by the name of the file. Since files can have more than one name (hardlinks)
 *    many 'lumiera_file' can point to a single 'lumiera_filedescriptor'
 * 2. The 'lumiera_filedescriptor' class which does the real work managing the file in the back.
 * 3. Since OS-filehandles are a limited resource we access the lazily as 'lumiera_filehandle' which are
 *    managed in a 'lumiera_filehandlecache'
 */

typedef struct lumiera_file_struct lumiera_file;
typedef lumiera_file* LumieraFile;


#include "backend/filehandle.h"
#include "backend/mmapings.h"

/**
 * File modes:
 * LUMIERA_FILE_READONLY        existing file for reading only
 * LUMIERA_FILE_READWRITE       existing file for reading and writing
 * LUMIERA_FILE_CREATE          non-existing file for reading and writing
 * LUMIERA_FILE_RECREATE        remove and recreated existing, file for reading and writing
 */
#define LUMIERA_FILE_READONLY (O_RDONLY | O_LARGEFILE | O_NOATIME)
#define LUMIERA_FILE_READWRITE (O_RDWR | O_LARGEFILE | O_NOATIME)
#define LUMIERA_FILE_CREATE (O_RDWR | O_LARGEFILE | O_NOATIME | O_CREAT | O_EXCL)
#define LUMIERA_FILE_RECREATE (O_RDWR | O_LARGEFILE | O_NOATIME | O_CREAT | O_TRUNC)

/* creat and excl flags will be masked out for descriptor lookup */
#define LUMIERA_FILE_MASK ~(O_CREAT | O_EXCL | O_TRUNC)

struct lumiera_file_struct
{
  /* all files of one descriptor */
  llist node;
  char* name;
  LumieraFiledescriptor descriptor;
};


/**
 * Initialize a file structure.
 * @param self pointer to the file structure
 * @param name filename
 * @param flags open flags
 * @return self
 */
LumieraFile
lumiera_file_init (LumieraFile self, const char* name, int flags);


/**
 * Destroy a file structure.
 * frees all associated resources, releases the filedescriptor etc.
 * @param self file structure to be destroyed
 * @param chunksize allocation/mmaping granularity, must be 2's exponent of pagesize
 *        only used at the first access to a file and ignored for subsequnet accesses
 * @return self
 */
LumieraFile
lumiera_file_destroy (LumieraFile self);


/**
 * Allocate a new file structure.
 * @param name filename
 * @param flags open flags
 * @return new file structure
 */
LumieraFile
lumiera_file_new (const char* name, int flags);


/**
 * Frees a file structure.
 * @param self file structure to be freed
 */
void
lumiera_file_delete (LumieraFile self);


/**
 * Get a POSIX filehandle for a file.
 * Filehandles are opened on demand and must be acquired for use.
 * Using filehandles is refcounted and might be nested.
 * After using them they must be released which puts them back into filehandlecache aging.
 * @param self file structure
 * @return POSIX filehandle or -1 on error, check lumiera_error() to retrieve the errorcode
 * Currently only LUMIERA_ERROR_ERRNO will be raised but this might change in future.
 * Opening files can fail for many reasons and at any time!
 */
int
lumiera_file_handle_acquire (LumieraFile self);


/**
 * Put filehandle back into cache aging.
 * @param self file which handle to be released
 */
void
lumiera_file_handle_release (LumieraFile self);


/**
 * Query the underlying mmapings object from a file
 * The MMapings only exists after a chunksize got set with lumiera_file_chunksize_set()
 * @param self the file to query
 * @return Handle to the MMapings object or NULL on error (setting the error state)
 */
LumieraMMapings
lumiera_file_mmapings (LumieraFile self);


/**
 * Set the chunksize for mapping operations
 * @param chunksize allocation/mmaping granularity, must be 2's exponent of pagesize
 *        only used at the first access to a file and ignored for subsequnet accesses
 */
size_t
lumiera_file_chunksize_set (LumieraFile self, size_t chunksize);

#endif

