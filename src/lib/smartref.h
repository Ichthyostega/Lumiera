/*
  smartref.h  -  smart and weak references

  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>

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

#ifndef CINELERRA_SMARTREF_H
#define CINELERRA_SMARTREF_H

#include <nobug.h>

typedef struct cinelerra_reftarget_struct cinelerra_reftarget;
typedef cinelerra_reftarget* CinelerraReftarget;

typedef struct cinelerra_reference_struct cinelerra_reference;
typedef cinelerra_reference* CinelerraReference;

#include "lib/mutex.h"
#include "lib/llist.h"

/**
 * @file Smart and Weak references
 * Smart references keep some object alive while they exxisting
 * Weak references become invalidated when the referenced object gets destroyed
 */

/**
 *
 */

struct cinelerra_reftarget_struct
{
  void* object;
  void (*dtor)(void*);
  cinelerra_mutex lock;
  llist smartrefs;
  llist weakrefs;
};


CinelerraReftarget
cinelerra_reftarget_init (CinelerraReftarget self, void* obj, void (*dtor)(void*))
{
  cinelerra_mutex_init (&self->lock);
  llist_init (&self->smartrefs);
  llist_init (&self->weakrefs);
  self->object = obj;
  self->dtor = dtor;
  return self;
}

CinelerraReftarget
cinelerra_reftarget_destroy (CinelerraReftarget self)
{
  // lock
  // check smartrefs empty else error!
  // invalidate weakrefs
  // unlock and destroy mutex
  // if dtor -> call dtor
  return self;
}


/**
 *
 */

struct cinelerra_reference_struct
{
  cinelerra_mutex lock;
  CinelerraReftarget target;
  llist node;
};

/* helper function for nobug */
static inline void
cinelerra_reference_ensureunlinked (CinelerraReference self)
{
  ENSURE (llist_is_empty (&self->node), "forgot to destroy reference");
}

/* override with a macro to use the cleanup checker */
#define cinelerra_reference \
cinelerra_reference NOBUG_CLEANUP(cinelerra_reference_ensureunlinked)


/**
 *
 */

CinelerraReference
cinelerra_smartref_init (CinelerraReference self, CinelerraReftarget target)
{
  cinelerra_mutex_init (&self->lock);
  llist_init (&self->node);
  self->target = target;

  cinelerra_mutexacquirer l;
  cinelerra_mutexacquirer_init_mutex (&l, &target->lock, CINELERRA_LOCKED);
  llist_insert_tail (&target->smartrefs, &self->node);
  cinelerra_mutexacquirer_unlock (&l);
  return self;
}

CinelerraReference
cinelerra_smartref_destroy (CinelerraReference self)
{
  cinelerra_mutexacquirer l;
  cinelerra_mutexacquirer_init_mutex (&l, &self->target->lock, CINELERRA_LOCKED);
  llist_unlink (&self->node);
  CinelerraReftarget tmp = self->target;
  self->target = NULL;
  cinelerra_mutexacquirer_unlock (&l);

  cinelerra_mutex_destroy (&self->lock);
  return self;
}

void*
cinelerra_smartref_get (CinelerraReference self, CinelerraMutexacquirer lock)
{
  cinelerra_mutexacquirer_init_mutex (lock, &self->lock, CINELERRA_LOCKED);
  return self->target->object;
}

/**
 *
 */

CinelerraReference
cinelerra_weakref_init (CinelerraReference self, CinelerraReftarget target)
{
  cinelerra_mutex_init (&self->lock);
  llist_init (&self->node);
  self->target = target;

  cinelerra_mutexacquirer l;
  cinelerra_mutexacquirer_init_mutex (&l, &target->lock, CINELERRA_LOCKED);
  llist_insert_tail (&target->weakrefs, &self->node);
  cinelerra_mutexacquirer_unlock (&l);
  return self;
}

CinelerraReference
cinelerra_weakref_destroy (CinelerraReference self)
{
  //cinelerra_mutexacquirer l;
  //cinelerra_mutexacquirer_init_mutex (&l, &self->target->lock, CINELERRA_LOCKED);
  //llist_unlink (&self->node);
  //self->target = NULL;
  //cinelerra_mutexacquirer_unlock (&l);

  //cinelerra_mutex_destroy (&self->lock);
  return self;
}

void*
cinelerra_weakref_get (CinelerraReference self, CinelerraMutexacquirer lock)
{
  cinelerra_mutexacquirer_init_mutex (lock, &self->lock, CINELERRA_LOCKED);
  return self->target? self->target->object : NULL;
}



#endif
