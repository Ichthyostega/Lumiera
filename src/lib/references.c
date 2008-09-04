/*
  references.c  -  strong and weak references

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
#include "lib/references.h"
#include "lib/safeclib.h"

/**
 * @file
 * Strong and Weak references
 * Strong references keep some object alive while they existing
 * Weak references become invalidated when the referenced object gets destroyed
 *
 * Thread safety: references protect their internal operations with a mutex (optimization using futex may come in future)
 * while operations on the reference itself (initialization, destruction, strengthen, weaken) and on the referenced object (get)
 * should be protected from elsewhere.
 */


LumieraReference
lumiera_reference_strong_init_once (LumieraReference self, void* obj, void (*dtor)(void*))
{
  LumieraReftarget target = lumiera_malloc (sizeof(lumiera_reftarget));

  target->object = obj;
  target->dtor = dtor;
  target->strong_cnt = 1;
  target->weak_cnt = 0;
  lumiera_mutex_init (&target->lock);

  self->object = obj;
  self->target = target;
  return self;
}


LumieraReference
lumiera_reference_destroy (LumieraReference self)
{
  LumieraReftarget target = self->target;

  /* defensive, lets detect errors if anything still tries to use this reference */
  self->target = NULL;

  lumiera_mutexacquirer lock;
  lumiera_mutexacquirer_init_mutex (&lock, &target->lock, LUMIERA_LOCKED);

  if (self->object)
    {
      /* strong reference */
      if (!--target->strong_cnt)
        {
          /* was last strong reference */
          if (target->dtor)
            target->dtor (target->object);
          target->object = NULL;
          if (!target->weak_cnt)
            {
              /* no weak refs either, destroy it */
              lumiera_mutexacquirer_unlock (&lock);
              lumiera_mutex_destroy (&target->lock);
              free (target);
              return self;
            }
        }
    }
  else
    {
      /* weak reference */
      if (!--target->weak_cnt && !target->strong_cnt)
        {
          /* was last weak reference, and no strong refs left */
          lumiera_mutexacquirer_unlock (&lock);
          lumiera_mutex_destroy (&target->lock);
          free (target);
          return self;
        }
    }
  lumiera_mutexacquirer_unlock (&lock);
  return self;
}


LumieraReference
lumiera_reference_strong_init (LumieraReference self, LumieraReference source)
{
  lumiera_mutexacquirer lock;
  lumiera_mutexacquirer_init_mutex (&lock, &source->target->lock, LUMIERA_LOCKED);

  self->object = source->target->object;
  self->target = source->target;

  if (self->object)
    {
      ++self->target->strong_cnt;
    }
  else
    {
      ++self->target->weak_cnt;
      self = NULL;
    }
  lumiera_mutexacquirer_unlock (&lock);
  return self;
}


LumieraReference
lumiera_reference_weak_init (LumieraReference self, LumieraReference source)
{
  lumiera_mutexacquirer lock;
  lumiera_mutexacquirer_init_mutex (&lock, &source->target->lock, LUMIERA_LOCKED);

  self->object = NULL;
  self->target = source->target;

  ++self->target->weak_cnt;
  if (!self->target->object)
    /* already invalidated */
    self = NULL;

  lumiera_mutexacquirer_unlock (&lock);
  return self;
}


LumieraReference
lumiera_reference_weaken (restrict LumieraReference self)
{
  /* is this a strong reference? */
  if (self->object)
    {
      lumiera_mutexacquirer lock;
      lumiera_mutexacquirer_init_mutex (&lock, &self->target->lock, LUMIERA_LOCKED);

      self->object = NULL;
      ++self->target->weak_cnt;
      if (!--self->target->strong_cnt)
        {
          if (self->target->dtor)
            self->target->dtor (self->target->object);
          self->target->object = NULL;
          self = NULL;
        }
      lumiera_mutexacquirer_unlock (&lock);
    }
  return self;
}


LumieraReference
lumiera_reference_strengthen (LumieraReference self)
{
  /* is this a weak reference? */
  if (!self->object)
    {
      lumiera_mutexacquirer lock;
      lumiera_mutexacquirer_init_mutex (&lock, &self->target->lock, LUMIERA_LOCKED);

      if (self->target->object)
        {
          self->object = self->target->object;
          --self->target->weak_cnt;
          ++self->target->strong_cnt;
        }
      else
        self = NULL;
      lumiera_mutexacquirer_unlock (&lock);
    }
  return self;
}

