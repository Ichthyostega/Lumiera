/*
  TEST-THREADS  -  test thread management

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

/** @file test-threads.c
 ** C unit test to cover thread handling helpers
 ** Especially, we rely on a specific setup for threads, which allows
 ** to manage worker threads in a threadpool
 ** @see threads.h
 ** @see test-threadpool.c
 */


//#include <stdio.h>
//#include <string.h>

#include "common/config.h"

#include "include/logging.h"
#include "lib/mutex.h"
#include "vault/threads.h"
#include "lib/test/test.h"

#include <unistd.h>
#include <errno.h>
#include <time.h>

void threadfn(void* blah)
{
  (void) blah;
  struct timespec wait = {0,300000000};

  fprintf (stderr, "thread running %s\n", NOBUG_THREAD_ID_GET);
  nanosleep (&wait, NULL);
  fprintf (stderr, "thread done %s\n", NOBUG_THREAD_ID_GET);
}


void threadsyncfn(void* blah)
{
  struct timespec wait = {0,200000000};
  LumieraCondition sync = (LumieraCondition) blah;

  ECHO ("thread starting up %s", NOBUG_THREAD_ID_GET);
  LUMIERA_CONDITION_SECTION(cond_sync, sync)
    {
      ECHO ("send startup signal %s", NOBUG_THREAD_ID_GET);
      LUMIERA_CONDITION_SIGNAL;
      ECHO ("wait for trigger %s", NOBUG_THREAD_ID_GET);
      LUMIERA_CONDITION_WAIT(1);
    }

  ECHO ("thread running %s", NOBUG_THREAD_ID_GET);
  nanosleep (&wait, NULL);
  ECHO ("thread done %s", NOBUG_THREAD_ID_GET);
}



lumiera_mutex testmutex;

void mutexfn(void* blah)
{
  (void) blah;
  struct timespec wait = {0,300000000};

  LUMIERA_MUTEX_SECTION (NOBUG_ON, &testmutex)
    {
      fprintf (stderr, "mutex thread running %s\n", NOBUG_THREAD_ID_GET);
      nanosleep (&wait, NULL);
      fprintf (stderr, "thread done %s\n", NOBUG_THREAD_ID_GET);
    }
}



TESTS_BEGIN

TEST ("simple_thread")
{
  fprintf (stderr, "main before thread %s\n", NOBUG_THREAD_ID_GET);

  lumiera_thread_run (LUMIERA_THREADCLASS_WORKER,
                      threadfn,
                      NULL,
                      argv[1],
                      NULL);

  struct timespec wait = {0,600000000};
  nanosleep (&wait, NULL);
  fprintf (stderr, "main after thread %s\n", NOBUG_THREAD_ID_GET);
}

TEST ("thread_synced")
{
  lumiera_condition cnd;
  lumiera_condition_init (&cnd, "threadsync", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_CONDITION_SECTION(cond_sync, &cnd)
    {
      ECHO ("main before thread %s", NOBUG_THREAD_ID_GET);

      lumiera_thread_run (LUMIERA_THREADCLASS_WORKER,
                          threadsyncfn,
                          &cnd,
                          argv[1],
                          NULL);

      ECHO ("main wait for thread being ready %s", NOBUG_THREAD_ID_GET);
      LUMIERA_CONDITION_WAIT(1);

      ECHO ("main trigger thread %s", NOBUG_THREAD_ID_GET);
      LUMIERA_CONDITION_SIGNAL;

      ECHO ("wait for thread end %s", NOBUG_THREAD_ID_GET);
      LUMIERA_CONDITION_WAIT(1);
      ECHO ("thread ended %s", NOBUG_THREAD_ID_GET);
    }

  lumiera_condition_destroy (&cnd, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}



TEST ("mutex_thread")
{
  lumiera_mutex_init (&testmutex, "test", &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);

  LUMIERA_MUTEX_SECTION (NOBUG_ON, &testmutex)
    {
      fprintf (stderr, "main before thread %s\n", NOBUG_THREAD_ID_GET);

      lumiera_thread_run (LUMIERA_THREADCLASS_WORKER,
                          mutexfn,
                          NULL,
                          argv[1],
                          NULL);

      struct timespec wait = {0,600000000};
      nanosleep (&wait, NULL);
      fprintf (stderr, "main after thread %s\n", NOBUG_THREAD_ID_GET);
    }

  lumiera_mutex_destroy (&testmutex, &NOBUG_FLAG(NOBUG_ON), NOBUG_CONTEXT);
}


TESTS_END
