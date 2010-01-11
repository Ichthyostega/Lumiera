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

#include "lib/error.h"
#include "lib/rwlock.h"


/**
 * @file
 * Read/write locks.
 */

LumieraRWLock
lumiera_rwlock_init (LumieraRWLock self, const char* purpose, struct nobug_flag* flag)
{
  if (self)
    {
      pthread_rwlock_init (&self->rwlock, NULL);
      NOBUG_RESOURCE_HANDLE_INIT (self->rh);
      NOBUG_RESOURCE_ANNOUNCE_RAW (flag, "rwlock", purpose, self, self->rh);
    }
  return self;
}


LumieraRWLock
lumiera_rwlock_destroy (LumieraRWLock self, struct nobug_flag* flag)
{
  if (self)
    {
      NOBUG_RESOURCE_FORGET_RAW (flag,  self->rh);
      if (pthread_rwlock_destroy (&self->rwlock))
        LUMIERA_DIE (LOCK_DESTROY);
    }
  return self;
}


/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
