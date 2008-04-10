/*
  references.h  -  strong and weak references

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

#ifndef LUMIERA_REFERENCES_H
#define LUMIERA_REFERENCES_H

#include <nobug.h>

/**
 * @file
 * Strong and Weak references, header.
 */


typedef struct lumiera_reference_struct lumiera_reference;
typedef lumiera_reference* LumieraReference;

#include "lib/error.h"
#include "lib/mutex.h"

/* Implementation detail */
struct lumiera_reftarget_struct
{
  void* object;
  void (*dtor)(void*);
  unsigned strong_cnt;          /*when strong becomes 0 obj is destroyed, if weak is 0 destroy target too*/
  unsigned weak_cnt;            /*when weak becomes 0 and !obj  and lock strong is 0, destroy target */
  lumiera_mutex lock;
};
typedef struct lumiera_reftarget_struct lumiera_reftarget;
typedef lumiera_reftarget* LumieraReftarget;


/**
 * A reference pointing to some other object
 */
struct lumiera_reference_struct
{
  void* object;    /*set for strong, NULL for weak*/
  LumieraReftarget target;
};

#define lumiera_reference \
lumiera_reference NOBUG_CLEANUP(lumiera_reference_ensuredestroyed)

/* helper function for nobug */
static inline void
lumiera_reference_ensuredestroyed (LumieraReference self)
{
  ENSURE (!self->target, "forgot to destroy reference");
}


LumieraReference
lumiera_reference_strong_init_once (LumieraReference self, void* obj, void (*dtor)(void*));


LumieraReference
lumiera_reference_destroy (LumieraReference self);

/**
 * Get object from a strong reference.
 * @return pointer to object, NULL if applied to a weak reference
 */
static inline void*
lumiera_reference_get (LumieraReference self)
{
  ENSURE (self->target, "illegal reference (not initialized or already destroyed?)");
  return self->object;
}


LumieraReference
lumiera_reference_strong_init (LumieraReference self, LumieraReference source);


LumieraReference
lumiera_reference_weak_init (LumieraReference self, LumieraReference source);


LumieraReference
lumiera_reference_weaken (restrict LumieraReference self);


LumieraReference
lumiera_reference_strengthen (LumieraReference self);

#endif
