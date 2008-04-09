/*
  filedescriptor.h  -  file handling

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

#ifndef LUMIERA_FILEDESCRIPTOR_H
#define LUMIERA_FILEDESCRIPTOR_H

#include "lib/mutex.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct lumiera_filedescriptor_struct lumiera_filedescriptor;
typedef lumiera_filedescriptor* LumieraFiledescriptor;


#include "backend/filehandle.h"
#include "backend/file.h"

/**
 * @file Filedescriptors
 * Filedescriptors are the underlying working horse in accessing files.
 */



struct lumiera_filedescriptor_struct
{
  struct stat stat;                     /* create after first open, maintained metadata, MUST BE FIRST! */
  int flags;                            /* open flags, must be masked for reopen */
  lumiera_mutex lock;                   /* locks operations on this file descriptor */
  unsigned reopened;                    /* count for reopens 0=not yet opened, 1=first, 2..=reopened */
  unsigned refcount;                    /* reference counter, all users sans registry */

  LumieraFilehandle handle;
  //LumieraFileMap mappings;
  //LumieraWriteBuffer writebuffer;

  RESOURCE_HANDLE (rh);
};

/**
 * Initialize the global filedescriptor registry.
 * All filedescriptors are looked up by dev/ino/open-flags in a hashtable. Opening hardlinked
 * files will be targeted to the same filedescriptor.
 * This function never fails but dies on error.
 */
void
lumiera_filedescriptor_registry_init (void);

/**
 * Destroy and free the global filedescriptor registry.
 * Never fails.
 */
void
lumiera_filedescriptor_registry_destroy (void);


/**
 * Find existing filedescriptor or create one
 * @param name name of the file
 * @param flags opening flags for the filedescriptor
 * @return descriptor on success or NULL on error
 */
LumieraFiledescriptor
lumiera_filedescriptor_acquire (const char* name, int flags);


/**
 * Release a filedescriptor.
 * @param self filedescriptor to be released
 */
void
lumiera_filedescriptor_release (LumieraFiledescriptor self);


/**
 * Allocate a new filedescriptor cloned from a template
 * @param template the source filedescriptor
 * @return the constrccted filedescriptor
 */
LumieraFiledescriptor
lumiera_filedescriptor_new (LumieraFiledescriptor template);

/**
 * Delete a filedescriptor
 * Called whenever its reference count drops to zero.
 * @param self the filedescriptor to be deleted
 */
void
lumiera_filedescriptor_delete (LumieraFiledescriptor self);

#endif
