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

#include <pthread.h>
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
         (void*)1, lumiera_mutex_unlock_cb NOBUG_ALPHA_COMMA_NULL NOBUG_ALPHA_COMMA_NULL};      \
       lumiera_lock_section_.lock;)                                                             \
    for (                                                                                       \
         ({                                                                                     \
           lumiera_lock_section_.lock = (mtx);                                                  \
           NOBUG_IF_ALPHA(lumiera_lock_section_.flag = &NOBUG_FLAG(nobugflag);)                 \
           RESOURCE_WAIT (nobugflag, (mtx)->rh, "acquire mutex", lumiera_lock_section_.rh)      \
             {                                                                                  \
               if (pthread_mutex_lock (&(mtx)->mutex))                                          \
                 LUMIERA_DIE (LOCK_ACQUIRE);                                                    \
               RESOURCE_STATE (nobugflag, NOBUG_RESOURCE_EXCLUSIVE, lumiera_lock_section_.rh);  \
             }                                                                                  \
         });                                                                                    \
         lumiera_lock_section_.lock;                                                            \
         ({                                                                                     \
           LUMIERA_MUTEX_SECTION_UNLOCK;                                                        \
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
         (void*)1, lumiera_mutex_unlock_cb NOBUG_ALPHA_COMMA_NULL NOBUG_ALPHA_COMMA_NULL};      \
       lumiera_lock_section_.lock;)                                                             \
    for (                                                                                       \
         ({                                                                                     \
           REQUIRE (lumiera_lock_section_old_->lock, "section prematurely unlocked");           \
           lumiera_lock_section_.lock = mtx;                                                    \
           NOBUG_IF_ALPHA(lumiera_lock_section_.flag = &NOBUG_FLAG(nobugflag);)                 \
           RESOURCE_WAIT (nobugflag, (mtx)->rh, "acquire mutex", lumiera_lock_section_.rh)      \
             {                                                                                  \
               if (pthread_mutex_lock (&(mtx)->mutex))                                          \
                 LUMIERA_DIE (LOCK_ACQUIRE);                                                    \
               RESOURCE_STATE (nobugflag, NOBUG_RESOURCE_EXCLUSIVE, lumiera_lock_section_.rh);  \
               LUMIERA_SECTION_UNLOCK_(lumiera_lock_section_old_);                              \
             }                                                                                  \
         });                                                                                    \
         lumiera_lock_section_.lock;                                                            \
         ({                                                                                     \
           LUMIERA_MUTEX_SECTION_UNLOCK;                                                        \
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
 * @return self as given
 */
LumieraMutex
lumiera_mutex_init (LumieraMutex self, const char* purpose, struct nobug_flag* flag);

/**
 * Destroy a mutex variable
 * @param self is a pointer to the mutex to be destroyed
 * @return self as given
 */
LumieraMutex
lumiera_mutex_destroy (LumieraMutex self, struct nobug_flag* flag);


/**
 * Callback for unlocking mutexes.
 * @internal
 */
int
lumiera_mutex_unlock_cb (void* mutex);


#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
