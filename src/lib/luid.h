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
