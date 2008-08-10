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

#include "lib/error.h"
#include "lib/mutex.h"


/**
 * @file
 * Condition variables, header
 */

LUMIERA_ERROR_DECLARE (CONDITION_DESTROY);



/**
 * Condition section.
 * Locks the condition mutex, one can use LUMIERA_CONDITION_WAIT to wait for signals or
 * LUMIERA_CONDITION_SIGNAL or LUMIERA_CONDITION_BROADCAST to wake waiting threads
 */
#define LUMIERA_CONDITION_SECTION(nobugflag, cnd)                                                       \
  for (lumiera_conditionacquirer NOBUG_CLEANUP(lumiera_conditionacquirer_ensureunlocked)                \
         lumiera_condition_section_ = {(LumieraCondition)1};                                            \
       lumiera_condition_section_.condition;)                                                           \
    for (                                                                                               \
         ({                                                                                             \
           lumiera_condition_section_.condition = (cnd);                                                \
           NOBUG_IF(NOBUG_MODE_ALPHA, lumiera_condition_section_.flag = &NOBUG_FLAG(nobugflag));        \
           NOBUG_RESOURCE_HANDLE_INIT (lumiera_condition_section_.rh);                                  \
           RESOURCE_ENTER (nobugflag, (cnd)->rh, "acquire condition", &lumiera_condition_section_,      \
                           NOBUG_RESOURCE_EXCLUSIVE, lumiera_condition_section_.rh);                    \
           if (pthread_mutex_lock (&(cnd)->mutex)) LUMIERA_DIE (MUTEX_LOCK);                            \
         });                                                                                            \
         lumiera_condition_section_.condition;                                                          \
         ({                                                                                             \
           if (lumiera_condition_section_.condition)                                                    \
             {                                                                                          \
               pthread_mutex_unlock (&lumiera_condition_section_.condition->mutex);                     \
               lumiera_condition_section_.condition = NULL;                                             \
               RESOURCE_LEAVE(nobugflag, lumiera_condition_section_.rh);                                \
             }                                                                                          \
         }))

#define LUMIERA_CONDITION_WAIT                                                                                          \
  do {                                                                                                                  \
  NOBUG_RESOURCE_STATE_RAW (lumiera_condition_section_.flag, lumiera_condition_section_.rh, NOBUG_RESOURCEING);         \
  pthread_cond_wait (&lumiera_condition_section_.condition->cond, &lumiera_condition_section_.condition->mutex);        \
  NOBUG_RESOURCE_STATE_RAW (lumiera_condition_section_.flag, lumiera_condition_section_.rh, NOBUG_RESOURCE_EXCLUSIVE);  \
  while (0)

#define LUMIERA_CONDITION_SIGNAL (nobugflag) pthread_cond_signal (&lumiera_condition_section_.condition->cond)

#define LUMIERA_CONDITION_BROADCAST (nobugflag)  pthread_cond_broadcast (&lumiera_condition_section_.condition->cond)



/**
 * Condition variables.
 *
 */
struct lumiera_condition_struct
{
  pthread_cond_t cond;
  pthread_mutex_t mutex;
  RESOURCE_HANDLE (rh);
};
typedef struct lumiera_condition_struct lumiera_condition;
typedef lumiera_condition* LumieraCondition;


/**
 * Initialize a condition variable
 * @param self is a pointer to the condition variable to be initialized
 * @return self as given
 */
LumieraCondition
lumiera_condition_init (LumieraCondition self, const char* purpose, struct nobug_flag* flag);


/**
 * Destroy a condition variable
 * @param self is a pointer to the condition variable to be destroyed
 * @return self as given
 */
LumieraCondition
lumiera_condition_destroy (LumieraCondition self, struct nobug_flag* flag);


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
  LumieraCondition condition;
  NOBUG_IF(NOBUG_MODE_ALPHA, struct nobug_flag* flag);
  RESOURCE_HANDLE (rh);
};
typedef struct lumiera_conditionacquirer_struct lumiera_conditionacquirer;
typedef struct lumiera_conditionacquirer_struct* LumieraConditionacquirer;

/* helper function for nobug */
static inline void
lumiera_conditionacquirer_ensureunlocked (LumieraConditionacquirer self)
{
  ENSURE (!self->condition, "forgot to unlock condition variable");
}


#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
