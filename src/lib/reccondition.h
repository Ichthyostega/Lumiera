/*
  reccondition.h  -  recursive locked condition variables

  Copyright (C)         Lumiera.org
    2008, 2009,         Christian Thaeter <ct@pipapo.org>

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

#ifndef LUMIERA_RECCONDITION_H
#define LUMIERA_RECCONDITION_H

#include "lib/error.h"
#include "lib/sectionlock.h"

#include <pthread.h>
#include <nobug.h>

/**
 * @file
 * Condition variables, header
 */


/**
 * Recursive Condition section.
 * Locks the condition mutex, one can use LUMIERA_RECCONDITION_WAIT to wait for signals or
 * LUMIERA_RECCONDITION_SIGNAL or LUMIERA_RECCONDITION_BROADCAST to wake waiting threads
 * @param nobugflag NoBug flag used to log actions on the condition
 * @param cnd Condition variable to be locked
 */
#define LUMIERA_RECCONDITION_SECTION(nobugflag, cnd)                                                    \
  for (lumiera_sectionlock NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked)                            \
         lumiera_reccond_section_ = {                                                                   \
         (void*)1, lumiera_reccondition_unlock_cb NOBUG_ALPHA_COMMA_NULL NOBUG_ALPHA_COMMA_NULL};       \
       lumiera_reccond_section_.lock;)                                                                  \
    for (                                                                                               \
         ({                                                                                             \
           lumiera_reccond_section_.lock = (cnd);                                                       \
           NOBUG_IF_ALPHA(lumiera_reccond_section_.flag = &NOBUG_FLAG(nobugflag);)                      \
           RESOURCE_WAIT (nobugflag, (cnd)->rh, "acquire reccondmutex", lumiera_reccond_section_.rh)    \
             {                                                                                          \
               if (pthread_mutex_lock (&(cnd)->reccndmutex))                                            \
                 LUMIERA_DIE (LOCK_ACQUIRE);                                                            \
               RESOURCE_STATE (nobugflag, NOBUG_RESOURCE_RECURSIVE, lumiera_reccond_section_.rh);       \
             }                                                                                          \
         });                                                                                            \
         lumiera_reccond_section_.lock;                                                                 \
         ({                                                                                             \
           LUMIERA_RECCONDITION_SECTION_UNLOCK;                                                         \
         }))



#define LUMIERA_RECCONDITION_SECTION_CHAIN(nobugflag, cnd)                                              \
  for (lumiera_sectionlock *lumiera_reccond_section_old_ = &lumiera_reccond_section_,                         \
         NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked) lumiera_reccond_section_ = {                 \
         (void*)1, lumiera_reccondition_unlock_cb NOBUG_ALPHA_COMMA_NULL NOBUG_ALPHA_COMMA_NULL};       \
       lumiera_reccond_section_.lock;)                                                                  \
    for (                                                                                               \
         ({                                                                                             \
           REQUIRE (lumiera_reccond_section_old_->lock, "section prematurely unlocked");                   \
           lumiera_reccond_section_.lock = (cnd);                                                       \
           NOBUG_IF_ALPHA(lumiera_reccond_section_.flag = &NOBUG_FLAG(nobugflag);)                      \
           RESOURCE_WAIT (nobugflag, (cnd)->rh, "acquire reccondmutex", lumiera_reccond_section_.rh)    \
             {                                                                                          \
               if (pthread_mutex_lock (&(cnd)->reccndmutex))                                            \
                 LUMIERA_DIE (LOCK_ACQUIRE);                                                            \
               RESOURCE_STATE (nobugflag, NOBUG_RESOURCE_RECURSIVE, lumiera_reccond_section_.rh);       \
               LUMIERA_SECTION_UNLOCK_(lumiera_reccond_section_old_);                                      \
             }                                                                                          \
         });                                                                                            \
         lumiera_reccond_section_.lock;                                                                 \
         ({                                                                                             \
           LUMIERA_RECCONDITION_SECTION_UNLOCK;                                                         \
         }))


#define LUMIERA_RECCONDITION_SECTION_UNLOCK             \
  LUMIERA_SECTION_UNLOCK_(&lumiera_reccond_section_)


/**
 * Wait for a condition.
 * Must be used inside a RECCONDITION_SECTION.
 * @param expr Condition which must become true, else the condition variable goes back into sleep
 */
#define LUMIERA_RECCONDITION_WAIT(expr)                                                                                 \
  do {                                                                                                                  \
    REQUIRE (lumiera_reccond_section_.lock, "Condition recmutex not locked");                                           \
    NOBUG_RESOURCE_STATE_RAW (lumiera_reccond_section_.flag, NOBUG_RESOURCE_WAITING, lumiera_reccond_section_.rh)       \
    {                                                                                                                   \
      pthread_cond_wait (&((LumieraReccondition)lumiera_reccond_section_.lock)->cond,                                   \
                         &((LumieraReccondition)lumiera_reccond_section_.lock)->reccndmutex);                           \
      NOBUG_RESOURCE_STATE_RAW (lumiera_reccond_section_.flag, NOBUG_RESOURCE_RECURSIVE, lumiera_reccond_section_.rh);  \
    }                                                                                                                   \
  } while (!(expr))


/**
 * Signal a condition variable
 * Must be used inside a RECCONDITION_SECTION.
 * Wakes one thread waiting on the condition variable
 */
#define LUMIERA_RECCONDITION_SIGNAL                                                                     \
  do {                                                                                                  \
    REQUIRE (lumiera_reccond_section_.lock, "Condition recmutex not locked");                           \
    TRACE(NOBUG_FLAG_RAW(lumiera_reccond_section_.flag), "Signal %p", &lumiera_reccond_section_);       \
    pthread_cond_signal (&((LumieraReccondition)lumiera_reccond_section_.lock)->cond);                  \
  } while (0)


/**
 * Broadcast a condition variable
 * Must be used inside a RECCONDITION_SECTION.
 * Wakes all threads waiting on the condition variable
 */
#define LUMIERA_RECCONDITION_BROADCAST                                                                  \
  do {                                                                                                  \
    REQUIRE (lumiera_reccond_section_.lock, "Condition recmutex not locked");                           \
    TRACE(NOBUG_FLAG_RAW(lumiera_reccond_section_.flag), "Broadcast %p", &lumiera_reccond_section_);    \
    pthread_cond_broadcast (&((LumieraReccondition)lumiera_reccond_section_.lock)->cond);               \
  } while (0)


/**
 * Condition variables. Recursive mutex variant.
 *
 */
struct lumiera_reccondition_struct
{
  pthread_cond_t cond;
  pthread_mutex_t reccndmutex;
  RESOURCE_HANDLE (rh);
};
typedef struct lumiera_reccondition_struct lumiera_reccondition;
typedef lumiera_reccondition* LumieraReccondition;


/**
 * Initialize a condition variable
 * @param self is a pointer to the condition variable to be initialized
 * @return self as given
 */
LumieraReccondition
lumiera_reccondition_init (LumieraReccondition self, const char* purpose, struct nobug_flag* flag);


/**
 * Destroy a condition variable
 * @param self is a pointer to the condition variable to be destroyed
 * @return self as given
 */
LumieraReccondition
lumiera_reccondition_destroy (LumieraReccondition self, struct nobug_flag* flag);


int
lumiera_reccondition_unlock_cb (void* cond);

#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
