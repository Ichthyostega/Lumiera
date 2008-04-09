/*
  filehandle  -  filehandle management and caching

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
#ifndef LUMIERA_FILEHANDLE_H
#define LUMIERA_FILEHANDLE_H

#include "lib/error.h"
#include "lib/llist.h"

typedef struct lumiera_filehandle_struct lumiera_filehandle;
typedef lumiera_filehandle* LumieraFilehandle;

#include "backend/filedescriptor.h"

/**
 * @file Filehandles
 * Filehandles manage the underlying POSIX filehandle for a filedescriptor.
 * Since we want to support handling of more files than POSIX filehandles are available on a common system
 * the filehandles are opened, cached and closed on demand, see 'filehandlecache'.
 */


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
 * Allocate a new filehandle structure.
 * @return new filehandle structure
 */
LumieraFilehandle
lumiera_filehandle_new ();


/**
 * destroy the resources associated eith a filehandle structure.
 * This function is used by the filehandlecache to recycle filehandle structs.
 * @param node pointer to the cachenode member of a struct filehandle
 * @return pointer to the start of the memory of the destroyed filehandle
 */
void*
lumiera_filehandle_destroy_node (LList node);

#endif
