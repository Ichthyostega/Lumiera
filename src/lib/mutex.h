/*
  mutex.h  -  mutal exclusion locking

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

#ifndef LUMIERA_MUTEX_H
#define LUMIERA_MUTEX_H

#include "lib/error.h"

#include <pthread.h>
#include <nobug.h>

/**
 * @file
 * Mutual exclusion locking, header.
 */

LUMIERA_ERROR_DECLARE (MUTEX_LOCK);
LUMIERA_ERROR_DECLARE (MUTEX_UNLOCK);
LUMIERA_ERROR_DECLARE (MUTEX_DESTROY);

/**
 * Mutual exclusive section.
 */
#define LUMIERA_MUTEX_SECTION(nobugflag, mtx)                                                           \
  for (lumiera_mutexacquirer NOBUG_CLEANUP(lumiera_mutexacquirer_ensureunlocked) lumiera_mutex_section_ \
         = {(LumieraMutex)1 NOBUG_ALPHA_COMMA_NULL};                                                    \
       lumiera_mutex_section_.mutex;)                                                                   \
    for (                                                                                               \
         ({                                                                                             \
           lumiera_mutex_section_.mutex = (mtx);                                                        \
           NOBUG_RESOURCE_HANDLE_INIT (lumiera_mutex_section_.rh);                                      \
           RESOURCE_ENTER (nobugflag, (mtx)->rh, "acquire mutex", &lumiera_mutex_section_,              \
                           NOBUG_RESOURCE_EXCLUSIVE, lumiera_mutex_section_.rh);                        \
           if (pthread_mutex_lock (&(mtx)->mutex)) LUMIERA_DIE (MUTEX_LOCK);                            \
         });                                                                                            \
         lumiera_mutex_section_.mutex;                                                                  \
         ({                                                                                             \
           if (lumiera_mutex_section_.mutex)                                                            \
             {                                                                                          \
               pthread_mutex_unlock (&lumiera_mutex_section_.mutex->mutex);                             \
               lumiera_mutex_section_.mutex = NULL;                                                     \
               RESOURCE_LEAVE(nobugflag, lumiera_mutex_section_.rh);                                    \
             }                                                                                          \
         }))

/**
 * Mutual exclusion chainbuilder section.
 * Usage: LUMIERA_MUTEX_SECTION(a){LUMIERA_MUTEX_SECTION_CHAIN(b){run();}}
 *     calls lock(a); lock(b); unlock(a); run(); unlock(b);
 * This macro should only be used inside LUMIERA_MUTEX_SECTION and should be
 * called on the correct mutexes, period.
 */
#define LUMIERA_MUTEX_SECTION_CHAIN(nobugflag, mtx)                                                     \
  for (lumiera_mutexacquirer *lumiera_mutex_section_old_ = &lumiera_mutex_section_,                     \
         NOBUG_CLEANUP(lumiera_mutexacquirer_ensureunlocked) lumiera_mutex_section_ = {(LumieraMutex)1  \
           NOBUG_ALPHA_COMMA_NULL};                                                                     \
       lumiera_mutex_section_.mutex;)                                                                   \
    for (                                                                                               \
         ({                                                                                             \
           lumiera_mutex_section_.mutex = (mtx);                                                        \
           NOBUG_RESOURCE_HANDLE_INIT (lumiera_mutex_section_.rh);                                      \
           RESOURCE_ENTER (nobugflag, (mtx)->rh, "acquire mutex", &lumiera_mutex_section_,              \
                           NOBUG_RESOURCE_EXCLUSIVE, lumiera_mutex_section_.rh);                        \
           if (pthread_mutex_lock (&(mtx)->mutex)) LUMIERA_DIE (MUTEX_LOCK);                            \
           if (lumiera_mutex_section_old_->mutex)                                                       \
             {                                                                                          \
               pthread_mutex_unlock (&lumiera_mutex_section_old_->mutex->mutex);                        \
               lumiera_mutex_section_old_->mutex = NULL;                                                \
               RESOURCE_LEAVE(nobugflag, lumiera_mutex_section_old_->rh);                               \
             }                                                                                          \
         });                                                                                            \
         lumiera_mutex_section_.mutex;                                                                  \
         ({                                                                                             \
           if (lumiera_mutex_section_.mutex)                                                            \
             {                                                                                          \
               pthread_mutex_unlock (&lumiera_mutex_section_.mutex->mutex);                             \
               lumiera_mutex_section_.mutex = NULL;                                                     \
               RESOURCE_LEAVE(nobugflag, lumiera_mutex_section_.rh);                                    \
             }                                                                                          \
         }))


/**
 * Recursive Mutual exclusive section.
 */
#define LUMIERA_RECMUTEX_SECTION(nobugflag, mtx)                                                        \
  for (lumiera_mutexacquirer NOBUG_CLEANUP(lumiera_mutexacquirer_ensureunlocked) lumiera_mutex_section_ \
         = {(LumieraMutex)1 NOBUG_ALPHA_COMMA_NULL};                                                    \
       lumiera_mutex_section_.mutex;)                                                                   \
    for (                                                                                               \
         ({                                                                                             \
           lumiera_mutex_section_.mutex = (mtx);                                                        \
           NOBUG_RESOURCE_HANDLE_INIT (lumiera_mutex_section_.rh);                                      \
           RESOURCE_ENTER (nobugflag, (mtx)->rh, "acquire recmutex", &lumiera_mutex_section_,           \
                           NOBUG_RESOURCE_RECURSIVE, lumiera_mutex_section_.rh);                        \
           if (pthread_mutex_lock (&(mtx)->mutex)) LUMIERA_DIE (MUTEX_LOCK);                            \
         });                                                                                            \
         lumiera_mutex_section_.mutex;                                                                  \
         ({                                                                                             \
           if (lumiera_mutex_section_.mutex)                                                            \
             {                                                                                          \
               pthread_mutex_unlock (&lumiera_mutex_section_.mutex->mutex);                             \
               lumiera_mutex_section_.mutex = NULL;                                                     \
               RESOURCE_LEAVE(nobugflag, lumiera_mutex_section_.rh);                                    \
             }                                                                                          \
         }))


/**
 * Mutual exclusion chainbuilder section.
 * Usage: LUMIERA_MUTEX_SECTION(a){LUMIERA_RECMUTEX_SECTION_CHAIN(b){run();}}
 *     calls lock(a); lock(b); unlock(a); run(); unlock(b);
 * This macro should only be used inside LUMIERA_MUTEX_SECTION and should be
 * called on the correct mutexes, period.
 */
#define LUMIERA_RECMUTEX_SECTION_CHAIN(nobugflag, mtx)                                                  \
  for (lumiera_mutexacquirer *lumiera_mutex_section_old_ = &lumiera_mutex_section_,                     \
         NOBUG_CLEANUP(lumiera_mutexacquirer_ensureunlocked) lumiera_mutex_section_ = {(LumieraMutex)1  \
           NOBUG_ALPHA_COMMA_NULL};                                                                     \
       lumiera_mutex_section_.mutex;)                                                                   \
    for (                                                                                               \
         ({                                                                                             \
           lumiera_mutex_section_.mutex = (mtx);                                                        \
           NOBUG_RESOURCE_HANDLE_INIT (lumiera_mutex_section_.rh);                                      \
           RESOURCE_ENTER (nobugflag, (mtx)->rh, "acquire recmutex", &lumiera_mutex_section_,           \
                           NOBUG_RESOURCE_RECURSIVE, lumiera_mutex_section_.rh);                        \
           if (pthread_mutex_lock (&(mtx)->mutex)) LUMIERA_DIE (MUTEX_LOCK);                            \
           if (lumiera_mutex_section_old_->mutex)                                                       \
             {                                                                                          \
               pthread_mutex_unlock (&lumiera_mutex_section_old_->mutex->mutex);                        \
               lumiera_mutex_section_old_->mutex = NULL;                                                \
               RESOURCE_LEAVE(nobugflag, lumiera_mutex_section_old_->rh);                               \
             }                                                                                          \
         });                                                                                            \
         lumiera_mutex_section_.mutex;                                                                  \
         ({                                                                                             \
           if (lumiera_mutex_section_.mutex)                                                            \
             {                                                                                          \
               pthread_mutex_unlock (&lumiera_mutex_section_.mutex->mutex);                             \
               lumiera_mutex_section_.mutex = NULL;                                                     \
               RESOURCE_LEAVE(nobugflag, lumiera_mutex_section_.rh);                                    \
             }                                                                                          \
         }))


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
 * Initialize a mutex variable
 * Initializes a 'recursive' mutex which might be locked by the same thread multiple times.
 * @param self is a pointer to the mutex to be initialized
 * @return self as given
 */
LumieraMutex
lumiera_recmutex_init (LumieraMutex self, const char* purpose, struct nobug_flag* flag);


/**
 * Destroy a mutex variable
 * @param self is a pointer to the mutex to be destroyed
 * @return self as given
 */
LumieraMutex
lumiera_mutex_destroy (LumieraMutex self, struct nobug_flag* flag);


/**
 * mutexacquirer used to manage the state of a mutex.
 */
struct lumiera_mutexacquirer_struct
{
  LumieraMutex mutex;
  RESOURCE_HANDLE (rh);
};
typedef struct lumiera_mutexacquirer_struct lumiera_mutexacquirer;
typedef struct lumiera_mutexacquirer_struct* LumieraMutexacquirer;

/* helper function for nobug */
static inline void
lumiera_mutexacquirer_ensureunlocked (LumieraMutexacquirer self)
{
  ENSURE (!self->mutex, "forgot to unlock mutex");
}

#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
