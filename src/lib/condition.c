/*
  condition.c  -  condition variable

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

/**
 * @file
 * Condition variables
 */

LUMIERA_ERROR_DEFINE (CONDITION_DESTROY, "condition destroy failed");


LumieraCondition
lumiera_condition_init (LumieraCondition self, const char* purpose, struct nobug_flag* flag)
{
  if (self)
    {
      pthread_cond_init (&self->cond, NULL);
      pthread_mutex_init (&self->mutex, NULL);
      NOBUG_RESOURCE_HANDLE_INIT (self->rh);
      NOBUG_RESOURCE_ANNOUNCE_RAW (flag, "cond_var", purpose, self, self->rh);
    }
  return self;
}


LumieraCondition
lumiera_condition_destroy (LumieraCondition self, struct nobug_flag* flag)
{
  if (self)
    {
      NOBUG_RESOURCE_FORGET_RAW (flag,  self->rh);

      if (pthread_mutex_destroy (&self->mutex))
        LUMIERA_DIE (MUTEX_DESTROY);

      if (pthread_cond_destroy (&self->cond))
        LUMIERA_DIE (CONDITION_DESTROY);
    }
  return self;
}




