/*
  rwlock.c  -  read/write locks

  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>

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
#define _GNU_SOURCE

#include <errno.h>
#include "lib/rwlock.h"

CINELERRA_ERROR_DEFINE(RWLOCK_AGAIN, "maximum number of readlocks exceed");
CINELERRA_ERROR_DEFINE(RWLOCK_DEADLOCK, "deadlock detected");

/**
 * @file Read/write locks.
 */


/**
 * Initialize a rwlock
 * @param self is a pointer to the rwlock to be initialized
 * @return self as given
 */
CinelerraRWLock
cinelerra_rwlock_init (CinelerraRWLock self)
{
  if (self)
    {
      pthread_rwlock_init (&self->rwlock, NULL);
    }
  return self;
}

/**
 * destroy a rwlock
 * @param self is a pointer to the rwlock to be initialized
 * @return self on success or NULL at error
 */
CinelerraRWLock
cinelerra_rwlock_destroy (CinelerraRWLock self)
{
  if (self)
    {
      if (pthread_rwlock_destroy (&self->rwlock))
        CINELERRA_DIE;
    }
  return self;
}




/**
 * initialize a rwlockacquirer state
 * @param self rwlockacquirer to be initialized, must be an automatic variable
 * @param cond associated rwlock
 * @param state initial state of the mutex, either CINELERRA_RDLOCKED, CINELERRA_WRLOCKED or CINELERRA_UNLOCKED
 * @return self as given or NULL on error
 */
CinelerraRWLockacquirer
cinelerra_rwlockacquirer_init (CinelerraRWLockacquirer self, CinelerraRWLock rwlock, enum cinelerra_lockstate state)
{
  REQUIRE (self);
  REQUIRE (rwlock);
  REQUIRE (state != CINELERRA_LOCKED, "illegal state for rwlock");
  self->rwlock = rwlock;
  self->state = state;

  switch (state)
    {
    case CINELERRA_RDLOCKED:
      switch (pthread_rwlock_rdlock (&rwlock->rwlock))
        {
        case 0:
          break;
        case EAGAIN:
          cinelerra_error_set (CINELERRA_ERROR_RWLOCK_AGAIN);
          return NULL;
        case EDEADLK:
          cinelerra_error_set (CINELERRA_ERROR_RWLOCK_DEADLOCK);
          return NULL;
        default:
          CINELERRA_DIE;
        }
    case CINELERRA_WRLOCKED:
      switch (pthread_rwlock_wrlock (&rwlock->rwlock))
        {
        case 0:
          break;
        case EDEADLK:
          cinelerra_error_set (CINELERRA_ERROR_RWLOCK_DEADLOCK);
          return NULL;
        default:
          CINELERRA_DIE;
        }
    default:
      break;
    }
  return self;
}


/**
 * readlock the rwlock.
 * must not already be locked
 * @param self rwlockacquirer associated with a rwlock
 * @return self as given or NULL on error
 */
CinelerraRWLockacquirer
cinelerra_rwlockacquirer_rdlock (CinelerraRWLockacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == CINELERRA_UNLOCKED, "rwlock already locked");

  switch (pthread_rwlock_rdlock (&self->rwlock->rwlock))
    {
    case 0:
      break;
    case EAGAIN:
      cinelerra_error_set (CINELERRA_ERROR_RWLOCK_AGAIN);
      return NULL;
    case EDEADLK:
      cinelerra_error_set (CINELERRA_ERROR_RWLOCK_DEADLOCK);
      return NULL;
    default:
      CINELERRA_DIE;
    }

  self->state = CINELERRA_RDLOCKED;
  return self;
}


/**
 * writelock the rwlock.
 * must not already be locked
 * @param self rwlockacquirer associated with a rwlock
 * @return self as given or NULL on error
 */
CinelerraRWLockacquirer
cinelerra_rwlockacquirer_wrlock (CinelerraRWLockacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == CINELERRA_UNLOCKED, "rwlock already locked");

  switch (pthread_rwlock_wrlock (&self->rwlock->rwlock))
    {
    case 0:
      break;
    case EDEADLK:
      cinelerra_error_set (CINELERRA_ERROR_RWLOCK_DEADLOCK);
      return NULL;
    default:
      CINELERRA_DIE;
    }

  self->state = CINELERRA_WRLOCKED;
  return self;
}

