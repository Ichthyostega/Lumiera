/*
  safe_clib.h  -  Portable and safe wrapers around some clib functions and some tools

  Copyright (C)         CinelerraCV
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

#include "error.h"

#include <stdlib.h>

/**
 * @file Portable and safe wrapers around some clib functions and some tools
 */
LUMIERA_ERROR_DECLARE(NO_MEMORY);

/**
 * Allocate memory.
 * always succeeds or dies
 * @param size memory to be allocated
 * @return pointer to the allocated memory
 */
void*
lumiera_malloc (size_t sz);


/**
 * Duplicate a C string.
 * always succeeds or dies
 * @param str string to be copied
 * @param len maximal length to be copied
 * @return pointer to the new string, "" if NULL was passed as str
 */
char*
lumiera_strndup (const char* str, size_t len);


/**
 * Compare two C strings.
 * Handles NULL pointers as "", shortcut for same addresses
 * @param a first string for comparsion
 * @param b second string for comparsion
 * @param len maximal length for the comparsion
 * @return 0 if the strings are identical, -1 if smaller 1 if bigger.
 */
int
lumiera_strncmp (const char* a, const char* b, size_t len);


/**
 * check 2 strings for identity.
 * @param a first string for comparsion
 * @param b second string for comparsion
 * @return 1 when the strings are the the same, 0 if not.
 */
int
lumiera_streq (const char* a, const char* b);


/**
 * Round robin temporary buffers.
 * This provides 64 buffers per thread which are recycled with each use.
 * The idea here is to have fast buffers for temporal data without need for explicit heap management.
 */

/**
 * free all buffers associated with this thread.
 * This function is called automatically, usually one doesnt need to call it.
 */
void
lumiera_tmpbuf_freeall (void);

/**
 * Query a thread local tmpbuf.
 * @param size minimal needed size for the tmpbuf
 * @return the tmpbuf
 */
void*
lumiera_tmpbuf_provide (size_t size);

/**
 * Duplicate string to a tmpbuf.
 * @param src string to be duplicated
 * @param size maximal length to be copied
 * @return temporary buffer containing a copy of the string
 */
char*
lumiera_tmpbuf_strndup (const char* src, size_t size);

/**
 * Construct a string in a tmpbuf.
 * @param size maximal length for the string
 * @param fmt printf like formatstring
 * @param ... parameters
 * @return temporary buffer containing the constructed of the string
 */
char*
lumiera_tmpbuf_sprintf (size_t size, const char* fmt, ...);

