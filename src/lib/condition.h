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

#ifndef LUMIERA_CONDITION_H
#define LUMIERA_CONDITION_H

#include "lib/locking.h"

/**
 * @file
 * Condition variables, header
 */

LUMIERA_ERROR_DECLARE (CONDITION_DESTROY);

/**
 * Condition variables.
 *
 */
struct lumiera_condition_struct
{
  pthread_cond_t cond;
  pthread_mutex_t mutex;
};
typedef struct lumiera_condition_struct lumiera_condition;
typedef lumiera_condition* LumieraCondition;


/**
 * Initialize a condition variable
 * @param self is a pointer to the condition variable to be initialized
 * @return self as given
 */
LumieraCondition
lumiera_condition_init (LumieraCondition self);


/**
 * Destroy a condition variable
 * @param self is a pointer to the condition variable to be destroyed
 * @return self as given
 */
LumieraCondition
lumiera_condition_destroy (LumieraCondition self);


/**
 * signal a single waiting thread.
 * @param self condition variable to be signaled, must be given, all errors are fatal
 */
static inline void
lumiera_condition_signal (LumieraCondition self)
{
  REQUIRE (self);
  if (pthread_mutex_lock (&self->mutex))
    LUMIERA_DIE (MUTEX_LOCK);
  pthread_cond_signal (&self->cond);
  if (pthread_mutex_unlock (&self->mutex))
    LUMIERA_DIE (MUTEX_UNLOCK);
}

/**
 * signal all waiting threads
 * @param self condition variable to be signaled, must be given, all errors are fatal
 */
static inline void
lumiera_condition_broadcast (LumieraCondition self)
{
  REQUIRE (self);
  if (pthread_mutex_lock (&self->mutex))
    LUMIERA_DIE (MUTEX_LOCK);
  pthread_cond_broadcast (&self->cond);
  if (pthread_mutex_unlock (&self->mutex))
    LUMIERA_DIE (MUTEX_UNLOCK);
}





/**
 * conditionacquirer used to manage the state of a condition variable.
 */
struct lumiera_conditionacquirer_struct
{
  LumieraCondition cond;
  enum lumiera_lockstate  state;
};
typedef struct lumiera_conditionacquirer_struct lumiera_conditionacquirer;
typedef struct lumiera_conditionacquirer_struct* LumieraConditionacquirer;

/* helper function for nobug */
static inline void
lumiera_conditionacquirer_ensureunlocked (LumieraConditionacquirer self)
{
  ENSURE (self->state == LUMIERA_UNLOCKED, "forgot to unlock the condition mutex");
}

/* override with a macro to use the cleanup checker */
#define lumiera_conditionacquirer \
lumiera_conditionacquirer NOBUG_CLEANUP(lumiera_conditionacquirer_ensureunlocked)


/**
 * initialize a conditionacquirer state
 * @param self conditionacquirer to be initialized, must be an automatic variable
 * @param cond associated condition variable
 * @param state initial state of the mutex, either LUMIERA_LOCKED or LUMIERA_UNLOCKED
 * @return self as given
 * errors are fatal
 */
static inline LumieraConditionacquirer
lumiera_conditionacquirer_init (LumieraConditionacquirer self, LumieraCondition cond, enum lumiera_lockstate state)
{
  REQUIRE (self);
  REQUIRE (cond);
  self->cond = cond;
  self->state = state;
  if (state == LUMIERA_LOCKED)
    if (pthread_mutex_lock (&cond->mutex))
      LUMIERA_DIE (MUTEX_LOCK);

  return self;
}

/**
 * lock the mutex.
 * must not already be locked
 * @param self conditionacquirer associated with a condition variable
 */
static inline void
lumiera_conditionacquirer_lock (LumieraConditionacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == LUMIERA_UNLOCKED, "mutex already locked");

  if (pthread_mutex_lock (&self->cond->mutex))
    LUMIERA_DIE (MUTEX_LOCK);

  self->state = LUMIERA_LOCKED;
}


/**
 * wait on a locked condition.
 * Waits until the condition variable gets signaled from another thread. Must already be locked.
 * @param self conditionacquirer associated with a condition variable
 */
static inline void
lumiera_conditionacquirer_wait (LumieraConditionacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == LUMIERA_LOCKED, "mutex must be locked");
  pthread_cond_wait (&self->cond->cond, &self->cond->mutex);
}


/**
 * release mutex.
 * a conditionacquirer must be unlocked before leaving scope
 * @param self conditionacquirer associated with a condition variable
 */
static inline void
lumiera_conditionacquirer_unlock (LumieraConditionacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == LUMIERA_LOCKED, "mutex was not locked");
  if (pthread_mutex_unlock (&self->cond->mutex))
    LUMIERA_DIE (MUTEX_UNLOCK);
  self->state = LUMIERA_UNLOCKED;
}


/**
 * signal a single waiting thread
 * @param self conditionacquirer associated with the condition variable to be signaled
 */
static inline void
lumiera_conditionacquirer_signal (LumieraConditionacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == LUMIERA_LOCKED, "mutex was not locked");
  pthread_cond_signal (&self->cond->cond);
}


/**
 * signal all waiting threads
 * @param self conditionacquirer associated with the condition variable to be signaled
 */
static inline void
lumiera_conditionacquirer_broadcast (LumieraConditionacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == LUMIERA_LOCKED, "mutex was not locked");
  pthread_cond_broadcast (&self->cond->cond);
}


#endif
