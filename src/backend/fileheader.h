/*
  fileheader.h  -  Definitions of generic lumiera fileheaders and identification

  Copyright (C)         Lumiera.org
    2010,               Christian Thaeter <ct@pipapo.org>

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

#ifndef LUMIERA_FILEHEADER_H
#define LUMIERA_FILEHEADER_H

#include "lib/error.h"

LUMIERA_ERROR_DECLARE (FILEHEADER_NOWRITE);
LUMIERA_ERROR_DECLARE (FILEHEADER_HEADER);


typedef struct lumiera_fileheader_struct lumiera_fileheader;
typedef lumiera_fileheader* LumieraFileheader;

typedef struct lumiera_fileheader_raw_struct lumiera_fileheader_raw;
typedef lumiera_fileheader_raw* LumieraFileheaderRaw;

typedef struct lumiera_fileheaderext_struct lumiera_fileheaderext;
typedef lumiera_fileheaderext* LumieraFileheaderext;

typedef struct lumiera_fileheaderext_raw_struct lumiera_fileheaderext_raw;
typedef lumiera_fileheaderext_raw* LumieraFileheaderextRaw;


#include "backend/file.h"


#include <nobug.h>
#include <semaphore.h>


/**
 * @file
 *
 * Lumiera creates some files on itself, caches, indexes and so on.
 * Here we define a unified header format for identifying and handling these files.
 *
 * Most of this files store binary data in host order for performance reasons an are not yet
 * intended to be transfered between computers. While the transferability depends on the
 * concrete implementation later and is not contstrained here.
 *
 */


#ifndef LUMIERA_PACKED
#define LUMIERA_PACKED __attribute__((__packed__))
#endif

/**
 * A basic fileheader
 * On-Disk representation starts with 32 bytes identifying the file.
 * The first 32 bytes are human readable text.
 */
struct LUMIERA_PACKED lumiera_fileheader_raw_struct
{
  /** four character codes identifying this file type */
  char fourcc[4];
  /** decimal digits, right bound space filled, denoting the file version, 0 is reserved for experimental things */
  char version[3];
  /** always '\n' */
  char newline1;
  /** freeform string, comment or so on, initialized to spaces */
  char meta[22];
  /** always '\n' */
  char newline2;
  /** always '\0' */
  char null;
};

/**
 * A fileheader object encapsulates the underlying mmap object which keeps the
 * raw header data in memory and and the dereferenced thereof.
 */
struct lumiera_fileheader_struct
{
  LumieraFileheaderRaw  header;
  LumieraMMap map;
};


/**
 * Create a fileheader on a file open for writing.
 * This overwrites any existing date, take care. The created fileheader is mmaped into memory
 * and must be closed after use. The File should be locked for operations on the fileheader.
 * @param file The file on which to create the header.
 * @param fourcc pointer to a string of length 4
 * @param version version number for the header (should be incremented after changes)
 *                the value '0' is reserved for experimental versions.
 * @param size The actual size of all header data, including following format specific data.
 * @return A lumiera_fileheader object by value, .header and .map are set to NULL on error.
 */
lumiera_fileheader
lumiera_fileheader_create (LumieraFile file, char* fourcc, int version, size_t size);


/**
 * Open an existing fileheader.
 * The underlying file might be readonly. The opened fileheader is mmaped into memory
 * and must be closed after use. The File should be locked for operations on the fileheader.
 * @param file The file on which to open the header.
 * @param fourcc pointer to a string of length 4 with the expected identifier for the file
 * @param size The actual size of all header data, including following format specific data.
 * @return A lumiera_fileheader object by value, .header and .map are set to NULL on error.
 */
lumiera_fileheader
lumiera_fileheader_open (LumieraFile file, char* fourcc, size_t size);


/**
 * Closes a previously created or opened fileheader.
 * @param self the fileheader to close.
 * no errors, no nothing returned
 */
void
lumiera_fileheader_close (LumieraFileheader self);


/**
 * Queries the version of a fileheader.
 * @param self the fileheader to query
 * @return the version stored in the fileheader or -1 on error.
 */
int
lumiera_fileheader_version (LumieraFileheader self);





#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
