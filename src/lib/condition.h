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
#include "lib/sectionlock.h"

#include <pthread.h>
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
         lumiera_cond_section_ = {                                                              \
         (void*)1, lumiera_condition_unlock_cb NOBUG_ALPHA_COMMA_NULL NOBUG_ALPHA_COMMA_NULL};  \
       lumiera_cond_section_.lock;)                                                             \
    for (                                                                                       \
         ({                                                                                     \
           lumiera_cond_section_.lock = (cnd);                                                  \
           NOBUG_IF_ALPHA(lumiera_cond_section_.flag = &NOBUG_FLAG(nobugflag);)                 \
             RESOURCE_ENTER (nobugflag, (cnd)->rh, "acquire condmutex",                         \
                             NOBUG_RESOURCE_WAITING, lumiera_cond_section_.rh);                 \
           if (pthread_mutex_lock (&(cnd)->cndmutex))                                           \
             LUMIERA_DIE (LOCK_ACQUIRE);                                                        \
           RESOURCE_STATE (nobugflag, NOBUG_RESOURCE_EXCLUSIVE, lumiera_cond_section_.rh);      \
         });                                                                                    \
         lumiera_cond_section_.lock;                                                            \
         ({                                                                                     \
           LUMIERA_CONDITION_SECTION_UNLOCK;                                                    \
         }))


#define LUMIERA_CONDITION_SECTION_CHAIN(nobugflag, cnd)                                         \
  for (lumiera_sectionlock *lumiera_lock_section_old_ = &lumiera_lock_section_,                 \
         NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked) lumiera_cond_section_ = {            \
         (void*)1, lumiera_condition_unlock_cb NOBUG_ALPHA_COMMA_NULL NOBUG_ALPHA_COMMA_NULL};  \
       lumiera_cond_section_.lock;)                                                             \
    for (                                                                                       \
         ({                                                                                     \
           REQUIRE (lumiera_lock_section_old_->lock, "section prematurely unlocked");           \
           lumiera_cond_section_.lock = cnd;                                                    \
           NOBUG_IF_ALPHA(lumiera_cond_section_.flag = &NOBUG_FLAG(nobugflag);)                 \
           RESOURCE_ENTER (nobugflag, (cnd)->rh, "acquire condmutex",                           \
                           NOBUG_RESOURCE_WAITING, lumiera_cond_section_.rh);                   \
           if (pthread_mutex_lock (&(cnd)->cndmutex))                                           \
             LUMIERA_DIE (LOCK_ACQUIRE);                                                        \
           RESOURCE_STATE (nobugflag, NOBUG_RESOURCE_EXCLUSIVE, lumiera_cond_section_.rh);      \
           LUMIERA_SECTION_UNLOCK_(lumiera_lock_section_old_);                                  \
         });                                                                                    \
         lumiera_cond_section_.lock;                                                            \
         ({                                                                                     \
           LUMIERA_CONDITION_SECTION_UNLOCK;                                                    \
         }))



#define LUMIERA_CONDITION_SECTION_UNLOCK                \
  LUMIERA_SECTION_UNLOCK_(&lumiera_cond_section_)


/**
 * Wait for a condition.
 * Must be used inside a CONDITION_SECTION.
 * @param expr Conditon which must become true, else the condition variable goes back into sleep
 */
#define LUMIERA_CONDITION_WAIT(expr)                                                                            \
  do {                                                                                                          \
    REQUIRE (lumiera_cond_section_.lock, "Condition mutex not locked");                                         \
    NOBUG_RESOURCE_STATE_RAW (lumiera_cond_section_.flag, NOBUG_RESOURCE_WAITING, lumiera_cond_section_.rh);    \
    pthread_cond_wait (&((LumieraCondition)lumiera_cond_section_.lock)->cond,                                   \
                       &((LumieraCondition)lumiera_cond_section_.lock)->cndmutex);                              \
    NOBUG_RESOURCE_STATE_RAW (lumiera_cond_section_.flag, NOBUG_RESOURCE_EXCLUSIVE, lumiera_cond_section_.rh);  \
  } while (!(expr))


/**
 * Signal a condition variable
 * Must be used inside a CONDITION_SECTION.
 * Wakes one thread waiting on the condition variable
 */
#define LUMIERA_CONDITION_SIGNAL                                                                \
  do {                                                                                          \
    REQUIRE (lumiera_cond_section_.lock, "Condition mutex not locked");                         \
    TRACE(NOBUG_FLAG_RAW(lumiera_cond_section_.flag), "Signal %p", &lumiera_cond_section_);     \
    pthread_cond_signal (&((LumieraCondition)lumiera_cond_section_.lock)->cond);                \
  } while (0)


/**
 * Broadcast a condition variable
 * Must be used inside a CONDITION_SECTION.
 * Wakes all threads waiting on the condition variable
 */
#define LUMIERA_CONDITION_BROADCAST                                                             \
  do {                                                                                          \
    REQUIRE (lumiera_cond_section_.lock, "Condition mutex not locked");                         \
    TRACE(NOBUG_FLAG_RAW(lumiera_cond_section_.flag), "Broadcast %p", &lumiera_cond_section_);  \
    pthread_cond_broadcast (&((LumieraCondition)lumiera_cond_section_.lock)->cond);             \
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

int
lumiera_condition_unlock_cb (void* cond);


#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
