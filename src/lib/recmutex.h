/*
  recmutex.h  -  recursive mutal exclusion locking

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
#define LUMIERA_RECMUTEX_SECTION(nobugflag, mtx)                                                \
  for (lumiera_sectionlock NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked)                    \
         lumiera_lock_section_ = {                                                              \
         mtx, (lumiera_sectionlock_unlock_fn) lumiera_mutex_unlock                              \
           NOBUG_ALPHA_COMMA(&NOBUG_FLAG(nobugflag)) NOBUG_ALPHA_COMMA_NULL};                   \
       ({                                                                                       \
         if (lumiera_lock_section_.lock)                                                        \
           lumiera_lock_section_.lock =                                                         \
             lumiera_recmutex_lock (mtx, &NOBUG_FLAG(nobugflag), &lumiera_lock_section_.rh);    \
         lumiera_lock_section_.lock;                                                            \
       });                                                                                      \
       ({                                                                                       \
         LUMIERA_RECMUTEX_SECTION_UNLOCK;                                                       \
       }))


#define LUMIERA_RECMUTEX_SECTION_CHAIN(nobugflag, mtx)                                          \
  for (lumiera_sectionlock *lumiera_lock_section_old_ = &lumiera_lock_section_,                 \
         NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked) lumiera_lock_section_ = {            \
         mtx, (lumiera_sectionlock_unlock_fn) lumiera_mutex_unlock                              \
           NOBUG_ALPHA_COMMA(&NOBUG_FLAG(nobugflag)) NOBUG_ALPHA_COMMA_NULL};                   \
       ({                                                                                       \
         if (lumiera_lock_section_.lock)                                                        \
           {                                                                                    \
             REQUIRE (lumiera_lock_section_old_->lock, "section prematurely unlocked");         \
             lumiera_lock_section_.lock =                                                       \
               lumiera_recmutex_lock (mtx, &NOBUG_FLAG(nobugflag), lumiera_lock_section_.rh);   \
             LUMIERA_SECTION_UNLOCK_(lumiera_lock_section_old_);                                \
           }                                                                                    \
         lumiera_lock_section_.lock;                                                            \
       });                                                                                      \
       ({                                                                                       \
         LUMIERA_RECMUTEX_SECTION_UNLOCK;                                                       \
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
 * Initialize a recursive mutex variable
 * Initializes a 'recursive' mutex which might be locked by the same thread multiple times.
 * @param self is a pointer to the mutex to be initialized
 * @return self as given
 */
LumieraRecmutex
lumiera_recmutex_init (LumieraRecmutex self, const char* purpose, struct nobug_flag* flag);

/**
 * Destroy a recursive mutex variable
 * @param self is a pointer to the mutex to be destroyed
 * @return self as given
 */
LumieraRecmutex
lumiera_recmutex_destroy (LumieraRecmutex self, struct nobug_flag* flag);


static inline LumieraRecmutex
lumiera_recmutex_lock (LumieraRecmutex self, struct nobug_flag* flag, struct nobug_resource_user** handle)
{
  if (self)
    {
      RESOURCE_WAIT (NOBUG_FLAG_RAW(flag), self->rh, "acquire mutex", *handle);

      if (pthread_mutex_lock (&self->recmutex))
        LUMIERA_DIE (LOCK_ACQUIRE);         /* never reached (in a correct program) */

      RESOURCE_STATE (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_RECURSIVE, *handle);
    }

  return self;
}


static inline LumieraRecmutex
lumiera_recmutex_trylock (LumieraRecmutex self, struct nobug_flag* flag, struct nobug_resource_user** handle)
{
  if (self)
    {
      NOBUG_RESOURCE_TRY (NOBUG_FLAG_RAW(flag), self->rh, "try acquire mutex", *handle);

      int err = pthread_mutex_trylock (&self->recmutex);

      if (!err)
        {
          RESOURCE_STATE (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_RECURSIVE, *handle);
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


#ifndef LUMIERA_RESTRICT
# ifdef __cplusplus              /* C++ doesnt support restrict */
#  define LUMIERA_RESTRICT
# else
#  define LUMIERA_RESTRICT restrict
# endif
#endif


static inline LumieraRecmutex
lumiera_recmutex_timedlock (LumieraRecmutex self,
                            const struct timespec* LUMIERA_RESTRICT timeout,
                            struct nobug_flag* flag,
                            struct nobug_resource_user** handle)
{
  if (self)
    {
      NOBUG_RESOURCE_TRY (NOBUG_FLAG_RAW(flag), self->rh, "timed acquire mutex", *handle);

      int err = pthread_mutex_timedlock (&self->recmutex, timeout);

      if (!err)
        {
          RESOURCE_STATE (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_RECURSIVE, *handle);
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


static inline void
lumiera_recmutex_unlock (LumieraRecmutex self, struct nobug_flag* flag, struct nobug_resource_user** handle)
{
  REQUIRE (self);

  NOBUG_RESOURCE_LEAVE_RAW(flag, *handle)
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
