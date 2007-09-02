/*
  test condition functions

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

#if 0
waiting_thread()
{
  lock;
  wait;
  unlock;
}


signaling_thread()
{
  signal();
}
#endif


int
conditionforgotunlock ()
{
  cinelerra_condition c;
  cinelerra_condition_init (&c);

  cinelerra_conditionlock l;
  cinelerra_conditionlock_init (&l, &c, CINELERRA_LOCKED);
  return 0;
}
