/*
  tmpbuf.c      -  Round Robin Temporary buffers

  Copyright (C)                         Lumiera.org
    2008, 2010                          Christian Thaeter <ct@pipapo.org>

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

#ifndef LUMIERA_TMPBUF_H
#define LUMIERA_TMPBUF_H

#include <stdlib.h>

/**
 * @file
 * Round robin temporary buffers.
 * This provides some buffers per thread which are round-robin recycled with each use.
 * The idea is to have fast buffers for temporal data without need for explicit heap management.
 * 
 * @warning this is the restored old version from  bc989dab7a97fc69c (July 2010)
 *         (the improved version is still buggy as of 5/2011)
 */


/* following value must be exponent of 2 */
/**
 * Number of buffers in the ring
 * This also defines how many concurent buffers can be in use in one thread (including nested calls)
 * tmpbufs are only suitable for nested calls where one knows in advance how much tmpbufs might be used
 */
#define LUMIERA_TMPBUF_NUM 16



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
 *        when set to a single character string, unknown characters will be replaced with this character
 * @return temporary buffer containing the constructed of the string
 */
char*
lumiera_tmpbuf_tr (const char* in, const char* from, const char* to, const char* def);


#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
