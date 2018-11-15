/*
  SAFECLIB.h  -  Portable and safe wrappers around some clib functions and some tools

  Copyright (C)         CinelerraCV
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


/** @file safeclib.h
 ** Portable and safe wrappers around some C-Lib functions
 */


#include "error.h"

#include <stdlib.h>

LUMIERA_ERROR_DECLARE(NO_MEMORY);

/**
 * Install the resourcecollector run hook.
 * The resourcecollectr must be hooked into the safeclib at bootup after it got
 * initialised and removed from it before shut down. Without resourcecollector
 * failed allocations will abort().
 * @param hook pointer to the resourcecollector_run function, must be of type
 *        lumiera_resourcecollector_run_fn but we don't want a dependency on vault in this header
 */
void
lumiera_safeclib_set_resourcecollector (void* hook);


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
 * Change the size of a memory block.
 * @param ptr pointer to the old memory block obtained by lumiera_malloc or lumiera_calloc
 * @param size new size of the block
 * @return address of new block
 */
void*
lumiera_realloc (void* ptr, size_t size);


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

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
