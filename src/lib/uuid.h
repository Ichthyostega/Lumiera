/*
  uuid  -  Universal unique identifiers

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
#ifndef LUMIERA_UUID_H
#define LUMIERA_UUID_H
#include "lumiera.h"

typedef unsigned char lumiera_uuid[16];

/**
 * Retrieve a generic pointer stored in a uuid
 */
void*
lumiera_uuid_ptr_get (lumiera_uuid* uuid);

/**
 * Generate a new uuid
 */
void
lumiera_uuid_gen (lumiera_uuid* uuid);

/**
 * Store a generic pointer in a uuid
 */
void
lumiera_uuid_set_ptr (lumiera_uuid* uuid, void* ptr);


/**
 * Copy an uuid
 */
void
lumiera_uuid_copy (lumiera_uuid* dest, lumiera_uuid* src);


/**
 * Test 2 uuid's for equality
 */
int
lumiera_uuid_eq (lumiera_uuid* uuida, lumiera_uuid* uuidb);


/**
 * Generate a hashsum over an uuid
 */
size_t
lumiera_uuid_hash (lumiera_uuid* uuid);

#endif
