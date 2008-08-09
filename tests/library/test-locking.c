/*
  test-locking.c  -  test locking functions

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

#include "tests/test.h"
#include "lib/mutex.h"
#include "lib/condition.h"

#include <stdio.h>
#include <string.h>

TESTS_BEGIN

TEST ("conditionforgotunlock")
{
  lumiera_condition c;
  lumiera_condition_init (&c);

  lumiera_conditionacquirer l;
  lumiera_conditionacquirer_init (&l, &c, LUMIERA_LOCKED);
  return 0;
}


TEST ("mutexsection")
{
  lumiera_mutex m;
  lumiera_mutex_init (&m);
  RESOURCE_ANNOUNCE (NOBUG_ON, "mutex", "mutexsection", &m, m.rh);

  LUMIERA_MUTEX_SECTION (NOBUG_ON, &m)
    {
      printf ("mutex locked section 1\n");
    }

  LUMIERA_MUTEX_SECTION (NOBUG_ON, &m)
    {
      printf ("mutex locked section 2\n");
    }

  RESOURCE_FORGET (NOBUG_ON, m.rh);
  lumiera_mutex_destroy (&m);
}


TEST ("mutexforgotunlock")
{
  lumiera_mutex m;
  lumiera_mutex_init (&m);
  RESOURCE_ANNOUNCE (NOBUG_ON, "mutex", "mutexforgotunlock", &m, m.rh);

  LUMIERA_MUTEX_SECTION (NOBUG_ON, &m)
    {
      break;    // MUTEX_SECTIONS must not be left by a jump
    }

  RESOURCE_FORGET (NOBUG_ON, m.rh);
  lumiera_mutex_destroy (&m);
}


TEST ("nestedmutexsection")
{
  lumiera_mutex m;
  lumiera_mutex_init (&m);
  RESOURCE_ANNOUNCE (NOBUG_ON, "mutex", "m_mutexsection", &m, m.rh);

  lumiera_mutex n;
  lumiera_mutex_init (&n);
  RESOURCE_ANNOUNCE (NOBUG_ON, "mutex", "n_mutexsection", &n, n.rh);

  LUMIERA_MUTEX_SECTION (NOBUG_ON, &m)
    {
      printf ("outer mutex locked section\n");

      LUMIERA_MUTEX_SECTION (NOBUG_ON, &n)
        {
          printf ("inner mutex locked section\n");
        }
    }

  RESOURCE_FORGET (NOBUG_ON, n.rh);
  lumiera_mutex_destroy (&n);

  RESOURCE_FORGET (NOBUG_ON, m.rh);
  lumiera_mutex_destroy (&m);
}




TESTS_END
