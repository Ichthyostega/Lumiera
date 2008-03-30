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
#include "lib/references.h"

/**
 * @file File management
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
#include "backend/filedescriptor.h"



NOBUG_DECLARE_FLAG (lumiera_file);

#define LUMIERA_FILE_READONLY (O_RDONLY | O_LARGEFILE | O_NOATIME)
#define LUMIERA_FILE_READWRITE (O_RDWR | O_LARGEFILE | O_NOATIME)
#define LUMIERA_FILE_CREATE (O_RDWR | O_LARGEFILE | O_NOATIME | O_CREAT | O_EXCL)
#define LUMIERA_FILE_RECREATE (O_RDWR | O_LARGEFILE | O_NOATIME | O_CREAT | O_TRUNC)

struct lumiera_file_struct
{
  const char* name;
  struct lumiera_reference_struct descriptor;
};

/**
 * 
 */
LumieraFile
lumiera_file_init (LumieraFile self, const char* name, int flags);

/**
 * 
 */
LumieraFile
lumiera_file_destroy (LumieraFile self);

/**
 * 
 */
LumieraFile
lumiera_file_new (const char* name, int flags);

/**
 * 
 */
void
lumiera_file_delete (LumieraFile self);


#endif

