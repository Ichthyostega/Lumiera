/*
  rwlock.h  -  read/write locks

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

#ifndef LUMIERA_RWLOCK_H
#define LUMIERA_RWLOCK_H

#ifndef _GNU_SOURCE
#error "This header must be included with _GNU_SOURCE or _POSIX_C_SOURCE >= 200112L defined"
#endif

#include <pthread.h>
#include <nobug.h>

LUMIERA_ERROR_DECLARE(RWLOCK_AGAIN);
LUMIERA_ERROR_DECLARE(RWLOCK_DEADLOCK);
LUMIERA_ERROR_DECLARE(RWLOCK_DESTROY);
LUMIERA_ERROR_DECLARE(RWLOCK_UNLOCK);
LUMIERA_ERROR_DECLARE(RWLOCK_RDLOCK);
LUMIERA_ERROR_DECLARE(RWLOCK_WRLOCK);

/**
 * @file
 * Read/write locks, header.
 */


/**
 * Read locked section.
 */
#define LUMIERA_RDLOCK_SECTION(nobugflag, rwlck)                                                \
  for (lumiera_sectionlock NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked)                    \
         lumiera_lock_section_ = {                                                              \
         (void*)1, lumiera_rwlock_unlock_cb NOBUG_ALPHA_COMMA_NULL NOBUG_ALPHA_COMMA_NULL};     \
       lumiera_lock_section_.lock;)                                                             \
    for (                                                                                       \
         ({                                                                                     \
           lumiera_lock_section_.lock = &(rwlck)->rwlock;                                       \
           NOBUG_IF_ALPHA(lumiera_lock_section_.flag = &NOBUG_FLAG(nobugflag);)                 \
             RESOURCE_ENTER (nobugflag, (rwlck)->rh, "acquire readlock",                        \
                             &lumiera_lock_section_, NOBUG_RESOURCE_WAITING,                    \
                             lumiera_lock_section_.rh);                                         \
           if (pthread_rwlock_rdlock ((pthread_rwlock_t*)lumiera_lock_section_.lock))           \
             LUMIERA_DIE (LOCK_ACQUIRE);                                                        \
           TODO ("implement NOBUG_RESOURCE_SHARED");                                            \
           /*RESOURCE_STATE (nobugflag, NOBUG_RESOURCE_SHARED, lumiera_lock_section_.rh); */ \
         });                                                                                    \
         lumiera_lock_section_.lock;                                                            \
         ({                                                                                     \
           LUMIERA_RWLOCK_SECTION_UNLOCK;                                                       \
         }))


#define LUMIERA_RDLOCK_SECTION_CHAIN(nobugflag, rwlck)                                          \
  for (lumiera_sectionlock *lumiera_lock_section_old_ = &lumiera_lock_section_,                 \
         NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked) lumiera_lock_section_ = {            \
         (void*)1, lumiera_rwlock_unlock_cb NOBUG_ALPHA_COMMA_NULL NOBUG_ALPHA_COMMA_NULL};     \
       lumiera_lock_section_.lock;)                                                             \
    for (                                                                                       \
         ({                                                                                     \
           lumiera_lock_section_.lock = &(rwlck)->rwlock;                                       \
           NOBUG_IF_ALPHA(lumiera_lock_section_.flag = &NOBUG_FLAG(nobugflag);)                 \
           RESOURCE_ENTER (nobugflag, (rwlck)->rh, "acquire readlock", &lumiera_lock_section_,  \
                           NOBUG_RESOURCE_WAITING, lumiera_lock_section_.rh);                   \
           if (pthread_rwlock_rdlock ((pthread_rwlock_t*)lumiera_lock_section_.lock))           \
             LUMIERA_DIE (LOCK_ACQUIRE);                                                        \
           /*RESOURCE_STATE (nobugflag, NOBUG_RESOURCE_SHARED, lumiera_lock_section_.rh); */ \
           LUMIERA_SECTION_UNLOCK_(lumiera_lock_section_old_);                                  \
         });                                                                                    \
         lumiera_lock_section_.lock;                                                            \
         ({                                                                                     \
           LUMIERA_MUTEX_SECTION_UNLOCK;                                                        \
         }))




/**
 * Write locked section.
 */
#define LUMIERA_WRLOCK_SECTION(nobugflag, rwlck)                                                \
  for (lumiera_sectionlock NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked)                    \
         lumiera_lock_section_ = {                                                              \
         (void*)1, lumiera_rwlock_unlock_cb NOBUG_ALPHA_COMMA_NULL NOBUG_ALPHA_COMMA_NULL};     \
       lumiera_lock_section_.lock;)                                                             \
    for (                                                                                       \
         ({                                                                                     \
           lumiera_lock_section_.lock = &(rwlck)->rwlock;                                       \
           NOBUG_IF_ALPHA(lumiera_lock_section_.flag = &NOBUG_FLAG(nobugflag);)                 \
             RESOURCE_ENTER (nobugflag, (rwlck)->rh, "acquire writelock",                       \
                             &lumiera_lock_section_, NOBUG_RESOURCE_WAITING,                    \
                             lumiera_lock_section_.rh);                                         \
           if (pthread_rwlock_wrlock ((pthread_rwlock_t*)lumiera_lock_section_.lock))           \
             LUMIERA_DIE (LOCK_ACQUIRE);                                                        \
           RESOURCE_STATE (nobugflag, NOBUG_RESOURCE_EXCLUSIVE, lumiera_lock_section_.rh);      \
         });                                                                                    \
         lumiera_lock_section_.lock;                                                            \
         ({                                                                                     \
           LUMIERA_RWLOCK_SECTION_UNLOCK;                                                       \
         }))


#define LUMIERA_WRLOCK_SECTION_CHAIN(nobugflag, rwlck)                                          \
  for (lumiera_sectionlock *lumiera_lock_section_old_ = &lumiera_lock_section_,                 \
         NOBUG_CLEANUP(lumiera_sectionlock_ensureunlocked) lumiera_lock_section_ = {            \
         (void*)1, lumiera_rwlock_unlock_cb NOBUG_ALPHA_COMMA_NULL NOBUG_ALPHA_COMMA_NULL};     \
       lumiera_lock_section_.lock;)                                                             \
    for (                                                                                       \
         ({                                                                                     \
           lumiera_lock_section_.lock = &(rwlck)->rwlock;                                       \
           NOBUG_IF_ALPHA(lumiera_lock_section_.flag = &NOBUG_FLAG(nobugflag);)                 \
           RESOURCE_ENTER (nobugflag, (rwlck)->rh, "acquire writelock", &lumiera_lock_section_, \
                           NOBUG_RESOURCE_WAITING, lumiera_lock_section_.rh);                   \
           if (pthread_rwlock_wrlock ((pthread_rwlock_t*)lumiera_lock_section_.lock))           \
             LUMIERA_DIE (LOCK_ACQUIRE);                                                        \
           RESOURCE_STATE (nobugflag, NOBUG_RESOURCE_EXCLUSIVE, lumiera_lock_section_.rh);      \
           LUMIERA_SECTION_UNLOCK_(lumiera_lock_section_old_);                                  \
         });                                                                                    \
         lumiera_lock_section_.lock;                                                            \
         ({                                                                                     \
           LUMIERA_MUTEX_SECTION_UNLOCK;                                                        \
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
lumiera_rwlock_init (LumieraRWLock self, const char* purpose, struct nobug_flag* flag);

/**
 * destroy a rwlock
 * @param self is a pointer to the rwlock to be initialized
 * @return self on success or NULL at error
 */
LumieraRWLock
lumiera_rwlock_destroy (LumieraRWLock self, struct nobug_flag* flag);


int
lumiera_rwlock_unlock_cb (void* rwlock);


#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
