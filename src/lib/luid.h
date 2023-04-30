/*
  LUID.h  -  Lumiera unique identifiers

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


/** @file luid.h
 ** Lumiera unique object identifier.
 ** Lumiera unique identifiers are 128 byte random values. Unlike standard uuid's
 ** we don't tag a version within them and we may store generic pointers within the
 ** storage space occupied by an LUID.
 ** 
 ** Due to the extremely huge number space, LUID values can be used as unique identifiers
 ** without the need to check for duplicates or collisions. At various places, LUIDs are
 ** thus used right away on creation of new object instances or elements, in case a
 ** distinguishable <i>object identity</i> is required, e.g.
 ** - any new attachment of an object into the session ("placement")
 ** - unique output designation discovered during the translation into a low-level
 **   node graph ("builder")
 ** - interface slots for external binding and plug-ins
 ** 
 ** Moreover, there is a \link luidgen.c Luidgen \endlink tool to generate fixed LUIDs
 ** to be included into source code. It works by replacing the token \c LUIDGEN in the
 ** source code text by a newly generated (random) LUID in octal representation.
 ** 
 ** LUIDs can also be used to generate hash values for hash table storage.
 ** 
 ** @see HashIndexed Adapter for including a LUID into an object
 ** @see Placement usage example
 ** @see luidgen.c
 ** @see hash-util.h hash type definitions
 **
 */


#ifndef LIB_LUID_H
#define LIB_LUID_H


#include "lib/hash-value.h"  /* Types lumiera_uid and LumieraUid */

#include <stdlib.h>



/*
 C++ can't initialise arrays from string literals with the trailing \0 cropped
 C can't initialise non constant members,
 there we go
 ////////////////////////////////TODO 3/2014 possibly not used anymore, after removing LUIDs from the Interface descriptors (see #925)
*/
#ifdef __cplusplus
#define LUMIERA_UID_INITIALIZER(l)                              \
  {                                                             \
    l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7],             \
    l[8], l[9], l[10], l[11], l[12], l[13], l[14], l[15]        \
  }
#else
#define LUMIERA_UID_INITIALIZER(l) l
#endif

#define LUMIERA_UID_FMT                 \
  "\\%.3hho\\%.3hho\\%.3hho\\%.3hho"    \
  "\\%.3hho\\%.3hho\\%.3hho\\%.3hho"    \
  "\\%.3hho\\%.3hho\\%.3hho\\%.3hho"    \
  "\\%.3hho\\%.3hho\\%.3hho\\%.3hho"

#define LUMIERA_UID_CHAR(l,n) ((unsigned char*)l)[n]


#define LUMIERA_UID_ELEMENTS(l)                                                                         \
  LUMIERA_UID_CHAR(l,0), LUMIERA_UID_CHAR(l,1), LUMIERA_UID_CHAR(l,2), LUMIERA_UID_CHAR(l,3),           \
    LUMIERA_UID_CHAR(l,4), LUMIERA_UID_CHAR(l,5), LUMIERA_UID_CHAR(l,6), LUMIERA_UID_CHAR(l,7),         \
    LUMIERA_UID_CHAR(l,8), LUMIERA_UID_CHAR(l,9), LUMIERA_UID_CHAR(l,10), LUMIERA_UID_CHAR(l,11),       \
    LUMIERA_UID_CHAR(l,12), LUMIERA_UID_CHAR(l,13), LUMIERA_UID_CHAR(l,14), LUMIERA_UID_CHAR(l,15)

/**
 * LUIDGEN will be replaced by the 'luidgen' tool with a random uuid
 */
#define LUIDGEN PLEASE_RUN_THE_LUIDGEN_TOOL_ON_THIS_FILE


/**
 * Retrieve a generic pointer stored in a luid
 */
void*
lumiera_uid_ptr_get (const lumiera_uid* luid);

/**
 * Generate a new luid
 */
void
lumiera_uid_gen (lumiera_uid* luid);

/**
 * Store a generic pointer in a luid
 */
void
lumiera_uid_set_ptr (lumiera_uid* luid, void* ptr);


/**
 * Copy an luid
 */
void
lumiera_uid_copy (lumiera_uid* dest, lumiera_uid* src);


/**
 * Test 2 luid's for equality
 */
int
lumiera_uid_eq (const lumiera_uid* luida, const lumiera_uid* luidb);


/**
 * Generate a hash sum over an luid
 */
size_t
lumiera_uid_hash (const lumiera_uid* luid);

#endif /*LIB_LUID_H*/
