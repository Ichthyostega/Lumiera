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
//#include <errno.h>
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
#define LUMIERA_RDLOCK_SECTION(nobugflag, rwlck)                                                                                        \
  for (lumiera_rwlockacquirer NOBUG_CLEANUP(lumiera_rwlockacquirer_ensureunlocked) lumiera_rwlock_section_ = {(LumieraRWLock)1};        \
       lumiera_rwlock_section_.rwlock;)                                                                                                 \
    for (                                                                                                                               \
         ({                                                                                                                             \
           lumiera_rwlock_section_.rwlock = (rwlck);                                                                                    \
           NOBUG_RESOURCE_HANDLE_INIT (lumiera_rwlock_section_.rh);                                                                     \
           RESOURCE_ENTER (nobugflag, (rwlck)->rh, "acquire rwlock for reading", &lumiera_rwlock_section_,                              \
                           NOBUG_RESOURCE_EXCLUSIVE, lumiera_rwlock_section_.rh);                                                       \
           if (pthread_rwlock_rdlock (&(rwlck)->rwlock)) LUMIERA_DIE (RWLOCK_RDLOCK);                                                   \
         });                                                                                                                            \
         lumiera_rwlock_section_.rwlock;                                                                                                \
         ({                                                                                                                             \
           if (lumiera_rwlock_section_.rwlock)                                                                                          \
             {                                                                                                                          \
               pthread_rwlock_unlock (&lumiera_rwlock_section_.rwlock->rwlock);                                                         \
               lumiera_rwlock_section_.rwlock = NULL;                                                                                   \
               RESOURCE_LEAVE(nobugflag, lumiera_rwlock_section_.rh);                                                                   \
             }                                                                                                                          \
         }))


/**
 * Write locked section.
 */
#define LUMIERA_WRLOCK_SECTION(nobugflag, rwlck)                                                                                        \
  for (lumiera_rwlockacquirer NOBUG_CLEANUP(lumiera_rwlockacquirer_ensureunlocked) lumiera_rwlock_section_ = {(LumieraRWLock)1};        \
       lumiera_rwlock_section_.rwlock;)                                                                                                 \
    for (                                                                                                                               \
         ({                                                                                                                             \
           lumiera_rwlock_section_.rwlock = (rwlck);                                                                                    \
           NOBUG_RESOURCE_HANDLE_INIT (lumiera_rwlock_section_.rh);                                                                     \
           RESOURCE_ENTER (nobugflag, (rwlck)->rh, "acquire rwlock for reading", &lumiera_rwlock_section_,                              \
                           NOBUG_RESOURCE_EXCLUSIVE, lumiera_rwlock_section_.rh);                                                       \
           if (pthread_rwlock_wrlock (&(rwlck)->rwlock)) LUMIERA_DIE (RWLOCK_WRLOCK);                                                   \
         });                                                                                                                            \
         lumiera_rwlock_section_.rwlock;                                                                                                \
         ({                                                                                                                             \
           if (lumiera_rwlock_section_.rwlock)                                                                                          \
             {                                                                                                                          \
               pthread_rwlock_unlock (&lumiera_rwlock_section_.rwlock->rwlock);                                                         \
               lumiera_rwlock_section_.rwlock = NULL;                                                                                   \
               RESOURCE_LEAVE(nobugflag, lumiera_rwlock_section_.rh);                                                                   \
             }                                                                                                                          \
         }))


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
lumiera_rwlock_init (LumieraRWLock self);

/**
 * destroy a rwlock
 * @param self is a pointer to the rwlock to be initialized
 * @return self on success or NULL at error
 */
LumieraRWLock
lumiera_rwlock_destroy (LumieraRWLock self);




/**
 * rwlockacquirer used to manage the state of a rwlock variable.
 */
struct lumiera_rwlockacquirer_struct
{
  LumieraRWLock rwlock;
  RESOURCE_HANDLE (rh);
};
typedef struct lumiera_rwlockacquirer_struct lumiera_rwlockacquirer;
typedef struct lumiera_rwlockacquirer_struct* LumieraRWLockacquirer;

/* helper function for nobug */
static inline void
lumiera_rwlockacquirer_ensureunlocked (LumieraRWLockacquirer self)
{
  ENSURE (!self->rwlock, "forgot to unlock rwlock");
}


#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
