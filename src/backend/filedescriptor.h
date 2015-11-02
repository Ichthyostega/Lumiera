/*
  FILEDESCRIPTOR.h  -  file handling

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


/** @file filedescriptor.h
 ** File descriptors are the underlying working horse in accessing files.
 ** All information associated with managing a file is kept here.
 */

#ifndef BACKEND_FILEDESCRIPTOR_H
#define BACKEND_FILEDESCRIPTOR_H

#include "lib/mutex.h"
#include "lib/rwlock.h"
#include "lib/psplay.h"
#include "lib/llist.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct lumiera_filedescriptor_struct lumiera_filedescriptor;
typedef lumiera_filedescriptor* LumieraFiledescriptor;


#include "backend/filehandle.h"
#include "backend/file.h"
#include "backend/mmapings.h"


struct lumiera_filedescriptor_struct
{
  /** node for the lookup tree */
  psplaynode node;

  /** create after first open, maintained metadata */
  struct stat stat;

  /**
   * files which are written are rounded up to the next chunk boundary
   * by the mmaping backend and will be ftruncated to the realsize on close.
   */
  off_t realsize;

  /** open flags, must be masked for reopen */
  int flags;

  /** locks operations on this file descriptor */
  lumiera_mutex lock;

  /** Associated posix filehandle */
  LumieraFilehandle handle;

  /** established memory mappings */
  LumieraMMapings mmapings;

  /** list of all attached 'file' structures, that are the names of the files */
  llist files;

  /** file locking, a rwlock for thread locking */
  lumiera_rwlock filelock;

  /** readlock counter for releasing the file lock, -1 for write lock, 0 = unlocked */
  int lock_cnt;
  RESOURCE_USER (filelock_rh);
};


/**
 * Find existing filedescriptor or create one
 * @param name name of the file
 * @param flags opening flags for the filedescriptor
 * @return descriptor on success or NULL on error
 */
LumieraFiledescriptor
lumiera_filedescriptor_acquire (const char* name, int flags, LList filenode);


/**
 * Release a filedescriptor.
 * @param self filedescriptor to be released
 * @param file the file struct which initiated the release
 */
void
lumiera_filedescriptor_release (LumieraFiledescriptor self, const char* name, LList filenode);

int
lumiera_filedescriptor_handle_acquire (LumieraFiledescriptor self);

void
lumiera_filedescriptor_handle_release (LumieraFiledescriptor self);

const char*
lumiera_filedescriptor_name (LumieraFiledescriptor self);

int
lumiera_filedescriptor_flags (LumieraFiledescriptor self);

int
lumiera_filedescriptor_samestat (LumieraFiledescriptor self, struct stat* stat);

/**
 * Allocate a new filedescriptor cloned from a template
 * @param template the source filedescriptor
 * @return the constructed filedescriptor
 */
LumieraFiledescriptor
lumiera_filedescriptor_new (LumieraFiledescriptor template);

/**
 * Delete a filedescriptor
 * Called whenever its reference count drops to zero.
 * @param self the filedescriptor to be deleted
 */
void
lumiera_filedescriptor_delete (LumieraFiledescriptor self, const char* name);



LumieraFiledescriptor
lumiera_filedescriptor_rdlock (LumieraFiledescriptor self);


LumieraFiledescriptor
lumiera_filedescriptor_wrlock (LumieraFiledescriptor self);


LumieraFiledescriptor
lumiera_filedescriptor_unlock (LumieraFiledescriptor self);




#endif /*BACKEND_FILEDESCRIPTOR_H*/
