/*
  TEST-LOCKING  -  test locking functions

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/

/** @file §§§
 ** unit test §§TODO§§
 */


#include "lib/test/test.h"
#include "lib/mutex.h"
#include "lib/recmutex.h"
#include "lib/condition.h"
#include "lib/reccondition.h"
#include "lib/rwlock.h"

#include <stdio.h>
#include <string.h>

TESTS_BEGIN


TEST (mutexsection)
{
  lumiera_mutex m;
  lumiera_mutex_init (&m, "mutexsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_MUTEX_SECTION (NOBUG_ON, &m)
    {
      printf ("mutex locked section 1\n");
    }

  LUMIERA_MUTEX_SECTION (NOBUG_ON, &m)
    {
      printf ("mutex locked section 2\n");
    }

  lumiera_mutex_destroy (&m, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}


TEST (mutexforgotunlock)
{
  lumiera_mutex m;
  lumiera_mutex_init (&m, "mutexforgotunlock", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_MUTEX_SECTION (NOBUG_ON, &m)
    {
      break;    // MUTEX_SECTIONS must not be left by a jump
    }

  lumiera_mutex_destroy (&m, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}


TEST (mutexexplicitunlock)
{
  lumiera_mutex m;
  lumiera_mutex_init (&m, "mutexforgotunlock", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_MUTEX_SECTION (NOBUG_ON, &m)
    {
      ECHO("mutex locked section");
      LUMIERA_MUTEX_SECTION_UNLOCK;
      break;
    }

  lumiera_mutex_destroy (&m, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}


TEST (nestedmutexsection)
{
  lumiera_mutex m;
  lumiera_mutex_init (&m, "m_mutexsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  lumiera_mutex n;
  lumiera_mutex_init (&n, "n_mutexsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_MUTEX_SECTION (NOBUG_ON, &m)
    {
      printf ("outer mutex locked section\n");

      LUMIERA_MUTEX_SECTION (NOBUG_ON, &n)
        {
          printf ("inner mutex locked section\n");
        }
    }

  lumiera_mutex_destroy (&n, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
  lumiera_mutex_destroy (&m, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}

TEST (chainedmutexsection)
{
  lumiera_mutex m;
  lumiera_mutex_init (&m, "m_mutexsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  lumiera_mutex n;
  lumiera_mutex_init (&n, "n_mutexsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_MUTEX_SECTION (NOBUG_ON, &m)
    {
      printf ("outer mutex locked section\n");

      LUMIERA_MUTEX_SECTION_CHAIN (NOBUG_ON, &n)
        {
          printf ("inner but not outer mutex locked section\n");
        }
    }

  lumiera_mutex_destroy (&n, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
  lumiera_mutex_destroy (&m, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}



TEST (recursivemutexsection)
{
  lumiera_recmutex m;
  lumiera_recmutex_init (&m, "m_recmutexsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);


  LUMIERA_RECMUTEX_SECTION (NOBUG_ON, &m)
    {
      printf ("recmutex locked once\n");

      LUMIERA_RECMUTEX_SECTION (NOBUG_ON, &m)
        {
          printf ("recmutex locked twice\n");
        }
    }

 lumiera_recmutex_destroy (&m, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}



TEST (rwlocksection)
{
  lumiera_rwlock rwlock;
  lumiera_rwlock_init (&rwlock, "rwsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_WRLOCK_SECTION (NOBUG_ON, &rwlock)
    {
      printf ("write locked section 1\n");
    }

  LUMIERA_RDLOCK_SECTION (NOBUG_ON, &rwlock)
    {
      printf ("read locked section 2\n");
    }

  lumiera_rwlock_destroy (&rwlock, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}


TEST (rwlockforgotunlock)
{
  lumiera_rwlock rwlock;
  lumiera_rwlock_init (&rwlock, "rwlockforgotunlock", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_RDLOCK_SECTION (NOBUG_ON, &rwlock)
    {
      break;    // LOCK_SECTIONS must not be left by a jump
    }

  lumiera_rwlock_destroy (&rwlock, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}



TEST (rwdeadlockwr)
{
  lumiera_rwlock rwlock;
  lumiera_rwlock_init (&rwlock, "rwsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_WRLOCK_SECTION (NOBUG_ON, &rwlock)
    {
      printf ("write locked section 1\n");
      LUMIERA_RDLOCK_SECTION (NOBUG_ON, &rwlock)
        {
          printf ("read locked section 2\n");
        }
    }

  lumiera_rwlock_destroy (&rwlock, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}



TEST (rwdeadlockrw)
{
  lumiera_rwlock rwlock;
  lumiera_rwlock_init (&rwlock, "rwsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_RDLOCK_SECTION (NOBUG_ON, &rwlock)
    {
      printf ("read locked section 1\n");
      LUMIERA_WRLOCK_SECTION (NOBUG_ON, &rwlock)
        {
          printf ("write locked section 2\n");
        }
    }

  lumiera_rwlock_destroy (&rwlock, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}


TEST (conditionops (compiletest only))
{
  lumiera_condition cond;
  lumiera_condition_init (&cond, "conditionsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_CONDITION_SECTION (NOBUG_ON, &cond)
    {
      LUMIERA_CONDITION_WAIT(1);
      LUMIERA_CONDITION_SIGNAL;
      LUMIERA_CONDITION_BROADCAST;
    }

  lumiera_condition_destroy (&cond, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}


TEST (conditionsection)
{
  lumiera_condition cond;
  lumiera_condition_init (&cond, "conditionsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_CONDITION_SECTION (NOBUG_ON, &cond)
    {
      printf ("condition locked section 1\n");
    }

  LUMIERA_CONDITION_SECTION (NOBUG_ON, &cond)
    {
      printf ("condition locked section 2\n");
    }

  lumiera_condition_destroy (&cond, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}



TEST (conditionforgotunlock)
{
  lumiera_condition cond;
  lumiera_condition_init (&cond, "conditionforgotunlock", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_CONDITION_SECTION (NOBUG_ON, &cond)
    {
      break;    // CONDITION_SECTIONS must not be left by a jump
    }

  lumiera_condition_destroy (&cond, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}



TEST (recconditionops (compiletest only))
{
  lumiera_reccondition reccond;
  lumiera_reccondition_init (&reccond, "recconditionsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_RECCONDITION_SECTION (NOBUG_ON, &reccond)
    {
      LUMIERA_RECCONDITION_WAIT(1);
      LUMIERA_RECCONDITION_SIGNAL;
      LUMIERA_RECCONDITION_BROADCAST;
    }

  lumiera_reccondition_destroy (&reccond, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}


TEST (recconditionsection)
{
  lumiera_reccondition reccond;
  lumiera_reccondition_init (&reccond, "recconditionsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_RECCONDITION_SECTION (NOBUG_ON, &reccond)
    {
      printf ("reccondition locked section 1\n");
    }

  LUMIERA_RECCONDITION_SECTION (NOBUG_ON, &reccond)
    {
      printf ("reccondition locked section 2\n");
    }

  lumiera_reccondition_destroy (&reccond, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}


TEST (recconditionforgotunlock)
{
  lumiera_reccondition reccond;
  lumiera_reccondition_init (&reccond, "recconditionforgotunlock", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_RECCONDITION_SECTION (NOBUG_ON, &reccond)
    {
      break;    // RECCONDITION_SECTIONS must not be left by a jump
    }

  lumiera_reccondition_destroy (&reccond, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}

TEST (chainedrecconditionsection)
{
  lumiera_reccondition outer, inner;
  lumiera_reccondition_init (&outer, "outer_recconditionsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
  lumiera_reccondition_init (&inner, "inner_recconditionsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_RECCONDITION_SECTION (NOBUG_ON, &outer)
    {
      printf ("outer reccondition locked section\n");
      LUMIERA_RECCONDITION_SECTION_CHAIN (NOBUG_ON, &inner)
	{
	  printf ("inner reccondition locked section\n");
	}
    }
  lumiera_reccondition_destroy (&outer, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
  lumiera_reccondition_destroy (&inner, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}

TEST (nestedrecconditionsection)
{
  lumiera_reccondition outer, inner;
  lumiera_reccondition_init (&outer, "outer_recconditionsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
  lumiera_reccondition_init (&inner, "inner_recconditionsection", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_RECCONDITION_SECTION (NOBUG_ON, &outer)
    {
      printf ("outer reccondition locked section\n");
      LUMIERA_RECCONDITION_SECTION (NOBUG_ON, &inner)
	{
	  printf ("inner reccondition locked section\n");
	}
    }
  lumiera_reccondition_destroy (&outer, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
  lumiera_reccondition_destroy (&inner, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}


TESTS_END
