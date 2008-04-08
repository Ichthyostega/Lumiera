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

#ifndef CINELERRA_REFERENCES_H
#define CINELERRA_REFERENCES_H

#include <nobug.h>

/**
 * @file Strong and Weak references, header.
 */


typedef struct cinelerra_reference_struct cinelerra_reference;
typedef cinelerra_reference* CinelerraReference;

#include "lib/error.h"
#include "lib/mutex.h"

/* Implementation detail */
struct cinelerra_reftarget_struct
{
  void* object;
  void (*dtor)(void*);
  unsigned strong_cnt;          /*when strong becomes 0 obj is destroyed, if weak is 0 destroy target too*/
  unsigned weak_cnt;            /*when weak becomes 0 and !obj  and lock strong is 0, destroy target */
  cinelerra_mutex lock;
};
typedef struct cinelerra_reftarget_struct cinelerra_reftarget;
typedef cinelerra_reftarget* CinelerraReftarget;


/**
 * A reference pointing to some other object
 */
struct cinelerra_reference_struct
{
  void* object;    /*set for strong, NULL for weak*/
  CinelerraReftarget target;
};

#define cinelerra_reference \
cinelerra_reference NOBUG_CLEANUP(cinelerra_reference_ensuredestroyed)

/* helper function for nobug */
static inline void
cinelerra_reference_ensuredestroyed (CinelerraReference self)
{
  ENSURE (!self->target, "forgot to destroy reference");
}


CinelerraReference
cinelerra_reference_strong_init_once (CinelerraReference self, void* obj, void (*dtor)(void*));


CinelerraReference
cinelerra_reference_destroy (CinelerraReference self);

/**
 * Get object from a strong reference.
 * @return pointer to object, NULL if applied to a weak reference
 */
static inline void*
cinelerra_reference_get (CinelerraReference self)
{
  ENSURE (self->target, "illegal reference (not initialized or already destroyed?)");
  return self->object;
}


CinelerraReference
cinelerra_reference_strong_init (CinelerraReference self, CinelerraReference source);


CinelerraReference
cinelerra_reference_weak_init (CinelerraReference self, CinelerraReference source);


CinelerraReference
cinelerra_reference_weaken (restrict CinelerraReference self);


CinelerraReference
cinelerra_reference_strengthen (CinelerraReference self);

#endif
