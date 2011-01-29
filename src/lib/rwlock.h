/*
  rwlock.h  -  read/write locks

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

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

#ifndef LUMIERA_RWLOCK_H
#define LUMIERA_RWLOCK_H

#ifndef _GNU_SOURCE
#error "This header must be included with _GNU_SOURCE or _POSIX_C_SOURCE >= 200112L defined"
#endif

#include "lib/sectionlock.h"
#include "lib/lockerror.h"

#include <pthread.h>
#include <time.h>
#include <nobug.h>


/**
 * @file
 * Read/write locks, header.
 */


/**
 * Read locked section.
 * readlocks may fail when there are too much readers, one has to check the error afterwards!
 */
#define LUMIERA_RDLOCK_SECTION(nobugflag, rwlck)                                \
  for (lumiera_sectionlock NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked)    \
         lumiera_lock_section_ = {                                              \
         rwlck, (lumiera_sectionlock_unlock_fn) lumiera_rwlock_unlock           \
           NOBUG_ALPHA_COMMA(&NOBUG_FLAG(nobugflag)) NOBUG_ALPHA_COMMA_NULL};   \
       ({                                                                       \
         if (lumiera_lock_section_.lock)                                        \
           lumiera_lock_section_.lock =                                         \
             lumiera_rwlock_rdlock (rwlck, &NOBUG_FLAG(nobugflag),              \
                                    &lumiera_lock_section_.rh, NOBUG_CONTEXT);  \
         lumiera_lock_section_.lock;                                            \
       });                                                                      \
       ({                                                                       \
         LUMIERA_RWLOCK_SECTION_UNLOCK;                                         \
       }))


#define LUMIERA_RDLOCK_SECTION_CHAIN(nobugflag, rwlck)                                  \
  for (lumiera_sectionlock *lumiera_lock_section_old_ = &lumiera_lock_section_,         \
         NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked) lumiera_lock_section_ = {    \
         rwlck, (lumiera_sectionlock_unlock_fn) lumiera_rwlock_unlock                   \
           NOBUG_ALPHA_COMMA(&NOBUG_FLAG(nobugflag)) NOBUG_ALPHA_COMMA_NULL};           \
       ({                                                                               \
         if (lumiera_lock_section_.lock)                                                \
           {                                                                            \
             REQUIRE (lumiera_lock_section_old_->lock, "section prematurely unlocked"); \
             lumiera_lock_section_.lock =                                               \
               lumiera_rwlock_rdlock (rwlck, &NOBUG_FLAG(nobugflag),                    \
                                      lumiera_lock_section_.rh, NOBUG_CONTEXT);         \
             LUMIERA_SECTION_UNLOCK_(lumiera_lock_section_old_);                        \
           }                                                                            \
         lumiera_lock_section_.lock;                                                    \
       });                                                                              \
       ({                                                                               \
         LUMIERA_RWLOCK_SECTION_UNLOCK;                                                 \
       }))



/**
 * Write locked section.
 */
#define LUMIERA_WRLOCK_SECTION(nobugflag, rwlck)                                \
  for (lumiera_sectionlock NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked)    \
         lumiera_lock_section_ = {                                              \
         rwlck, (lumiera_sectionlock_unlock_fn) lumiera_rwlock_unlock           \
           NOBUG_ALPHA_COMMA(&NOBUG_FLAG(nobugflag)) NOBUG_ALPHA_COMMA_NULL};   \
       ({                                                                       \
         if (lumiera_lock_section_.lock)                                        \
           lumiera_lock_section_.lock =                                         \
             lumiera_rwlock_wrlock (rwlck, &NOBUG_FLAG(nobugflag),              \
                                    &lumiera_lock_section_.rh, NOBUG_CONTEXT);  \
         lumiera_lock_section_.lock;                                            \
       });                                                                      \
       ({                                                                       \
         LUMIERA_RWLOCK_SECTION_UNLOCK;                                         \
       }))


#define LUMIERA_WRLOCK_SECTION_CHAIN(nobugflag, rwlck)                                  \
  for (lumiera_sectionlock *lumiera_lock_section_old_ = &lumiera_lock_section_,         \
         NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked) lumiera_lock_section_ = {    \
         rwlck, (lumiera_sectionlock_unlock_fn) lumiera_rwlock_unlock                   \
           NOBUG_ALPHA_COMMA(&NOBUG_FLAG(nobugflag)) NOBUG_ALPHA_COMMA_NULL};           \
       ({                                                                               \
         if (lumiera_lock_section_.lock)                                                \
           {                                                                            \
             REQUIRE (lumiera_lock_section_old_->lock, "section prematurely unlocked"); \
             lumiera_lock_section_.lock =                                               \
               lumiera_rwlock_wrlock (rwlck, &NOBUG_FLAG(nobugflag),                    \
                                      lumiera_lock_section_.rh, NOBUG_CONTEXT);         \
             LUMIERA_SECTION_UNLOCK_(lumiera_lock_section_old_);                        \
           }                                                                            \
         lumiera_lock_section_.lock;                                                    \
       });                                                                              \
       ({                                                                               \
         LUMIERA_RWLOCK_SECTION_UNLOCK;                                                 \
       }))


#define LUMIERA_RWLOCK_SECTION_UNLOCK            \
  LUMIERA_SECTION_UNLOCK_(&lumiera_lock_section_)


/**
 * RWLock.
 *
 */
struct lumiera_rwlock_struct
{
  pthread_rwlock_t rwlock;
  RESOURCE_HANDLE (rh);
};
typedef struct lumiera_rwlock_struct lumiera_rwlock;
typedef lumiera_rwlock* LumieraRWLock;

/**
 * Initialize a rwlock
 * @param self is a pointer to the rwlock to be initialized
 * @return self as given
 */
LumieraRWLock
lumiera_rwlock_init (LumieraRWLock self,
                     const char* purpose,
                     struct nobug_flag* flag,
                     const struct nobug_context ctx);

/**
 * destroy a rwlock
 * @param self is a pointer to the rwlock to be initialized
 * @return self on success or NULL at error
 */
LumieraRWLock
lumiera_rwlock_destroy (LumieraRWLock self,
                        struct nobug_flag* flag,
                        const struct nobug_context ctx);


static inline LumieraRWLock
lumiera_rwlock_rdlock (LumieraRWLock self,
                       struct nobug_flag* flag,
                       struct nobug_resource_user** handle,
                       const struct nobug_context ctx)
{
  if (self)
    {
      NOBUG_RESOURCE_WAIT_CTX (NOBUG_FLAG_RAW(flag), self->rh, "acquire rwlock for reading", *handle, ctx)
        {
          int err = pthread_rwlock_rdlock (&self->rwlock);

          if (!err)
            {
              NOBUG_RESOURCE_STATE_CTX (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_SHARED, *handle, ctx) /*{}*/;
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


static inline LumieraRWLock
lumiera_rwlock_tryrdlock (LumieraRWLock self,
                          struct nobug_flag* flag,
                          struct nobug_resource_user** handle,
                          const struct nobug_context ctx)
{
  if (self)
    {
      NOBUG_RESOURCE_TRY_CTX (NOBUG_FLAG_RAW(flag), self->rh, "try acquire rwlock for reading", *handle, ctx)
        {
          int err = pthread_rwlock_tryrdlock (&self->rwlock);

          if (!err)
            {
              NOBUG_RESOURCE_STATE_CTX (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_SHARED, *handle, ctx) /*{}*/;
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


static inline LumieraRWLock
lumiera_rwlock_timedrdlock (LumieraRWLock self,
                            const struct timespec* timeout,
                            struct nobug_flag* flag,
                            struct nobug_resource_user** handle,
                            const struct nobug_context ctx)
{
  if (self)
    {
      NOBUG_RESOURCE_TRY_CTX (NOBUG_FLAG_RAW(flag), self->rh, "timed acquire rwlock for reading", *handle, ctx)
        {
          int err = pthread_rwlock_timedrdlock (&self->rwlock, timeout);

          if (!err)
            {
              NOBUG_RESOURCE_STATE_CTX (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_SHARED, *handle, ctx) /*{}*/;
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


static inline LumieraRWLock
lumiera_rwlock_wrlock (LumieraRWLock self,
                       struct nobug_flag* flag,
                       struct nobug_resource_user** handle,
                       const struct nobug_context ctx)
{
  if (self)
    {
      NOBUG_RESOURCE_WAIT_CTX (NOBUG_FLAG_RAW(flag), self->rh, "acquire rwlock for writing", *handle, ctx)
        {
          if (pthread_rwlock_wrlock (&self->rwlock))
            LUMIERA_DIE (LOCK_ACQUIRE);                 /* never reached (in a correct program) */

          NOBUG_RESOURCE_STATE_CTX (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_EXCLUSIVE, *handle, ctx) /*{}*/;
        }
    }

  return self;
}


static inline LumieraRWLock
lumiera_rwlock_trywrlock (LumieraRWLock self,
                          struct nobug_flag* flag,
                          struct nobug_resource_user** handle,
                          const struct nobug_context ctx)
{
  if (self)
    {
      NOBUG_RESOURCE_TRY_CTX (NOBUG_FLAG_RAW(flag), self->rh, "try acquire rwlock for writing", *handle, ctx)
        {
          int err = pthread_rwlock_trywrlock (&self->rwlock);

          if (!err)
            {
              NOBUG_RESOURCE_STATE_CTX (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_EXCLUSIVE, *handle, ctx) /*{}*/;
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


static inline LumieraRWLock
lumiera_rwlock_timedwrlock (LumieraRWLock self,
                            const struct timespec* timeout,
                            struct nobug_flag* flag,
                            struct nobug_resource_user** handle,
                            const struct nobug_context ctx)
{
  if (self)
    {
      NOBUG_RESOURCE_TRY_CTX (NOBUG_FLAG_RAW(flag), self->rh, "timed acquire rwlock for writing", *handle, ctx)
        {
          int err = pthread_rwlock_timedwrlock (&self->rwlock, timeout);

          if (!err)
            {
              NOBUG_RESOURCE_STATE_CTX (NOBUG_FLAG_RAW(flag), NOBUG_RESOURCE_EXCLUSIVE, *handle, ctx) /*{}*/;
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
lumiera_rwlock_unlock (LumieraRWLock self,
                       struct nobug_flag* flag,
                       struct nobug_resource_user** handle,
                       const struct nobug_context ctx)
{
  NOBUG_REQUIRE_CTX (self, ctx);

  NOBUG_RESOURCE_LEAVE_RAW_CTX (flag, *handle, ctx)
    {
      if (pthread_rwlock_unlock (&self->rwlock))
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
