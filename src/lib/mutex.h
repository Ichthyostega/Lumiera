/*
  mutex.h  -  mutal exclusion locking

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

#ifndef LUMIERA_MUTEX_H
#define LUMIERA_MUTEX_H

#include "lib/locking.h"

/**
 * @file Mutual exclusion locking, header.
 */


/**
 * Mutex.
 *
 */
struct lumiera_mutex_struct
{
  pthread_mutex_t mutex;
};
typedef struct lumiera_mutex_struct lumiera_mutex;
typedef lumiera_mutex* LumieraMutex;


LumieraMutex
lumiera_mutex_init (LumieraMutex self);


LumieraMutex
lumiera_mutex_destroy (LumieraMutex self);



/**
 * mutexacquirer used to manage the state of a mutex variable.
 */
struct lumiera_mutexacquirer_struct
{
  LumieraMutex mutex;
  enum lumiera_lockstate  state;
};
typedef struct lumiera_mutexacquirer_struct lumiera_mutexacquirer;
typedef struct lumiera_mutexacquirer_struct* LumieraMutexacquirer;

/* helper function for nobug */
static inline void
lumiera_mutexacquirer_ensureunlocked (LumieraMutexacquirer self)
{
  ENSURE (self->state == LUMIERA_UNLOCKED, "forgot to unlock mutex");
}

/* override with a macro to use the cleanup checker */
#define lumiera_mutexacquirer \
lumiera_mutexacquirer NOBUG_CLEANUP(lumiera_mutexacquirer_ensureunlocked)


/**
 * initialize a mutexacquirer state without mutex.
 * @param self mutexacquirer to be initialized, must be an automatic variable
 * @return self as given
 * This initialization is used when lumiera_mutexacquirer_try_mutex shall be used later
 */
static inline LumieraMutexacquirer
lumiera_mutexacquirer_init (LumieraMutexacquirer self)
{
  REQUIRE (self);
  self->mutex = NULL;
  self->state = LUMIERA_UNLOCKED;

  return self;
}

/**
 * initialize a mutexacquirer state
 * @param self mutexacquirer to be initialized, must be an automatic variable
 * @param mutex associated mutex
 * @param state initial state of the mutex, either LUMIERA_LOCKED or LUMIERA_UNLOCKED
 * @return self as given
 * errors are fatal
 */
static inline LumieraMutexacquirer
lumiera_mutexacquirer_init_mutex (LumieraMutexacquirer self, LumieraMutex mutex, enum lumiera_lockstate state)
{
  REQUIRE (self);
  REQUIRE (mutex);
  self->mutex = mutex;
  self->state = state;
  if (state == LUMIERA_LOCKED)
    if (pthread_mutex_lock (&mutex->mutex))
      LUMIERA_DIE;

  return self;
}


/**
 * lock the mutex.
 * must not already be locked
 * @param self mutexacquirer associated with a mutex variable
 */
static inline void
lumiera_mutexacquirer_lock (LumieraMutexacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == LUMIERA_UNLOCKED, "mutex already locked");

  if (pthread_mutex_lock (&self->mutex->mutex))
    LUMIERA_DIE;

  self->state = LUMIERA_LOCKED;
}


/**
 * get the state of a lock.
 * @param self mutexacquirer associated with a mutex variable
 * @return LUMIERA_LOCKED when the mutex is locked by this thead
 */
static inline enum lumiera_lockstate
lumiera_mutexacquirer_state (LumieraMutexacquirer self)
{
  REQUIRE (self);
  return self->state;
}


/**
 * try to lock a mutex.
 * must not already be locked
 * @param self mutexacquirer associated with a mutex variable
 * @param mutex pointer to a mutex which should be tried
 * @return LUMIERA_LOCKED when the mutex got locked
 */
static inline enum lumiera_lockstate
lumiera_mutexacquirer_try_mutex (LumieraMutexacquirer self, LumieraMutex mutex)
{
  REQUIRE (self);
  REQUIRE (self->state == LUMIERA_UNLOCKED, "mutex already locked");

  self->mutex=mutex;
  switch (pthread_mutex_trylock (&self->mutex->mutex))
    {
    case 0:
      return self->state = LUMIERA_LOCKED;
    case EBUSY:
      return LUMIERA_UNLOCKED;
    default:
      LUMIERA_DIE;
    }
}


/**
 * release mutex.
 * a mutexacquirer must be unlocked before leaving scope
 * @param self mutexacquirer associated with a mutex variable
 */
static inline void
lumiera_mutexacquirer_unlock (LumieraMutexacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == LUMIERA_LOCKED, "mutex was not locked");
  if (pthread_mutex_unlock (&self->mutex->mutex))
    LUMIERA_DIE;
  self->state = LUMIERA_UNLOCKED;
}

#endif
