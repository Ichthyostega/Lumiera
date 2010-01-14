/*
  mutex.h  -  mutal exclusion locking

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

#ifndef LUMIERA_MUTEX_H
#define LUMIERA_MUTEX_H

#include "lib/error.h"
#include "lib/sectionlock.h"
#include "lib/lockerror.h"

#include <pthread.h>
#include <time.h>
#include <nobug.h>

/**
 * @file
 * Mutual exclusion locking, header.
 */

/**
 * Mutual exclusive section.
 */
#define LUMIERA_MUTEX_SECTION(nobugflag, mtx)                                                   \
  for (lumiera_sectionlock NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked)                    \
         lumiera_lock_section_ = {                                                              \
         mtx, (lumiera_sectionlock_unlock_fn) lumiera_mutex_unlock                              \
           NOBUG_ALPHA_COMMA(&NOBUG_FLAG(nobugflag)) NOBUG_ALPHA_COMMA_NULL};                   \
       ({                                                                                       \
         if (lumiera_lock_section_.lock)                                                        \
           lumiera_lock_section_.lock =                                                         \
             lumiera_mutex_lock (mtx, &NOBUG_FLAG(nobugflag), &lumiera_lock_section_.rh);       \
         lumiera_lock_section_.lock;                                                            \
       });                                                                                      \
       ({                                                                                       \
         LUMIERA_MUTEX_SECTION_UNLOCK;                                                          \
       }))


/**
 * Mutual exclusion chainbuilder section.
 * Usage: LUMIERA_MUTEX_SECTION(a){LUMIERA_MUTEX_SECTION_CHAIN(b){run();}}
 *     calls lock(a); lock(b); unlock(a); run(); unlock(b);
 * This macro should only be used inside LUMIERA_MUTEX_SECTION and should be
 * called on the correct mutexes, period.
 */
#define LUMIERA_MUTEX_SECTION_CHAIN(nobugflag, mtx)                                             \
  for (lumiera_sectionlock *lumiera_lock_section_old_ = &lumiera_lock_section_,                 \
         NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked) lumiera_lock_section_ = {            \
         mtx, (lumiera_sectionlock_unlock_fn) lumiera_mutex_unlock                              \
           NOBUG_ALPHA_COMMA(&NOBUG_FLAG(nobugflag)) NOBUG_ALPHA_COMMA_NULL};                   \
       ({                                                                                       \
         if (lumiera_lock_section_.lock)                                                        \
           {                                                                                    \
             REQUIRE (lumiera_lock_section_old_->lock, "section prematurely unlocked");         \
             lumiera_lock_section_.lock =                                                       \
               lumiera_mutex_lock (mtx, &NOBUG_FLAG(nobugflag), &lumiera_lock_section_.rh);     \
             LUMIERA_SECTION_UNLOCK_(lumiera_lock_section_old_);                                \
           }                                                                                    \
         lumiera_lock_section_.lock;                                                            \
       });                                                                                      \
       ({                                                                                       \
         LUMIERA_MUTEX_SECTION_UNLOCK;                                                          \
       }))


#define LUMIERA_MUTEX_SECTION_UNLOCK            \
  LUMIERA_SECTION_UNLOCK_(&lumiera_lock_section_)


/**
 * Mutex.
 *
 */
struct lumiera_mutex_struct
{
  pthread_mutex_t mutex;
  RESOURCE_HANDLE (rh);
};
typedef struct lumiera_mutex_struct lumiera_mutex;
typedef lumiera_mutex* LumieraMutex;


/**
 * Initialize a mutex variable
 * This initializes a 'fast' default mutex which must not be locked recursively from one thread.
 * @param self is a pointer to the mutex to be initialized
 * @param purpose textual hint for the nobug resourcetracker
 * @param flag nobug logging target
 * @return self as given
 */
LumieraMutex
lumiera_mutex_init (LumieraMutex self, const char* purpose, struct nobug_flag* flag);

/**
 * Destroy a mutex variable
 * @param self is a pointer to the mutex to be destroyed
 * @param flag nobug logging target
 * @return self as given
 */
LumieraMutex
lumiera_mutex_destroy (LumieraMutex self, struct nobug_flag* flag);


/**
 * Lock a mutex variable
 * Never fails
 * @param self is a pointer to the mutex to be destroyed
 * @param flag nobug logging target
 * @param handle pointer to nobug user handle (NULL in beta and release builds)
 * @return self as given
 */
static inline LumieraMutex
lumiera_mutex_lock (LumieraMutex self, struct nobug_flag* flag, struct nobug_resource_user** handle)
{
  if (self)
    {
      RESOURCE_WAIT (NOBUG_FLAG_RAW(flag), self->rh, "acquire mutex", *handle);

      if (pthread_mutex_lock (&self->mutex))
        LUMIERA_DIE (LOCK_ACQUIRE);         /* never reached (in a correct program) */

      RESOURCE_STATE (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_EXCLUSIVE, *handle);
    }

  return self;
}


/**
 * Try to lock a mutex variable
 * @param self is a pointer to the mutex to be destroyed
 * @param flag nobug logging target
 * @param handle pointer to nobug user handle (NULL in beta and release builds)
 * @return self as given or NULL on error, lumiera_error gets set approbiately
 */
static inline LumieraMutex
lumiera_mutex_trylock (LumieraMutex self, struct nobug_flag* flag, struct nobug_resource_user** handle)
{
  if (self)
    {
      NOBUG_RESOURCE_TRY (NOBUG_FLAG_RAW(flag), self->rh, "try acquire mutex", *handle);

      int err = pthread_mutex_trylock (&self->mutex);

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

/**
 * Try to lock a mutex variable with a timeout
 * @param self is a pointer to the mutex to be destroyed
 * @param timeout timeout for waiting for the lock
 * @param flag nobug logging target
 * @param handle pointer to nobug user handle (NULL in beta and release builds)
 * @return self as given or NULL on error, lumiera_error gets set approbiately
 */
static inline LumieraMutex
lumiera_mutex_timedlock (LumieraMutex self,
                         const struct timespec* timeout,
                         struct nobug_flag* flag,
                         struct nobug_resource_user** handle)
{
  if (self)
    {
      NOBUG_RESOURCE_TRY (NOBUG_FLAG_RAW(flag), self->rh, "timed acquire mutex", *handle);

      int err = pthread_mutex_timedlock (&self->mutex, timeout);

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


/**
 * unlock a mutex variable
 * Never fails
 * @param self is a pointer to the mutex to be destroyed
 * @param flag nobug logging target
 * @param handle pointer to nobug user handle (NULL in beta and release builds)
 */
static inline void
lumiera_mutex_unlock (LumieraMutex self, struct nobug_flag* flag, struct nobug_resource_user** handle)
{
  REQUIRE(self);

  NOBUG_RESOURCE_LEAVE_RAW(flag, *handle)
    {
      if (pthread_mutex_unlock (&self->mutex))
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
