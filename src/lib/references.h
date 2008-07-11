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


/**
 * Construct an initial strong reference from an object.
 * For every object which should be managed with references you need to construct an initial strong reference
 * which then will be used to initialize all further references.
 * @param self pointer to the reference to be initialized
 * @param obj pointer to the object being referenced
 * @param dtor destructor function which will be called on obj when the last strong reference gets deleted
 * @return self as given
 */
LumieraReference
lumiera_reference_strong_init_once (LumieraReference self, void* obj, void (*dtor)(void*));


/**
 * Destroy a reference.
 * All references need to be destroyed when not used any more.
 * When the last strong reference gets destroyed, the object's destructor is called.
 * Remaining weak references stay invalidated then until they get destroyed too.
 * @param self reference to be destroyed
 * @return self as given
 * destroying a reference is not thread safe as far as 2 threads try to concurrently destroy it!
 */
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


/**
 * Copy construct a reference as strong reference
 * @param source reference to copy
 * @return self as strong reference (always for strong references) or NULL if source is an invalidated weak reference,
 * in the later case the reference is constructed as weak reference barely to allow it be destroyed
 */
LumieraReference
lumiera_reference_strong_init (LumieraReference self, LumieraReference source);


/**
 * Copy construct a reference as weak reference
 * @param source reference to copy
 * @return self (always for strong references) or NULL if self is an invalidated weak reference
 */
LumieraReference
lumiera_reference_weak_init (LumieraReference self, LumieraReference source);


/**
 * turn a (strong) reference into a weak reference
 * Weaken a reference may remove its last strong reference and thus destroy the object
 * do nothing if the referene is already weak
 * @return self or NULL if the final strong reference got removed,
 */
LumieraReference
lumiera_reference_weaken (restrict LumieraReference self);


/**
 * turn a (weak) reference into a strong reference
 * only references of object which are not already destroyed can be strengthened
 * @return self when successful, NULL when the object was already destroyed, 'self' stays a dead weak reference in that case
 */
LumieraReference
lumiera_reference_strengthen (LumieraReference self);

#endif
