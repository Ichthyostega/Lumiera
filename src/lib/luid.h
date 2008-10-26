/*
  luid  -  Lumiera unique identifiers

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
#ifndef LUMIERA_LUID_H
#define LUMIERA_LUID_H

#include <stdlib.h>

/**
 * @file
 * Lumiera unique identifiers are 128 byte random value. Unlike standard uuid's we
 * don't tag a version within them and we may store generic pointers in the space
 * occupied by an luid.
 */

typedef unsigned char lumiera_uid[16];
typedef lumiera_uid* LumieraUid;

/*
 C++ can't initialize arrays from string literals with the trailing \0 cropped
 C can't initialize non constant members,
 there we go
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
lumiera_uid_ptr_get (lumiera_uid* luid);

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
lumiera_uid_eq (lumiera_uid* luida, lumiera_uid* luidb);


/**
 * Generate a hashsum over an luid
 */
size_t
lumiera_uid_hash (lumiera_uid* luid);

#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
