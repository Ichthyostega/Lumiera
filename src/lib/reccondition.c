/*
  reccondition.c  -  condition variable, w/ recursive mutex

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

#include "lib/condition.h"
#include "lib/reccondition.h"

/**
 * @file
 * Condition variables
 */



static pthread_once_t recursive_mutexattr_once = PTHREAD_ONCE_INIT;
static pthread_mutexattr_t recursive_mutexattr;

static void recursive_mutexattr_init()
{
  pthread_mutexattr_init (&recursive_mutexattr);
  pthread_mutexattr_settype (&recursive_mutexattr, PTHREAD_MUTEX_RECURSIVE);
}


LumieraReccondition
lumiera_reccondition_init (LumieraReccondition self, const char* purpose, struct nobug_flag* flag)
{
  if (self)
    {
      if (recursive_mutexattr_once == PTHREAD_ONCE_INIT)
        pthread_once (&recursive_mutexattr_once, recursive_mutexattr_init);

      pthread_cond_init (&self->cond, NULL);
      pthread_mutex_init (&self->mutex, &recursive_mutexattr);
      NOBUG_RESOURCE_HANDLE_INIT (self->rh);
      NOBUG_RESOURCE_ANNOUNCE_RAW (flag, "reccond_var", purpose, self, self->rh);
    }
  return self;
}


LumieraReccondition
lumiera_reccondition_destroy (LumieraReccondition self, struct nobug_flag* flag)
{
  if (self)
    {
      NOBUG_RESOURCE_FORGET_RAW (flag,  self->rh);

      if (pthread_mutex_destroy (&self->mutex))
        LUMIERA_DIE (LOCK_DESTROY);

      if (pthread_cond_destroy (&self->cond))
        LUMIERA_DIE (CONDITION_DESTROY);
    }
  return self;
}




