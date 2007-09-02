/*
  condition.c  -  condition variable

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

#include "lib/condition.h"

CinelerraCondition
cinelerra_condition_init (CinelerraCondition self)
{
  if (self)
    {
      pthread_cond_init (&self->cond, NULL);
      pthread_mutex_init (&self->mutex, NULL);
    }
  return self;
}

CinelerraCondition
cinelerra_condition_destroy (CinelerraCondition self)
{
  if (self)
    {
      if (pthread_mutex_destroy (&self->mutex))
        CINELERRA_DIE;
      else if (pthread_cond_destroy (&self->cond))
        CINELERRA_DIE;
    }
  return self;
}




