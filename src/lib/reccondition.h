/*
  reccondition.h  -  recursive locked condition variables

  Copyright (C)         Lumiera.org
    2008, 2009, 2010,   Christian Thaeter <ct@pipapo.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

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
 * Recursive Condition section.
 * Locks the condition mutex, one can use LUMIERA_RECCONDITION_WAIT to wait for signals or
 * LUMIERA_RECCONDITION_SIGNAL or LUMIERA_RECCONDITION_BROADCAST to wake waiting threads
 * @param nobugflag NoBug flag used to log actions on the condition
 * @param cnd Condition variable to be locked
 */
#define LUMIERA_RECCONDITION_SECTION(nobugflag, cnd)                                    \
  for (lumiera_sectionlock NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked)            \
         lumiera_lock_section_ = {                                                      \
         cnd, (lumiera_sectionlock_unlock_fn) lumiera_reccondition_unlock               \
           NOBUG_ALPHA_COMMA(&NOBUG_FLAG(nobugflag)) NOBUG_ALPHA_COMMA_NULL};           \
       ({                                                                               \
         if (lumiera_lock_section_.lock)                                                \
           lumiera_lock_section_.lock =                                                 \
             lumiera_reccondition_lock (cnd, &NOBUG_FLAG(nobugflag),                    \
                                        &lumiera_lock_section_.rh, NOBUG_CONTEXT);      \
         lumiera_lock_section_.lock;                                                    \
       });                                                                              \
       ({                                                                               \
         LUMIERA_RECCONDITION_SECTION_UNLOCK;                                           \
       }))



#define LUMIERA_RECCONDITION_SECTION_CHAIN(nobugflag, cnd)                              \
  for (lumiera_sectionlock *lumiera_lock_section_old_ = &lumiera_lock_section_,         \
         NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked) lumiera_lock_section_ = {    \
         cnd, (lumiera_sectionlock_unlock_fn) lumiera_reccondition_unlock               \
           NOBUG_ALPHA_COMMA(&NOBUG_FLAG(nobugflag)) NOBUG_ALPHA_COMMA_NULL};           \
       ({                                                                               \
         if (lumiera_lock_section_.lock)                                                \
           {                                                                            \
             REQUIRE (lumiera_lock_section_old_->lock, "section prematurely unlocked"); \
             lumiera_lock_section_.lock =                                               \
               lumiera_reccondition_lock (cnd, &NOBUG_FLAG(nobugflag),                  \
                                          &lumiera_lock_section_.rh, NOBUG_CONTEXT);    \
             LUMIERA_SECTION_UNLOCK_(lumiera_lock_section_old_);                        \
           }                                                                            \
         lumiera_lock_section_.lock;                                                    \
       });                                                                              \
       ({                                                                               \
         LUMIERA_RECCONDITION_SECTION_UNLOCK;                                           \
       }))


#define LUMIERA_RECCONDITION_SECTION_UNLOCK             \
  LUMIERA_SECTION_UNLOCK_(&lumiera_lock_section_)


/**
 * Wait for a condition.
 * Must be used inside a RECCONDITION_SECTION.
 * @param expr Condition which must become true, else the condition variable goes back into sleep
 */
#define LUMIERA_RECCONDITION_WAIT(expr)                                         \
  while (!(expr)) {                                                             \
    REQUIRE (lumiera_lock_section_.lock, "Reccondition mutex not locked");      \
    lumiera_reccondition_wait (lumiera_lock_section_.lock,                      \
                               lumiera_lock_section_.flag,                      \
                               &lumiera_lock_section_.rh,                       \
                               NOBUG_CONTEXT);                                  \
  } while (!(expr))


/**
 * Timed wait for a condition.
 * Must be used inside a RECCONDITION_SECTION.
 * @param expr Recconditon which must become true, else the condition variable goes back into sleep
 * @param timeout time when the wait expired
 * sets LUMIERA_ERROR_LOCK_TIMEOUT when the timeout passed
 */
#define LUMIERA_RECCONDITION_TIMEDWAIT(expr, timeout)                           \
  while (!(expr)) {                                                             \
    REQUIRE (lumiera_lock_section_.lock, "Reccondition mutex not locked");      \
    if (!lumiera_reccondition_timedwait (lumiera_lock_section_.lock,            \
                                         timeout,                               \
                                         lumiera_lock_section_.flag,            \
                                         &lumiera_lock_section_.rh,             \
                                         NOBUG_CONTEXT))                        \
      break;                                                                    \
  }



/**
 * Signal a condition variable
 * Must be used inside a RECCONDITION_SECTION.
 * Wakes one thread waiting on the condition variable
 */
#define LUMIERA_RECCONDITION_SIGNAL                                             \
  do {                                                                          \
    REQUIRE (lumiera_lock_section_.lock, "Reccondition mutex not locked");      \
    lumiera_reccondition_signal (lumiera_lock_section_.lock,                    \
                                 lumiera_lock_section_.flag,                    \
                                 NOBUG_CONTEXT);                                \
  } while (0)


/**
 * Broadcast a condition variable
 * Must be used inside a RECCONDITION_SECTION.
 * Wakes all threads waiting on the condition variable
 */
#define LUMIERA_RECCONDITION_BROADCAST                                          \
  do {                                                                          \
    REQUIRE (lumiera_lock_section_.lock, "Reccondition mutex not locked");      \
    lumiera_reccondition_broadcast (lumiera_lock_section_.lock,                 \
                                    lumiera_lock_section_.flag,                 \
                                    NOBUG_CONTEXT);                             \
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
 * Initialise a condition variable
 * @param self is a pointer to the condition variable to be initialised
 * @return self as given
 */
LumieraReccondition
lumiera_reccondition_init (LumieraReccondition self,
                           const char* purpose,
                           struct nobug_flag* flag,
                           const struct nobug_context ctx);


/**
 * Destroy a condition variable
 * @param self is a pointer to the condition variable to be destroyed
 * @return self as given
 */
LumieraReccondition
lumiera_reccondition_destroy (LumieraReccondition self,
                              struct nobug_flag* flag,
                              const struct nobug_context ctx);



static inline LumieraReccondition
lumiera_reccondition_lock (LumieraReccondition self,
                           struct nobug_flag* flag,
                           struct nobug_resource_user** handle,
                           const struct nobug_context ctx)
{
  if (self)
    {
      NOBUG_RESOURCE_WAIT_CTX (NOBUG_FLAG_RAW(flag), self->rh, "acquire reccondvar", *handle, ctx)
        {
          if (pthread_mutex_lock (&self->reccndmutex))
            LUMIERA_DIE (LOCK_ACQUIRE);         /* never reached (in a correct program) */
          NOBUG_RESOURCE_STATE_CTX (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_RECURSIVE, *handle, ctx) /*{}*/;
        }
    }

  return self;
}


static inline LumieraReccondition
lumiera_reccondition_trylock (LumieraReccondition self,
                              struct nobug_flag* flag,
                              struct nobug_resource_user** handle,
                              const struct nobug_context ctx)
{
  if (self)
    {
      NOBUG_RESOURCE_TRY_CTX (NOBUG_FLAG_RAW(flag), self->rh, "try acquire reccondvar", *handle, ctx)
        {
          int err = pthread_mutex_trylock (&self->reccndmutex);

          if (!err)
            {
              NOBUG_RESOURCE_STATE_CTX (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_RECURSIVE, *handle, ctx) /*{}*/;
            }
          else
            {
              NOBUG_RESOURCE_LEAVE_RAW_CTX (flag, *handle, ctx) /*{}*/;
              lumiera_lockerror_set (err, flag, ctx);
              self = NULL;
            }
        }
    }

  return self;
}


static inline LumieraReccondition
lumiera_reccondition_timedlock (LumieraReccondition self,
                                const struct timespec* timeout,
                                struct nobug_flag* flag,
                                struct nobug_resource_user** handle,
                                const struct nobug_context ctx)
{
  if (self)
    {
      NOBUG_RESOURCE_TRY_CTX (NOBUG_FLAG_RAW(flag), self->rh, "timed acquire reccondvar", *handle, ctx)
        {
          int err = pthread_mutex_timedlock (&self->reccndmutex, timeout);

          if (!err)
            {
              NOBUG_RESOURCE_STATE_CTX (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_RECURSIVE, *handle, ctx) /*{}*/;
            }
          else
            {
              NOBUG_RESOURCE_LEAVE_RAW_CTX (flag, *handle, ctx) /*{}*/;
              lumiera_lockerror_set (err, flag, ctx);
              self = NULL;
            }
        }
    }

  return self;
}


static inline LumieraReccondition
lumiera_reccondition_wait (LumieraReccondition self,
                           struct nobug_flag* flag,
                           struct nobug_resource_user** handle,
                           const struct nobug_context ctx)
{
  if (self)
    {
      NOBUG_RESOURCE_STATE_RAW_CTX (flag, NOBUG_RESOURCE_WAITING, *handle, ctx)
        {
          pthread_cond_wait (&self->cond, &self->reccndmutex);
          NOBUG_RESOURCE_STATE_RAW_CTX (flag, NOBUG_RESOURCE_RECURSIVE, *handle, ctx) /*{}*/;
        }
    }
  return self;
}


static inline LumieraReccondition
lumiera_reccondition_timedwait (LumieraReccondition self,
                                const struct timespec* timeout,
                                struct nobug_flag* flag,
                                struct nobug_resource_user** handle,
                                const struct nobug_context ctx)
{
  if (self)
    {
      NOBUG_RESOURCE_STATE_RAW_CTX (flag, NOBUG_RESOURCE_WAITING, *handle, ctx)
        {
          int err;
          do {
            err = pthread_cond_timedwait (&self->cond, &self->reccndmutex, timeout);
          } while(err == EINTR);

          if (!err)
            NOBUG_RESOURCE_STATE_RAW_CTX (flag, NOBUG_RESOURCE_RECURSIVE, *handle, ctx) /*{}*/;
          else
            {
              lumiera_lockerror_set (err, flag, ctx);
              self = NULL;
            }
        }
    }

  return self;
}



static inline void
lumiera_reccondition_signal (LumieraReccondition self,
                             struct nobug_flag* flag,
                             const struct nobug_context ctx)
{
  NOBUG_TRACE_CTX (NOBUG_FLAG_RAW (flag), ctx, "Signal %p", self);
  pthread_cond_signal (&self->cond);
}


static inline void
lumiera_reccondition_broadcast (LumieraReccondition self,
                                struct nobug_flag* flag,
                                const struct nobug_context ctx)
{
  NOBUG_TRACE_CTX (NOBUG_FLAG_RAW(flag), ctx, "Broadcast %p", self);
  pthread_cond_broadcast (&self->cond);
}


static inline void
lumiera_reccondition_unlock (LumieraReccondition self,
                             struct nobug_flag* flag,
                             struct nobug_resource_user** handle,
                             const struct nobug_context ctx)
{
  NOBUG_REQUIRE_CTX (self, ctx);

  NOBUG_RESOURCE_LEAVE_RAW_CTX (flag, *handle, ctx)
    {
      if (pthread_mutex_unlock (&self->reccndmutex))
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
