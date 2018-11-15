/*
  FILEHANDLE  -  filehandle management and caching

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


/** @file filehandle.h
 ** Filehandles manage the underlying POSIX filehandle for a file descriptor.
 ** Since we want to support handling of more files than POSIX filehandles are available
 ** on a common system the filehandles are opened, cached and closed on demand, see 'filehandlecache'.
 ** Access to filehandles is locked from elsewhere (filedescriptor, filehandlecache)
 */

#ifndef BACKEND_FILEHANDLE_H
#define BACKEND_FILEHANDLE_H

#include "lib/error.h"
#include "lib/llist.h"

typedef struct lumiera_filehandle_struct lumiera_filehandle;
typedef lumiera_filehandle* LumieraFilehandle;

#include "backend/filedescriptor.h"

//NOBUG_DECLARE_FLAG (filehandle);



/**
 * File handles
 */
struct lumiera_filehandle_struct
{
  llist cachenode;
  int fd;
  unsigned use_cnt;
  LumieraFiledescriptor descriptor;
};


/**
 * Initialise filehandle structure.
 * @param self filehandle structure to be initialised
 * @param descriptor on which this filehandle will be attached
 * @return new filehandle structure
 */
LumieraFilehandle
lumiera_filehandle_init (LumieraFilehandle self, LumieraFiledescriptor descriptor);


/**
 * Allocate a new filehandle structure.
 * @param descriptor on which this filehandle will be attached
 * @return new filehandle structure
 */
LumieraFilehandle
lumiera_filehandle_new (LumieraFiledescriptor descriptor);


/**
 * destroy the resources associated either a filehandle structure.
 * This function is used by the filehandle cache to recycle filehandle structs.
 * @param node pointer to the cache node member of a struct filehandle
 * @return pointer to the start of the memory of the destroyed filehandle
 */
void*
lumiera_filehandle_destroy_node (LList node);


int
lumiera_filehandle_handle (LumieraFilehandle self);


/**
 * just accessor, no safety net
 */
static inline int
lumiera_filehandle_get (LumieraFilehandle self)
{
  REQUIRE (self->descriptor);
  return self->fd;
}

#endif /*BACKEND_FILEHANDLE_H*/
