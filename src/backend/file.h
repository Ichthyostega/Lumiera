/*
  FILE.h  -  interface to files by filename

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


/** @file file.h
 ** File management.
 ** Handling Files is split into different classes:
 ** 1. The 'lumiera_file' class which acts as interface to the outside for managing files.
 **    'lumiera_file' is addressed by the name of the file. Since files can have more than one name (hardlinks)
 **    many 'lumiera_file' can point to a single 'lumiera_filedescriptor'
 ** 2. The 'lumiera_filedescriptor' class which does the real work managing the file in the back.
 ** 3. Since OS-filehandles are a limited resource we access the lazily as 'lumiera_filehandle' which are
 **    managed in a 'lumiera_filehandlecache'
 */

#ifndef BACKEND_FILE_H
#define BACKEND_FILE_H


#include "lib/mutex.h"
#include "lib/llist.h"
#include "lib/error.h"


LUMIERA_ERROR_DECLARE(FILE_CHANGED);
LUMIERA_ERROR_DECLARE(FILE_NOMMAPINGS);


typedef struct lumiera_file_struct lumiera_file;
typedef lumiera_file* LumieraFile;


#include "backend/filedescriptor.h"
#include "backend/filehandle.h"
#include "backend/mmapings.h"
#include "backend/mmap.h"

/**
 * File modes:
 * - \c LUMIERA_FILE_READONLY        existing file for reading only
 * - \c LUMIERA_FILE_READWRITE       existing file for reading and writing
 * - \c LUMIERA_FILE_CREATE          non-existing file for reading and writing
 * - \c LUMIERA_FILE_RECREATE        remove and recreated existing, file for reading and writing
 */
#define LUMIERA_FILE_READONLY (O_RDONLY | O_LARGEFILE | O_NOATIME)
#define LUMIERA_FILE_READWRITE (O_RDWR | O_LARGEFILE | O_NOATIME)
#define LUMIERA_FILE_CREATE (O_RDWR | O_LARGEFILE | O_NOATIME | O_CREAT | O_EXCL)
#define LUMIERA_FILE_RECREATE (O_RDWR | O_LARGEFILE | O_NOATIME | O_CREAT | O_TRUNC)

/* \c creat and \c excl flags will be masked out for descriptor lookup */
#define LUMIERA_FILE_MASK ~(O_CREAT | O_EXCL | O_TRUNC)

struct lumiera_file_struct
{
  /* all files of one descriptor */
  llist node;
  char* name;
  LumieraFiledescriptor descriptor;
};


/**
 * Initialise a file structure.
 * @param self pointer to the file structure
 * @param name filename
 * @param flags open flags
 * @return self or NULL in case of an error
 */
LumieraFile
lumiera_file_init (LumieraFile self, const char* name, int flags);


/**
 * Destroy a file structure.
 * frees all associated resources, releases the file descriptor etc.
 * @param self file structure to be destroyed
 * @param do_unlink if 1 then delete the file physically from disk (only the associated name)
 * @return self
 */
LumieraFile
lumiera_file_destroy (LumieraFile self, int do_unlink);


/**
 * Allocate a new file structure.
 * @param name filename
 * @param flags open flags
 * @return new file structure or NULL in case of an error
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
 * Frees a file structure and deletes the associated file name from disk.
 * @param self file structure to be freed
 */
void
lumiera_file_delete_unlink (LumieraFile self);


/**
 * Get a POSIX filehandle for a file.
 * Filehandles are opened on demand and must be acquired for use.
 * The use of filehandles is refcounted and might be nested.
 * After using them they must be released which puts them back into filehandle cache aging.
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
 * acquire a mmap which covers the given range
 * @param self file from where the mmap shall be acquired
 * @param start begin of the required range
 * @param size requested size
 * @return MMap object covering the requested range or NULL on error
 * note: the chunksize for the file must be set prior accessing mmaps
 */
LumieraMMap
lumiera_file_mmap_acquire (LumieraFile self, off_t start, size_t size);


/**
 * release a previously acquired MMap object
 * @param self file to which the map belongs
 * @param map object to be released
 */
void
lumiera_file_release_mmap (LumieraFile self, LumieraMMap map);


/**
 * helper macro for acquiring and releasing maped regions
 * @param nobugflag unused for now
 * @param file the file from from where to acquire the mapped region
 * @param start the start offset for the mmaped region
 * @param size the length of the requested block
 * @param addr name of a void* variable pointing to the requested memory
 */
#define LUMIERA_FILE_MMAP_SECTION(nobugflag, file, start, size, addr)   \
  for (LumieraMMap map_##__LINE__ =                                     \
         lumiera_file_mmap_acquire (file, start, size);                 \
       map_##__LINE__;                                                  \
       ({                                                               \
         lumiera_file_release_mmap (file, map_##__LINE__);              \
         map_##__LINE__ = NULL;                                         \
       }))                                                              \
    for (void* addr = lumiera_mmap_address (map_##__LINE__, start);     \
         addr;                                                          \
         addr = NULL)


/**
 * Query the underlying mmapings object from a file
 * The MMapings only exists after a chunksize got set with lumiera_file_chunksize_set()
 * @param self the file to query
 * @return Handle to the MMapings object or NULL on error (setting the error state)
 */
LumieraMMapings
lumiera_file_mmapings (LumieraFile self);


/**
 * Query the flags effective for a file
 * @param self the file to query
 * @return flags
 */
int
lumiera_file_checkflags (LumieraFile self, int flags);


/**
 * Set the chunksize for mapping operations
 * can only set once for a file descriptor, subsequent calls are no-ops
 * @param chunksize allocation/mmaping granularity, must be 2's exponent of page size
 * @param bias offset to shift chunks, used for stepping over a header for example.
 * @return the effective chunksize used for the file
 */
size_t
lumiera_file_set_chunksize_bias (LumieraFile self, size_t chunksize, size_t bias);


/**
 * Get the chunksize for mapping operations
 * @return the effective chunksize used for the file
 */
size_t
lumiera_file_chunksize_get (LumieraFile self);

/**
 * Get the bias for mapping operations
 * @return the effective bias used for the file
 */
size_t
lumiera_file_bias_get (LumieraFile self);




/**
 * Place and remove locks on a file.
 * This locks are per thread and lock the file across multiple lumiera processes
 * (or any other program which respect to advisory file locking).
 * Only exclusive locks over the whole file are supported for initially accessing
 * a file, other locking is done somewhere else.
 */

LumieraFile
lumiera_file_rdlock (LumieraFile self);

LumieraFile
lumiera_file_wrlock (LumieraFile self);


LumieraFile
lumiera_file_unlock (LumieraFile self);


#define LUMIERA_FILE_RDLOCK_SECTION(nobugflag, file)    \
  for (LumieraFile filelock_##__LINE__ =                \
         lumiera_file_rdlock (file);                    \
       filelock_##__LINE__;                             \
       ({                                               \
         lumiera_file_unlock (filelock_##__LINE__);     \
         filelock_##__LINE__ = NULL;                    \
       }))

#define LUMIERA_FILE_WRLOCK_SECTION(nobugflag, file)    \
  for (LumieraFile filelock_##__LINE__ =                \
         lumiera_file_wrlock (file);                    \
       filelock_##__LINE__;                             \
       ({                                               \
         lumiera_file_unlock (filelock_##__LINE__);     \
         filelock_##__LINE__ = NULL;                    \
       }))





#endif /*BACKEND_FILE_H*/
