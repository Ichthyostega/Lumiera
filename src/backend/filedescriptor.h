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


struct lumiera_filedescriptor_struct
{
  struct stat stat;                     /* create after first open, maintained metadata, MUST BE FIRST! */
  int flags;                            /* open flags, must be masked for reopen */
  lumiera_mutex lock;                   /* locks operations on this file descriptor */
  unsigned reopened;                    /* count for reopens 0=not yet opened, 1=first, 2..=reopened */
  unsigned refcount;                    /* reference counter, all users sans registry */

  //LumieraFileMap mappings;
  //LumieraWriteBuffer writebuffer;
};


LumieraFiledescriptor
lumiera_filedescriptor_acquire (const char* name, int flags);

void
lumiera_filedescriptor_release (LumieraFiledescriptor self);

LumieraFiledescriptor
lumiera_filedescriptor_new (LumieraFiledescriptor template);

void
lumiera_filedescriptor_delete (LumieraFiledescriptor self);


#endif
