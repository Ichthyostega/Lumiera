/*
  FILEHEADER.h  -  Definitions of generic lumiera file headers and identification

  Copyright (C)         Lumiera.org
    2010,               Christian Thaeter <ct@pipapo.org>

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


/** @file fileheader.h
 ** Common header format to identify various kinds of files.
 ** Lumiera creates some files on itself, caches, indexes and so on.
 ** Here we define a unified header format for identifying and handling these files.
 **
 ** Most of this files store binary data in host order for performance reasons and are not yet
 ** intended to be transfered between computers. While the transferability depends on the
 ** concrete implementation later and is not constrained here.
 **
 */

#ifndef BACKEND_FILEHEADER_H
#define BACKEND_FILEHEADER_H

#include "lib/error.h"



LUMIERA_ERROR_DECLARE (FILEHEADER_NOWRITE);
LUMIERA_ERROR_DECLARE (FILEHEADER_HEADER);
LUMIERA_ERROR_DECLARE (FILEHEADER_FLAGS);
LUMIERA_ERROR_DECLARE (FILEHEADER_FLAGSPACE);
LUMIERA_ERROR_DECLARE (FILEHEADER_ENDIANESS);

#define LUMIERA_FILEHEADER_ENDIANMAGIC 0x0123456789ABCDEFULL

typedef struct lumiera_fileheader_struct lumiera_fileheader;
typedef lumiera_fileheader* LumieraFileheader;

typedef struct lumiera_fileheader_raw_struct lumiera_fileheader_raw;
typedef lumiera_fileheader_raw* LumieraFileheaderRaw;


#include "vault/file.h"


#include <nobug.h>
#include <semaphore.h>




#ifndef LUMIERA_PACKED
#define LUMIERA_PACKED __attribute__((__packed__))
#endif

/**
 * A basic file header
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
  /** free form string, comment or so on, initialised to spaces */
  char meta[15];
  /** always '\n' */
  char newline2;

  /** Initialised to spaces, flags are single chars, unsorted */
  char flags[6];
  /** always '\n' */
  char newline3;
  /** always '\0' */
  char null;

  /* natively written 0x0123456789ABCDEFULL by the host created this */
  uint64_t endianess_mark;
};

/**
 * File header flags are chars to support debugging and inspection,
 * these add another human readable line to the header.
 */

/** file is clean */
#define LUMIERA_FILEHEADER_FLAG_CLEAN "c"

/** check for host order endianess */
#define LUMIERA_FILEHEADER_FLAG_ENDIANESS "e"





/**
 * A file header object encapsulates the underlying mmap object which keeps the
 * raw header data in memory and and the dereferenced thereof.
 */
struct lumiera_fileheader_struct
{
  LumieraFileheaderRaw  header;
  LumieraMMap map;
};


/**
 * Create a file header on a file open for writing.
 * This overwrites any existing date, take care. The created file header is mmaped into memory
 * and must be closed after use. The File should be locked for operations on the file header.
 * @param file The file on which to create the header.
 * @param fourcc pointer to a string of length 4
 * @param version version number for the header (should be incremented after changes)
 *                the value '0' is reserved for experimental versions.
 * @param size The actual size of all header data, including following format specific data.
 * @param flags initial flags which should be set (don't include CLEAN here, should be set on close)
 * @return A lumiera_fileheader object by value, .header and .map are set to NULL on error.
 */
lumiera_fileheader
lumiera_fileheader_create (LumieraFile file, char* fourcc, int version, size_t size, const char* flags);


/**
 * Open an existing file header.
 * The underlying file might be readonly. The opened file header is mmaped into memory
 * and must be closed after use. The File should be locked for operations on the file header.
 * @param file The file on which to open the header.
 * @param fourcc pointer to a string of length 4 with the expected identifier for the file
 * @param size The actual size of all header data, including following format specific data
 * @param flags_expected expect this flags being set
 * @param flags_remove remove this flags when opening
 * @return A lumiera_fileheader object by value, .header and .map are set to NULL on error.
 */
lumiera_fileheader
lumiera_fileheader_open (LumieraFile file, char* fourcc, size_t size, const char* flags_expected, const char* flags_remove);


/**
 * Closes a previously created or opened file header.
 * @param self the file header to close.
 * @param flags_add set this flags if not already set
 * no errors, no nothing returned (yet)
 */
void
lumiera_fileheader_close (LumieraFileheader self, const char* flags_add);


/**
 * Queries the version of a file header.
 * @param self the file header to query
 * @return the version stored in the file header or -1 on error.
 */
int
lumiera_fileheader_version (LumieraFileheader self);


/**
 * check if all flags given from some sets are either set or not.
 */
int
lumiera_fileheader_flags_validate (LumieraFileheader self, const char* expected, const char* unexpected);


/**
 * Sets flags if not already set
 */
LumieraFileheader
lumiera_fileheader_flags_set (LumieraFileheader self, const char* flags);

/**
 * Clear flags if present
 */
LumieraFileheader
lumiera_fileheader_flags_clear (LumieraFileheader self, const char* flags);


#endif /*BACKEND_FILEHEADER_H*/
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
