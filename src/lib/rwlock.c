/*
  rwlock.c  -  read/write locks

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
#include <errno.h>
#include "lib/rwlock.h"

LUMIERA_ERROR_DEFINE(RWLOCK_AGAIN, "maximum number of readlocks exceed");
LUMIERA_ERROR_DEFINE(RWLOCK_DEADLOCK, "deadlock detected");
LUMIERA_ERROR_DEFINE(RWLOCK_DESTROY, "destroy rwlock");
LUMIERA_ERROR_DEFINE(RWLOCK_UNLOCK, "unlock");
LUMIERA_ERROR_DEFINE(RWLOCK_RLOCK, "rlock");
LUMIERA_ERROR_DEFINE(RWLOCK_WLOCK, "wlock");

/**
 * @file
 * Read/write locks.
 */


LumieraRWLock
lumiera_rwlock_init (LumieraRWLock self)
{
  if (self)
    {
      pthread_rwlock_init (&self->rwlock, NULL);
    }
  return self;
}


LumieraRWLock
lumiera_rwlock_destroy (LumieraRWLock self)
{
  if (self)
    {
      if (pthread_rwlock_destroy (&self->rwlock))
        LUMIERA_DIE (RWLOCK_DESTROY);
    }
  return self;
}





LumieraRWLockacquirer
lumiera_rwlockacquirer_init (LumieraRWLockacquirer self, LumieraRWLock rwlock, enum lumiera_lockstate state)
{
  REQUIRE (self);
  REQUIRE (rwlock);
  REQUIRE (state != LUMIERA_LOCKED, "illegal state for rwlock");
  self->rwlock = rwlock;
  self->state = state;

  switch (state)
    {
    case LUMIERA_RDLOCKED:
      switch (pthread_rwlock_rdlock (&rwlock->rwlock))
        {
        case 0:
          break;
        case EAGAIN:
          lumiera_error_set (LUMIERA_ERROR_RWLOCK_AGAIN);
          return NULL;
        case EDEADLK:
          lumiera_error_set (LUMIERA_ERROR_RWLOCK_DEADLOCK);
          return NULL;
        default:
          LUMIERA_DIE (RWLOCK_RLOCK);
        }
    case LUMIERA_WRLOCKED:
      switch (pthread_rwlock_wrlock (&rwlock->rwlock))
        {
        case 0:
          break;
        case EDEADLK:
          lumiera_error_set (LUMIERA_ERROR_RWLOCK_DEADLOCK);
          return NULL;
        default:
          LUMIERA_DIE (RWLOCK_WLOCK);
        }
    default:
      break;
    }
  return self;
}



LumieraRWLockacquirer
lumiera_rwlockacquirer_rdlock (LumieraRWLockacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == LUMIERA_UNLOCKED, "rwlock already locked");

  switch (pthread_rwlock_rdlock (&self->rwlock->rwlock))
    {
    case 0:
      break;
    case EAGAIN:
      lumiera_error_set (LUMIERA_ERROR_RWLOCK_AGAIN);
      return NULL;
    case EDEADLK:
      lumiera_error_set (LUMIERA_ERROR_RWLOCK_DEADLOCK);
      return NULL;
    default:
      LUMIERA_DIE (RWLOCK_RLOCK);
    }

  self->state = LUMIERA_RDLOCKED;
  return self;
}



LumieraRWLockacquirer
lumiera_rwlockacquirer_wrlock (LumieraRWLockacquirer self)
{
  REQUIRE (self);
  REQUIRE (self->state == LUMIERA_UNLOCKED, "rwlock already locked");

  switch (pthread_rwlock_wrlock (&self->rwlock->rwlock))
    {
    case 0:
      break;
    case EDEADLK:
      lumiera_error_set (LUMIERA_ERROR_RWLOCK_DEADLOCK);
      return NULL;
    default:
      LUMIERA_DIE (RWLOCK_WLOCK);
    }

  self->state = LUMIERA_WRLOCKED;
  return self;
}

