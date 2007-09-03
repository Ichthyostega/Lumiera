/*
  mutex.h  -  mutal exclusion locking

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

#ifndef CINELERRA_MUTEX_H
#define CINELERRA_MUTEX_H

#include "lib/locking.h"

/**
 * Mutex.
 *
 */
struct cinelerra_mutex_struct
{
  pthread_mutex_t mutex;
};
typedef struct cinelerra_mutex_struct cinelerra_mutex;
typedef cinelerra_mutex* CinelerraMutex;


/**
 * Initialize a mutex variable
 * @param self is a pointer to the mutex to be initialized
 * @return self as given
 */
CinelerraMutex
cinelerra_mutex_init (CinelerraMutex self);


/**
 * Destroy a mutex variable
 * @param self is a pointer to the mutex to be destroyed
 * @return self as given
 */
CinelerraMutex
cinelerra_mutex_destroy (CinelerraMutex self);



/**
 * mutexacquirer used to manage the state of a mutex variable.
 */
struct cinelerra_mutexacquirer_struct
{
  CinelerraMutex mutex;
  enum cinelerra_lockstate  state;
};
typedef struct cinelerra_mutexacquirer_struct cinelerra_mutexacquirer;
typedef struct cinelerra_mutexacquirer_struct* CinelerraMutexacquirer;

/* helper function for nobug */
static inline void
cinelerra_mutexacquirer_ensureunlocked (CinelerraMutexacquirer self)
{
  ENSURE (self->state == CINELERRA_UNLOCKED, "forgot to unlock mutex");
}

/* override with a macro to use the cleanup checker */
#define cinelerra_mutexacquirer \
cinelerra_mutexacquirer NOBUG_CLEANUP(cinelerra_mutexacquirer_ensureunlocked)


/**
 * initialize a mutexacquirer state
 * @param self mutexacquirer to be initialized, must be an automatic variable
 * @param mutex associated mutex
 * @param state initial state of the mutex, either CINELERRA_LOCKED or CINELERRA_UNLOCKED
 * @return self as given
 * errors are fatal
 */
static inline CinelerraMutexacquirer
cinelerra_mutexacquirer_init (CinelerraMutexacquirer self, CinelerraMutex mutex, enum cinelerra_lockstate state)
{
  REQUIRE (self);
  REQUIRE (mutex);
  self->mutex = mutex;
  self->state = state;
  if (state == CINELERRA_LOCKED)
    if (pthread_mutex_lock (&mutex->mutex))
      CINELERRA_DIE;

  return self;
}

/**
 * lock the mutex.
 * must not already be locked
 * @param self mutexacquirer associated with a mutex variable
 */
static inline void
cinelerra_mutexacquirer_lock (CinelerraMutexacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == CINELERRA_UNLOCKED, "mutex already locked");

  if (pthread_mutex_lock (&self->mutex->mutex))
    CINELERRA_DIE;

  self->state = CINELERRA_LOCKED;
}


/**
 * release mutex.
 * a mutexacquirer must be unlocked before leaving scope
 * @param self mutexacquirer associated with a mutex variable
 */
static inline int
cinelerra_mutexacquirer_unlock (CinelerraMutexacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == CINELERRA_LOCKED, "mutex was not locked");
  if (pthread_mutex_unlock (&self->mutex->mutex))
    CINELERRA_DIE;
  self->state = CINELERRA_UNLOCKED;
}

#endif
