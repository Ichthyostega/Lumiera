/*
  condition.h  -  condition variables

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

#ifndef CINELERRA_CONDITION_H
#define CINELERRA_CONDITION_H

#include "lib/locking.h"


/**
 * Condition variables.
 *
 */
struct cinelerra_condition_struct
{
  pthread_cond_t cond;
  pthread_mutex_t mutex;
};
typedef struct cinelerra_condition_struct cinelerra_condition;
typedef cinelerra_condition* CinelerraCondition;


/**
 * Initialize a condition variable
 * @param self is a pointer to the condition variable to be initialized
 * @return self as given
 */
CinelerraCondition
cinelerra_condition_init (CinelerraCondition self);


/**
 * Destroy a condition variable
 * @param self is a pointer to the condition variable to be destroyed
 * @return self as given
 */
CinelerraCondition
cinelerra_condition_destroy (CinelerraCondition self);


/**
 * signal a single waiting thread.
 * @param self condition variable to be signaled, must be given, all errors are fatal
 */
static inline void
cinelerra_condition_signal (CinelerraCondition self)
{
  REQUIRE (self);
  if (pthread_mutex_lock (&self->mutex))
    CINELERRA_DIE;
  pthread_cond_signal (&self->cond);
  if (pthread_mutex_unlock (&self->mutex))
    CINELERRA_DIE;
}

/**
 * signal all waiting threads
 * @param self condition variable to be signaled, must be given, all errors are fatal
 */
static inline void
cinelerra_condition_broadcast (CinelerraCondition self)
{
  REQUIRE (self);
  if (pthread_mutex_lock (&self->mutex))
    CINELERRA_DIE;
  pthread_cond_broadcast (&self->cond);
  if (pthread_mutex_unlock (&self->mutex))
    CINELERRA_DIE;
}





/**
 * conditionlock used to manage the state of a condition variable.
 */
struct cinelerra_conditionlock_struct
{
  CinelerraCondition cond;
  enum cinelerra_lockstate  state;
};
typedef struct cinelerra_conditionlock_struct cinelerra_conditionlock;
typedef struct cinelerra_conditionlock_struct* CinelerraConditionlock;

/* helper function for nobug */
static inline void
cinelerra_conditionlock_ensureunlocked (CinelerraConditionlock self)
{
  ENSURE (self->state == CINELERRA_UNLOCKED, "forgot to unlock the condition mutex");
}

/* override with a macro to use the cleanup checker */
#define cinelerra_conditionlock \
cinelerra_conditionlock NOBUG_CLEANUP(cinelerra_conditionlock_ensureunlocked)


/**
 * initialize a conditionlock state
 * @param self conditionlock to be initialized, must be an automatic variable
 * @param cond associated condition variable
 * @param state initial state of the mutex, either CINELERRA_LOCKED or CINELERRA_UNLOCKED
 * @return self as given
 * errors are fatal
 */
static inline CinelerraConditionlock
cinelerra_conditionlock_init (CinelerraConditionlock self, CinelerraCondition cond, enum cinelerra_lockstate state)
{
  REQUIRE (self);
  REQUIRE (cond);
  self->cond = cond;
  self->state = state;
  if (state == CINELERRA_LOCKED)
    if (pthread_mutex_lock (&cond->mutex))
      CINELERRA_DIE;

  return self;
}

/**
 * lock the mutex.
 * must not already be locked
 * @param self conditionlock associated with a condition variable
 */
static inline void
cinelerra_conditionlock_lock (CinelerraConditionlock self)
{
  REQUIRE (self);
  REQUIRE (self->state == CINELERRA_UNLOCKED, "mutex already locked");

  if (pthread_mutex_lock (&self->cond->mutex))
    CINELERRA_DIE;

  self->state = CINELERRA_LOCKED;
}


/**
 * wait on a locked condition.
 * Waits until the condition variable gets signaled from another thread. Must already be locked.
 * @param self conditionlock associated with a condition variable
 */
static inline void
cinelerra_conditionlock_wait (CinelerraConditionlock self)
{
  REQUIRE (self);
  REQUIRE (self->state == CINELERRA_LOCKED, "mutex must be locked");
  pthread_cond_wait (&self->cond->cond, &self->cond->mutex);
}


/**
 * release mutex.
 * a conditionlock must be unlocked before leaving scope
 * @param self conditionlock associated with a condition variable
 */
static inline int
cinelerra_conditionlock_unlock (CinelerraConditionlock self)
{
  REQUIRE (self);
  REQUIRE (self->state == CINELERRA_LOCKED, "mutex was not locked");
  if (pthread_mutex_unlock (&self->cond->mutex))
    CINELERRA_DIE;
  self->state = CINELERRA_UNLOCKED;
}


/**
 * signal a single waiting thread
 * @param self conditionlock associated with the condition variable to be signaled
 */
static inline void
cinelerra_conditionlock_signal (CinelerraConditionlock self)
{
  REQUIRE (self);
  REQUIRE (self->state == CINELERRA_LOCKED, "mutex was not locked");
  pthread_cond_signal (&self->cond->cond);
}


/**
 * signal all waiting threads
 * @param self conditionlock associated with the condition variable to be signaled
 */
static inline int
cinelerra_conditionlock_broadcast (CinelerraConditionlock self)
{
  REQUIRE (self);
  REQUIRE (self->state == CINELERRA_LOCKED, "mutex was not locked");
  pthread_cond_broadcast (&self->cond->cond);
}


#endif
