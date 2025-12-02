/*
  TEST-LOCKING  -  test locking functions

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file test-locking.c
 ** C unit test to cover convenience setup for POSIX locking primitives
 ** These convenience macros are intended for use in low-level C code,
 ** where minimal and precise locking is relevant for performance reasons.
 ** @see mutex.h
 ** @see condition.h
 */


#include "lib/test/test.h"
#include "lib/mutex.h"
#include "lib/recmutex.h"

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


/* ====== 10/2023 : partially dismantled
 * 
 * After switching to C++14 Threads and Locking (#1279),
 * some backend-services are no longer used...
 * - rwlocksection
 * - rwlockforgotunlock
 * - rwdeadlockwr
 * - rwdeadlockrw
 * - rwlockdeadlockwr
 * - rwlockdeadlockrw
 * - conditionops
 * - conditionsection
 * - conditionforgotunlock
 * - condition signaling (planned)
 * - condition broadcasting (planned)
 * - recconditionops
 * - recconditionsection
 * - recconditionforgotunlock
 * - chainedrecconditionsection
 * - nestedrecconditionsection
 * - reccondition signaling (planned)
 * - reccondition broadcasting (planned)
 * 
 */

TESTS_END
