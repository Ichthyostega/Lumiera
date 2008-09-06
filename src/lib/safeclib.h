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
 * @file
 * Portable and safe wrapers around some clib functions and some tools
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
 * Allocate cleared memory for an array.
 * always succeeds or dies
 * @param n number of elements
 * @param size memory to be allocated
 * @return pointer to the allocated memory
 */
void*
lumiera_calloc (size_t n, size_t size);


/**
 * Free previously allocated memory.
 * @param mem pointer to the memory block obtained by lumiera_malloc or lumiera_calloc
 */
static inline void
lumiera_free (void* mem)
{
  /* for now only a alias, might change in future */
  free (mem);
}


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
lumiera_tmpbuf_snprintf (size_t size, const char* fmt, ...);


/**
 * Concat up to 3 strings in a tmpbuf.
 * @param str1 first string to concat or NULL
 * @param str1_len how much of the first string shall be used
 * @param str2 second string to concat or NULL
 * @param str2_len how much of the second string shall be used
 * @param str3 third string to concat or NULL
 * @param str3_len how much of the third string shall be used
 * @return temporary buffer containing the constructed of the string
 */
char*
lumiera_tmpbuf_strcat3 (const char* str1, size_t str1_len,
                        const char* str2, size_t str2_len,
                        const char* str3, size_t str3_len);

/**
 * Translates characters in a string, similar to the shell 'tr' utility
 * @param in input string to  be translated
 * @param from source character set
 * @param to destination character set
 * @param def default destination character when a character is not in the source set,
 *        when NULL then translation will abort on unknown characters and return NULL,
 *        when "" then unknown characters will be removed
 *        when set to a single character string, unknown characters will be replaced with this string
 * @return temporary buffer containing the constructed of the string
 */
char*
lumiera_tmpbuf_tr (const char* in, const char* from, const char* to, const char* def);

