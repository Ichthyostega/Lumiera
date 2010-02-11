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

//NOBUG_DECLARE_FLAG (file);

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


#include "backend/filedescriptor.h"
#include "backend/filehandle.h"
#include "backend/mmapings.h"
#include "backend/mmap.h"

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
 * @return self or NULL in case of an error
 */
LumieraFile
lumiera_file_init (LumieraFile self, const char* name, int flags);


/**
 * Destroy a file structure.
 * frees all associated resources, releases the filedescriptor etc.
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
 * acquire a mmap which covers the given range
 * @param self file from where the mmap shall be acquired
 * @param acquirer list node of the new owner which will registered in the mmap
 * @param start begin of the required range
 * @param size requested size
 * @return MMap object covering the requested range or NULL on error
 * note: the chunksize for the file must be set prior accessing mmaps
 */
LumieraMMap
lumiera_file_mmap_acquire (LumieraFile self, LList acquirer, off_t start, size_t size);


/**
 * release a previously acquired MMap object
 * @param self file to which the map belongs
 * @param acquirer holding node, used on require
 * @param map object to be released
 */
void
lumiera_file_release_mmap (LumieraFile self, LList acquirer, LumieraMMap map);


/**
 * helper macro for acquireing and releasing maped regions
 * @param file the file from from where to acquire the mapped region
 * @param start the start offset for the mmaped region
 * @param size the length of the requested block
 * @param addr name of a void* variable pointing to the requested memory
 */
#define LUMIERA_FILE_MMAP_SECTION(file, start, size, addr)                              \
  for (LLIST_AUTO(user_##__LINE__); user_##__LINE__.next; user_##__LINE__.next = NULL)  \
    for (LumieraMMap map_##__LINE__ =                                                   \
           lumiera_file_mmap_acquire (file, &user_##__LINE__, start, size);             \
         map_##__LINE__;                                                                \
         ({                                                                             \
           lumiera_file_release_mmap (file, &user_##__LINE__, map_##__LINE__);          \
           map_##__LINE__ = NULL;                                                       \
         }))                                                                            \
      for (void* addr = lumiera_mmap_address (map_##__LINE__, start);                   \
           addr;                                                                        \
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
 * Set the chunksize for mapping operations
 * @param chunksize allocation/mmaping granularity, must be 2's exponent of pagesize
 *        only used at the first access to a file and ignored for subsequent accesses
 * @return the effective chunksize used for the file
 */
size_t
lumiera_file_chunksize_set (LumieraFile self, size_t chunksize);


/**
 * Get the chunksize for mapping operations
 * @return the effective chunksize used for the file
 */
size_t
lumiera_file_chunksize_get (LumieraFile self);

#endif

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
