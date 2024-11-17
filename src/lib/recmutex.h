/*
  recmutex.h  -  recursive mutual exclusion locking

   Copyright (C)
     2008, 2009,      Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.
*/

#ifndef LUMIERA_RECMUTEX_H
#define LUMIERA_RECMUTEX_H

#include "lib/error.h"
#include "lib/sectionlock.h"
#include "lib/lockerror.h"
#include "lib/mutex.h"

#include <pthread.h>
#include <nobug.h>

/**
 * @file
 * Mutual exclusion locking, header.
 */

/**
 * Recursive Mutual exclusive section.
 */
#define LUMIERA_RECMUTEX_SECTION(nobugflag, mtx)                                \
  for (lumiera_sectionlock NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked)    \
         lumiera_lock_section_ = {                                              \
         mtx, (lumiera_sectionlock_unlock_fn) lumiera_mutex_unlock              \
           NOBUG_ALPHA_COMMA(&NOBUG_FLAG(nobugflag)) NOBUG_ALPHA_COMMA_NULL};   \
       ({                                                                       \
         if (lumiera_lock_section_.lock)                                        \
           lumiera_lock_section_.lock =                                         \
             lumiera_recmutex_lock (mtx, &NOBUG_FLAG(nobugflag),                \
                                    &lumiera_lock_section_.rh, NOBUG_CONTEXT);  \
         lumiera_lock_section_.lock;                                            \
       });                                                                      \
       ({                                                                       \
         LUMIERA_RECMUTEX_SECTION_UNLOCK;                                       \
       }))


#define LUMIERA_RECMUTEX_SECTION_CHAIN(nobugflag, mtx)                                  \
  for (lumiera_sectionlock *lumiera_lock_section_old_ = &lumiera_lock_section_,         \
         NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked) lumiera_lock_section_ = {    \
         mtx, (lumiera_sectionlock_unlock_fn) lumiera_mutex_unlock                      \
           NOBUG_ALPHA_COMMA(&NOBUG_FLAG(nobugflag)) NOBUG_ALPHA_COMMA_NULL};           \
       ({                                                                               \
         if (lumiera_lock_section_.lock)                                                \
           {                                                                            \
             REQUIRE (lumiera_lock_section_old_->lock, "section prematurely unlocked"); \
             lumiera_lock_section_.lock =                                               \
               lumiera_recmutex_lock (mtx, &NOBUG_FLAG(nobugflag),                      \
                                      lumiera_lock_section_.rh, NOBUG_CONTEXT);         \
             LUMIERA_SECTION_UNLOCK_(lumiera_lock_section_old_);                        \
           }                                                                            \
         lumiera_lock_section_.lock;                                                    \
       });                                                                              \
       ({                                                                               \
         LUMIERA_RECMUTEX_SECTION_UNLOCK;                                               \
       }))


#define LUMIERA_RECMUTEX_SECTION_UNLOCK            \
  LUMIERA_SECTION_UNLOCK_(&lumiera_lock_section_)



/**
 * Recursive Mutex.
 *
 */
struct lumiera_recmutex_struct
{
  pthread_mutex_t recmutex;
  RESOURCE_HANDLE (rh);
};
typedef struct lumiera_recmutex_struct lumiera_recmutex;
typedef lumiera_recmutex* LumieraRecmutex;

/**
 * Initialise a recursive mutex variable
 * Initialises a 'recursive' mutex which might be locked by the same thread multiple times.
 * @param self is a pointer to the mutex to be initialised
 * @return self as given
 */
LumieraRecmutex
lumiera_recmutex_init (LumieraRecmutex self,
                       const char* purpose,
                       struct nobug_flag* flag,
                       const struct nobug_context ctx);

/**
 * Destroy a recursive mutex variable
 * @param self is a pointer to the mutex to be destroyed
 * @return self as given
 */
LumieraRecmutex
lumiera_recmutex_destroy (LumieraRecmutex self,
                          struct nobug_flag* flag,
                          const struct nobug_context ctx);


static inline LumieraRecmutex
lumiera_recmutex_lock (LumieraRecmutex self,
                       struct nobug_flag* flag,
                       struct nobug_resource_user** handle,
                       const struct nobug_context ctx)
{
  if (self)
    {
      NOBUG_RESOURCE_WAIT_CTX (NOBUG_FLAG_RAW(flag), self->rh, "acquire mutex", *handle, ctx)
        {
          if (pthread_mutex_lock (&self->recmutex))
            LUMIERA_DIE (LOCK_ACQUIRE);         /* never reached (in a correct program) */

          NOBUG_RESOURCE_STATE_CTX (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_RECURSIVE, *handle, ctx) /*{}*/;
        }
    }

  return self;
}


static inline LumieraRecmutex
lumiera_recmutex_trylock (LumieraRecmutex self,
                          struct nobug_flag* flag,
                          struct nobug_resource_user** handle,
                          const struct nobug_context ctx)
{
  if (self)
    {
      NOBUG_RESOURCE_TRY_CTX (NOBUG_FLAG_RAW(flag), self->rh, "try acquire mutex", *handle, ctx)
        {
          int err = pthread_mutex_trylock (&self->recmutex);

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


static inline LumieraRecmutex
lumiera_recmutex_timedlock (LumieraRecmutex self,
                            const struct timespec* timeout,
                            struct nobug_flag* flag,
                            struct nobug_resource_user** handle,
                            const struct nobug_context ctx)
{
  if (self)
    {
      NOBUG_RESOURCE_TRY_CTX (NOBUG_FLAG_RAW(flag), self->rh, "timed acquire mutex", *handle, ctx)
        {
          int err = pthread_mutex_timedlock (&self->recmutex, timeout);

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


static inline void
lumiera_recmutex_unlock (LumieraRecmutex self,
                         struct nobug_flag* flag,
                         struct nobug_resource_user** handle,
                         const struct nobug_context ctx)
{
  NOBUG_REQUIRE_CTX (self, ctx);

  NOBUG_RESOURCE_LEAVE_RAW_CTX (flag, *handle, ctx)
    {
      if (pthread_mutex_unlock (&self->recmutex))
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
