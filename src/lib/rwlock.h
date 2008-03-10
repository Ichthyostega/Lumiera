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

#ifndef CINELERRA_RWLOCK_H
#define CINELERRA_RWLOCK_H

#ifndef _GNU_SOURCE
#error "This header must be included with _GNU_SOURCE or _POSIX_C_SOURCE >= 200112L defined"
#endif

#include <pthread.h>
#include <nobug.h>

#include "lib/locking.h"

CINELERRA_ERROR_DECLARE(RWLOCK_AGAIN);
CINELERRA_ERROR_DECLARE(RWLOCK_DEADLOCK);

/**
 * @file Read/write locks, header.
 */


/**
 * RWLock.
 *
 */
struct cinelerra_rwlock_struct
{
  pthread_rwlock_t rwlock;
};
typedef struct cinelerra_rwlock_struct cinelerra_rwlock;
typedef cinelerra_rwlock* CinelerraRWLock;


CinelerraRWLock
cinelerra_rwlock_init (CinelerraRWLock self);


CinelerraRWLock
cinelerra_rwlock_destroy (CinelerraRWLock self);




/**
 * rwlockacquirer used to manage the state of a rwlock variable.
 */
struct cinelerra_rwlockacquirer_struct
{
  CinelerraRWLock rwlock;
  enum cinelerra_lockstate  state;
};
typedef struct cinelerra_rwlockacquirer_struct cinelerra_rwlockacquirer;
typedef struct cinelerra_rwlockacquirer_struct* CinelerraRWLockacquirer;

/* helper function for nobug */
static inline void
cinelerra_rwlockacquirer_ensureunlocked (CinelerraRWLockacquirer self)
{
  ENSURE (self->state == CINELERRA_UNLOCKED, "forgot to unlock the rwlock mutex");
}

/* override with a macro to use the cleanup checker */
#define cinelerra_rwlockacquirer \
cinelerra_rwlockacquirer NOBUG_CLEANUP(cinelerra_rwlockacquirer_ensureunlocked)


CinelerraRWLockacquirer
cinelerra_rwlockacquirer_init (CinelerraRWLockacquirer self, CinelerraRWLock rwlock, enum cinelerra_lockstate state);

CinelerraRWLockacquirer
cinelerra_rwlockacquirer_rdlock (CinelerraRWLockacquirer self);


CinelerraRWLockacquirer
cinelerra_rwlockacquirer_wrlock (CinelerraRWLockacquirer self);


/**
 * release rwlock.
 * a rwlockacquirer must be unlocked before leaving scope
 * @param self rwlockacquirer associated with a rwlock variable
 */
static inline void
cinelerra_rwlockacquirer_unlock (CinelerraRWLockacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state != CINELERRA_UNLOCKED, "rwlock was not locked");
  if (pthread_rwlock_unlock (&self->rwlock->rwlock))
    CINELERRA_DIE;
  self->state = CINELERRA_UNLOCKED;
}


#endif
