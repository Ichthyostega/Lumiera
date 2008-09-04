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

#include "lib/locking.h"

LUMIERA_ERROR_DECLARE(RWLOCK_AGAIN);
LUMIERA_ERROR_DECLARE(RWLOCK_DEADLOCK);
LUMIERA_ERROR_DECLARE(RWLOCK_DESTROY);
LUMIERA_ERROR_DECLARE(RWLOCK_UNLOCK);
LUMIERA_ERROR_DECLARE(RWLOCK_RLOCK);
LUMIERA_ERROR_DECLARE(RWLOCK_WLOCK);

/**
 * @file
 * Read/write locks, header.
 */


/**
 * RWLock.
 *
 */
struct lumiera_rwlock_struct
{
  pthread_rwlock_t rwlock;
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
  enum lumiera_lockstate  state;
};
typedef struct lumiera_rwlockacquirer_struct lumiera_rwlockacquirer;
typedef struct lumiera_rwlockacquirer_struct* LumieraRWLockacquirer;

/* helper function for nobug */
static inline void
lumiera_rwlockacquirer_ensureunlocked (LumieraRWLockacquirer self)
{
  ENSURE (self->state == LUMIERA_UNLOCKED, "forgot to unlock the rwlock mutex");
}

/* override with a macro to use the cleanup checker */
#define lumiera_rwlockacquirer \
lumiera_rwlockacquirer NOBUG_CLEANUP(lumiera_rwlockacquirer_ensureunlocked)

/**
 * initialize a rwlockacquirer state
 * @param self rwlockacquirer to be initialized, must be an automatic variable
 * @param rwlock associated rwlock
 * @param state initial state of the mutex, either LUMIERA_RDLOCKED, LUMIERA_WRLOCKED or LUMIERA_UNLOCKED
 * @return self as given or NULL on error
 */
LumieraRWLockacquirer
lumiera_rwlockacquirer_init (LumieraRWLockacquirer self, LumieraRWLock rwlock, enum lumiera_lockstate state);

/**
 * readlock the rwlock.
 * must not already be locked
 * @param self rwlockacquirer associated with a rwlock
 * @return self as given or NULL on error
 */
LumieraRWLockacquirer
lumiera_rwlockacquirer_rdlock (LumieraRWLockacquirer self);

/**
 * writelock the rwlock.
 * must not already be locked
 * @param self rwlockacquirer associated with a rwlock
 * @return self as given or NULL on error
 */
LumieraRWLockacquirer
lumiera_rwlockacquirer_wrlock (LumieraRWLockacquirer self);


/**
 * release rwlock.
 * a rwlockacquirer must be unlocked before leaving scope
 * @param self rwlockacquirer associated with a rwlock variable
 */
static inline void
lumiera_rwlockacquirer_unlock (LumieraRWLockacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state != LUMIERA_UNLOCKED, "rwlock was not locked");
  if (pthread_rwlock_unlock (&self->rwlock->rwlock))
    LUMIERA_DIE (RWLOCK_UNLOCK);
  self->state = LUMIERA_UNLOCKED;
}


#endif
