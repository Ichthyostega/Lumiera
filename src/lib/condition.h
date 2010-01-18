/*
  condition.h  -  condition variables

  Copyright (C)         Lumiera.org
    2008, 2010,         Christian Thaeter <ct@pipapo.org>

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
#include "lib/sectionlock.h"
#include "lib/lockerror.h"

#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <nobug.h>

/**
 * @file
 * Condition variables, header
 */


/**
 * Condition section.
 * Locks the condition mutex, one can use LUMIERA_CONDITION_WAIT to wait for signals or
 * LUMIERA_CONDITION_SIGNAL or LUMIERA_CONDITION_BROADCAST to wake waiting threads
 * Condition variables must be at the end of locking chains, they can not be used at
 * intermediate position.
 * @param nobugflag NoBug flag used to log actions on the condition
 * @param cnd Condition variable to be locked
 */
#define LUMIERA_CONDITION_SECTION(nobugflag, cnd)                                               \
  for (lumiera_sectionlock NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked)                    \
         lumiera_lock_section_ = {                                                              \
         cnd, (lumiera_sectionlock_unlock_fn) lumiera_condition_unlock                          \
           NOBUG_ALPHA_COMMA(&NOBUG_FLAG(nobugflag)) NOBUG_ALPHA_COMMA_NULL};                   \
       ({                                                                                       \
         if (lumiera_lock_section_.lock)                                                        \
           lumiera_lock_section_.lock =                                                         \
             lumiera_condition_lock (cnd, &NOBUG_FLAG(nobugflag), &lumiera_lock_section_.rh);   \
         lumiera_lock_section_.lock;                                                            \
       });                                                                                      \
       ({                                                                                       \
         LUMIERA_CONDITION_SECTION_UNLOCK;                                                      \
       }))


#define LUMIERA_CONDITION_SECTION_CHAIN(nobugflag, cnd)                                         \
  for (lumiera_sectionlock *lumiera_lock_section_old_ = &lumiera_lock_section_,                 \
         NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked) lumiera_lock_section_ = {            \
         cnd, (lumiera_sectionlock_unlock_fn) lumiera_condition_unlock                          \
           NOBUG_ALPHA_COMMA(&NOBUG_FLAG(nobugflag)) NOBUG_ALPHA_COMMA_NULL};                   \
       ({                                                                                       \
         if (lumiera_lock_section_.lock)                                                        \
           {                                                                                    \
             REQUIRE (lumiera_lock_section_old_->lock, "section prematurely unlocked");         \
             lumiera_lock_section_.lock =                                                       \
               lumiera_condition_lock (cnd, &NOBUG_FLAG(nobugflag), &lumiera_lock_section_.rh); \
             LUMIERA_SECTION_UNLOCK_(lumiera_lock_section_old_);                                \
           }                                                                                    \
         lumiera_lock_section_.lock;                                                            \
       });                                                                                      \
       ({                                                                                       \
         LUMIERA_CONDITION_SECTION_UNLOCK;                                                      \
       }))


#define LUMIERA_CONDITION_SECTION_UNLOCK                \
  LUMIERA_SECTION_UNLOCK_(&lumiera_lock_section_)


/**
 * Wait for a condition.
 * Must be used inside a CONDITION_SECTION.
 * @param expr Conditon which must become true, else the condition variable goes back into sleep
 */
#define LUMIERA_CONDITION_WAIT(expr)                                    \
  while (!(expr)) {                                                     \
    REQUIRE (lumiera_lock_section_.lock, "Condition mutex not locked"); \
    lumiera_condition_wait (lumiera_lock_section_.lock,                 \
                            lumiera_lock_section_.flag,                 \
                            &lumiera_lock_section_.rh);                 \
  }


/**
 * Timed wait for a condition.
 * Must be used inside a CONDITION_SECTION.
 * @param expr Conditon which must become true, else the condition variable goes back into sleep
 * @param timeout time when the wait expired
 * sets LUMIERA_ERROR_LOCK_TIMEOUT when the timeout passed
 */
#define LUMIERA_CONDITION_TIMEDWAIT(expr, timeout)                      \
  while (!(expr)) {                                                     \
    REQUIRE (lumiera_lock_section_.lock, "Condition mutex not locked"); \
    if (!lumiera_condition_timedwait (lumiera_lock_section_.lock,       \
                                      timeout,                          \
                                      lumiera_lock_section_.flag,       \
                                      &lumiera_lock_section_.rh))       \
      break;                                                            \
  }

/**
 * Signal a condition variable
 * Must be used inside a CONDITION_SECTION.
 * Wakes one thread waiting on the condition variable
 */
#define LUMIERA_CONDITION_SIGNAL                                        \
  do {                                                                  \
    REQUIRE (lumiera_lock_section_.lock, "Condition mutex not locked"); \
    lumiera_condition_signal (lumiera_lock_section_.lock,               \
                              lumiera_lock_section_.flag);              \
  } while (0)


/**
 * Broadcast a condition variable
 * Must be used inside a CONDITION_SECTION.
 * Wakes all threads waiting on the condition variable
 */
#define LUMIERA_CONDITION_BROADCAST                                     \
  do {                                                                  \
    REQUIRE (lumiera_lock_section_.lock, "Condition mutex not locked"); \
    lumiera_condition_broadcast (lumiera_lock_section_.lock,            \
                                 lumiera_lock_section_.flag);           \
  } while (0)


/**
 * Condition variables.
 *
 */
struct lumiera_condition_struct
{
  pthread_cond_t cond;
  pthread_mutex_t cndmutex;
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


static inline LumieraCondition
lumiera_condition_lock (LumieraCondition self, struct nobug_flag* flag, struct nobug_resource_user** handle)
{
  if (self)
    {
      RESOURCE_WAIT (NOBUG_FLAG_RAW(flag), self->rh, "acquire condvar", *handle);

      if (pthread_mutex_lock (&self->cndmutex))
        LUMIERA_DIE (LOCK_ACQUIRE);         /* never reached (in a correct program) */

      RESOURCE_STATE (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_EXCLUSIVE, *handle);
    }

  return self;
}


static inline LumieraCondition
lumiera_condition_trylock (LumieraCondition self, struct nobug_flag* flag, struct nobug_resource_user** handle)
{
  if (self)
    {
      NOBUG_RESOURCE_TRY (NOBUG_FLAG_RAW(flag), self->rh, "try acquire condvar", *handle);

      int err = pthread_mutex_trylock (&self->cndmutex);

      if (!err)
        {
          RESOURCE_STATE (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_EXCLUSIVE, *handle);
        }
      else
        {
          NOBUG_RESOURCE_LEAVE_RAW(flag, *handle) /*{}*/;
          lumiera_lockerror_set (err, flag, __func__);
          return NULL;
        }
    }

  return self;
}


static inline LumieraCondition
lumiera_condition_timedlock (LumieraCondition self,
                             const struct timespec* timeout,
                             struct nobug_flag* flag,
                             struct nobug_resource_user** handle)
{
  if (self)
    {
      NOBUG_RESOURCE_TRY (NOBUG_FLAG_RAW(flag), self->rh, "timed acquire condvar", *handle);

      int err = pthread_mutex_timedlock (&self->cndmutex, timeout);

      if (!err)
        {
          RESOURCE_STATE (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_EXCLUSIVE, *handle);
        }
      else
        {
          NOBUG_RESOURCE_LEAVE_RAW(flag, *handle) /*{}*/;
          lumiera_lockerror_set (err, flag, __func__);
          return NULL;
        }
    }

  return self;
}


static inline LumieraCondition
lumiera_condition_wait (LumieraCondition self, struct nobug_flag* flag, struct nobug_resource_user** handle)
{
  if (self)
    {
      NOBUG_RESOURCE_STATE_RAW (flag, NOBUG_RESOURCE_WAITING, *handle);

      pthread_cond_wait (&self->cond, &self->cndmutex);

      NOBUG_RESOURCE_STATE_RAW (flag, NOBUG_RESOURCE_EXCLUSIVE, *handle);
    }
  return self;
}


static inline LumieraCondition
lumiera_condition_timedwait (LumieraCondition self,
                             const struct timespec* timeout,
                             struct nobug_flag* flag,
                             struct nobug_resource_user** handle)
{
  if (self)
    {
      NOBUG_RESOURCE_STATE_RAW (flag, NOBUG_RESOURCE_WAITING, *handle);

      int err;
      do {
        err = pthread_cond_timedwait (&self->cond, &self->cndmutex, timeout);
      } while(err == EINTR);

      NOBUG_RESOURCE_STATE_RAW (flag, NOBUG_RESOURCE_EXCLUSIVE, *handle);

      if (err)
        {
          lumiera_lockerror_set (err, flag, __func__);
          return NULL;
        }
    }

  return self;
}



static inline void
lumiera_condition_signal (LumieraCondition self, struct nobug_flag* flag)
{
  TRACE(NOBUG_FLAG_RAW(flag), "Signal %p", self);
  pthread_cond_signal (&self->cond);
}


static inline void
lumiera_condition_broadcast (LumieraCondition self, struct nobug_flag* flag)
{
  TRACE(NOBUG_FLAG_RAW(flag), "Broadcast %p", self);
  pthread_cond_broadcast (&self->cond);
}


static inline void
lumiera_condition_unlock (LumieraCondition self, struct nobug_flag* flag, struct nobug_resource_user** handle)
{
  REQUIRE(self);

  NOBUG_RESOURCE_LEAVE_RAW(flag, *handle)
    {
      if (pthread_mutex_unlock (&self->cndmutex))
        LUMIERA_DIE (LOCK_RELEASE);       /* never reached (in a correct program) */
    }
}


#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
