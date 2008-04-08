/*
  condition.h  -  condition variables

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

#ifndef CINELERRA_CONDITION_H
#define CINELERRA_CONDITION_H

#include "lib/locking.h"

/**
 * @file Condition variables, header
 */


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


CinelerraCondition
cinelerra_condition_init (CinelerraCondition self);


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
 * conditionacquirer used to manage the state of a condition variable.
 */
struct cinelerra_conditionacquirer_struct
{
  CinelerraCondition cond;
  enum cinelerra_lockstate  state;
};
typedef struct cinelerra_conditionacquirer_struct cinelerra_conditionacquirer;
typedef struct cinelerra_conditionacquirer_struct* CinelerraConditionacquirer;

/* helper function for nobug */
static inline void
cinelerra_conditionacquirer_ensureunlocked (CinelerraConditionacquirer self)
{
  ENSURE (self->state == CINELERRA_UNLOCKED, "forgot to unlock the condition mutex");
}

/* override with a macro to use the cleanup checker */
#define cinelerra_conditionacquirer \
cinelerra_conditionacquirer NOBUG_CLEANUP(cinelerra_conditionacquirer_ensureunlocked)


/**
 * initialize a conditionacquirer state
 * @param self conditionacquirer to be initialized, must be an automatic variable
 * @param cond associated condition variable
 * @param state initial state of the mutex, either CINELERRA_LOCKED or CINELERRA_UNLOCKED
 * @return self as given
 * errors are fatal
 */
static inline CinelerraConditionacquirer
cinelerra_conditionacquirer_init (CinelerraConditionacquirer self, CinelerraCondition cond, enum cinelerra_lockstate state)
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
 * @param self conditionacquirer associated with a condition variable
 */
static inline void
cinelerra_conditionacquirer_lock (CinelerraConditionacquirer self)
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
 * @param self conditionacquirer associated with a condition variable
 */
static inline void
cinelerra_conditionacquirer_wait (CinelerraConditionacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == CINELERRA_LOCKED, "mutex must be locked");
  pthread_cond_wait (&self->cond->cond, &self->cond->mutex);
}


/**
 * release mutex.
 * a conditionacquirer must be unlocked before leaving scope
 * @param self conditionacquirer associated with a condition variable
 */
static inline int
cinelerra_conditionacquirer_unlock (CinelerraConditionacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == CINELERRA_LOCKED, "mutex was not locked");
  if (pthread_mutex_unlock (&self->cond->mutex))
    CINELERRA_DIE;
  self->state = CINELERRA_UNLOCKED;
}


/**
 * signal a single waiting thread
 * @param self conditionacquirer associated with the condition variable to be signaled
 */
static inline void
cinelerra_conditionacquirer_signal (CinelerraConditionacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == CINELERRA_LOCKED, "mutex was not locked");
  pthread_cond_signal (&self->cond->cond);
}


/**
 * signal all waiting threads
 * @param self conditionacquirer associated with the condition variable to be signaled
 */
static inline int
cinelerra_conditionacquirer_broadcast (CinelerraConditionacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == CINELERRA_LOCKED, "mutex was not locked");
  pthread_cond_broadcast (&self->cond->cond);
}


#endif
