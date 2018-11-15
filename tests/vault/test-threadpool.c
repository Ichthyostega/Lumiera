/*
  TEST-THREADPOOL  -  test thread pool creation and usage

  Copyright (C)         Lumiera.org
    2009,               Michael Ploujnikov <ploujj@gmail.com>

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

/** @file test-threadpool.c
 ** C unit test to cover our threadpool implementation
 */


#include "lib/test/test.h"

#include "backend/threadpool.h"
#include "include/logging.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

void is_prime(void * arg)
{
  unsigned long long number = *(unsigned long long *)arg;
  unsigned long long prime = 1;
  usleep(1);
  for (unsigned long long x = number-1; x >= sqrt(number); --x)
    {
      if ((number % x) == 0)
        {
          prime = 0;
          break;
        }
    }
  *(unsigned long long *)arg = prime;
  usleep(1);
}

void sleep_fn(void * arg)
{
  unsigned int usec = *(int *)arg;

  unsigned int result = usleep (usec);
  *(int *)arg = result;
}


void other_fn(void * arg)
{
  int input = *(int *)arg;
  lumiera_thread_sync (); // the main thread can discard the argument storage
  CHECK (input == 42, "input is not 42, but %d", input);
  input -= 42;
  ECHO ("result is %d", input);
}

void sleeping_worker_fn(void * arg)
{
  int input = *(int *)arg;
  int delay = rand () % 100000;
  usleep (delay);
  lumiera_thread_sync (); // the main thread can discard the argument storage
  input -= 81;
  CHECK (input == 42, "result is not 42, but %d", input);
}

// subtracts 13 from the value
void joinable_worker_fn(void * arg)
{
  int input = *(int *)arg;
  lumiera_thread_sync (); // signal that arguments have been received
  *(int *)arg = input - 13;
}

// adds 42 to the value
void joinable_master_fn(void * arg)
{
  int input = *(int *)arg;
  lumiera_thread_sync ();
  CHECK (input == 42, "input is not 42, but %d", input);
  LumieraThread worker = lumiera_thread_run (LUMIERA_THREADCLASS_IDLE
                                             | LUMIERA_THREAD_JOINABLE,
                                             &joinable_worker_fn,
                                             (void *)&input,
                                             "joinable worker thread",
                                             &NOBUG_FLAG (NOBUG_ON));
  lumiera_thread_sync_other (worker);
  lumiera_thread_join (worker);
  lumiera_thread_sync_other (worker); // wait until the arguments are sent
  lumiera_thread_join (worker); // wait until the result has been calculated
  CHECK (input == 42-13, "result is not 42-13=29, but %d", input);
  input += 42;
  *(int *)arg = input;
}

TESTS_BEGIN

TEST (threadpool-basic)
{
  lumiera_threadpool_init();
  lumiera_threadpool_destroy();
}

TEST (threadpool1)
{
  ECHO("start by initializing the threadpool");
  lumiera_threadpool_init();
  LumieraThread t1 =
    lumiera_threadpool_acquire_thread(LUMIERA_THREADCLASS_INTERACTIVE,
                                      "test purpose",
                                      &NOBUG_FLAG(NOBUG_ON));
  //  lumiera_threadpool_release_thread(t1);
  ECHO("acquired thread 1 %p",t1);
  lumiera_threadpool_destroy();
}


TEST (two-thread-acquire)
{
  ECHO("start by initializing the threadpool");
  lumiera_threadpool_init();
  ECHO("acquiring thread 1");

  LumieraThread t1 =
    lumiera_threadpool_acquire_thread(LUMIERA_THREADCLASS_INTERACTIVE,
                                      "test purpose",
                                      &NOBUG_FLAG(NOBUG_ON));

  ECHO("acquiring thread 2");
  LumieraThread t2 =
    lumiera_threadpool_acquire_thread(LUMIERA_THREADCLASS_IDLE,
                                      "test purpose",
                                      &NOBUG_FLAG(NOBUG_ON));

  ECHO("thread 1 state=%s", lumiera_threadstate_names[t1->state]);
  CHECK(LUMIERA_THREADSTATE_IDLE == t1->state);

  ECHO("thread 2 state=%s", lumiera_threadstate_names[t2->state]);
  CHECK(LUMIERA_THREADSTATE_IDLE == t2->state);

  LUMIERA_CONDITION_SECTION(NOBUG_ON, &t1->signal)
    {
      t1->state = LUMIERA_THREADSTATE_WAKEUP;
      LUMIERA_CONDITION_SIGNAL;
    }

  LUMIERA_CONDITION_SECTION(NOBUG_ON, &t2->signal)
    {
      t2->state = LUMIERA_THREADSTATE_WAKEUP;
      LUMIERA_CONDITION_SIGNAL;
    }

  ECHO("cleaning up");
  lumiera_threadpool_destroy();
}

TEST (many-sleepy-threads)
{

  const int threads_per_pool_count = 10;
  unsigned int delay = 10000;

  lumiera_threadpool_init();
  LumieraThread threads[threads_per_pool_count*LUMIERA_THREADCLASS_COUNT];
  
  for (int kind = 0; kind < LUMIERA_THREADCLASS_COUNT; ++kind)
    {
      for (int i = 0; i < threads_per_pool_count; ++i)
        {
          threads[i+kind*threads_per_pool_count] =
            lumiera_thread_run(kind,
                               &sleep_fn,
                               (void *) &delay,
                               "just sleep a bit",
                               &NOBUG_FLAG(NOBUG_ON));
        }
    }

  lumiera_threadpool_destroy();

}

TEST (toomany-random-sleepy-threads (compiletest only))
{
  const int threads_per_pool_count = 500;
  unsigned int delay[threads_per_pool_count*LUMIERA_THREADCLASS_COUNT];
  lumiera_threadpool_init();
  LumieraThread threads[threads_per_pool_count*LUMIERA_THREADCLASS_COUNT];

  for (int kind = 0; kind < LUMIERA_THREADCLASS_COUNT; ++kind)
    {
      for (int i = 0; i < threads_per_pool_count; ++i)
        {
          delay[i] = rand() % 1000000;
          threads[i+kind*threads_per_pool_count] =
            lumiera_thread_run(kind,
                               &sleep_fn,
                               (void *) &delay[i],
                               "just sleep a bit",
                               &NOBUG_FLAG(NOBUG_ON));
        }
    }
  lumiera_threadpool_destroy();
}

TEST (no-function)
{
  LumieraThread t;

  lumiera_threadpool_init();

  t = lumiera_thread_run (LUMIERA_THREADCLASS_INTERACTIVE,
                          NULL,
                          NULL,
                          "process my test function",
                          &NOBUG_FLAG(NOBUG_ON));

  // cleanup
  ECHO("finished waiting");
  lumiera_threadpool_destroy();
}

TEST (process-function)
{
  // this is what the scheduler would do once it figures out what function a job needs to run
  LumieraThread t;
  unsigned long long number = 1307;

  lumiera_threadpool_init();

  ECHO ("the input to the function is %llu", number);

  t = lumiera_thread_run (LUMIERA_THREADCLASS_INTERACTIVE,
                          &is_prime,
                          (void *)&number, //void * arg,
                          "process my test function",
                          &NOBUG_FLAG(NOBUG_ON)); // struct nobug_flag* flag)

  // cleanup
  lumiera_threadpool_destroy();
  ECHO("the result is %llu", number);

}

TEST (many-random-sleepy-threads (compiletest only))
{
  const int threads_per_pool_count = 10;
  unsigned int delay[threads_per_pool_count*LUMIERA_THREADCLASS_COUNT];
  lumiera_threadpool_init();
  LumieraThread threads[threads_per_pool_count*LUMIERA_THREADCLASS_COUNT];

  for (int kind = 0; kind < LUMIERA_THREADCLASS_COUNT; ++kind)
    {
      for (int i = 0; i < threads_per_pool_count; ++i)
        {
          delay[i] = rand() % 1000000;
          threads[i+kind*threads_per_pool_count] =
            lumiera_thread_run(kind,
                               &sleep_fn,
                               (void *) &delay[i],
                               "just sleep a bit",
                               &NOBUG_FLAG(NOBUG_ON));
        }
    }
  lumiera_threadpool_destroy();
}

TEST (simple-sync)
{
  lumiera_threadpool_init ();

  int value = 42;

  LumieraThread other = lumiera_thread_run (LUMIERA_THREADCLASS_IDLE,
                                             &other_fn,
                                             (void *)&value,
                                             "other thread",
                                             &NOBUG_FLAG (NOBUG_ON));

  ECHO ("syncing with the other thread");
  lumiera_thread_sync_other (other);
  value += 42;
  CHECK (value == 42*2, "value is not equal to 42*2=84, but %d", value);

  lumiera_threadpool_destroy ();
}

TEST (sync-many)
{
  lumiera_threadpool_init ();

  int value = 1337;
  int workers = 100;
  LumieraThread threads[workers];

  for (int i = 0; i < workers; i++)
    {
      value = 123;
      threads[i] = lumiera_thread_run (LUMIERA_THREADCLASS_IDLE,
                                       &sleeping_worker_fn,
                                       (void *)&value,
                                       "worker thread",
                                       &NOBUG_FLAG (NOBUG_ON));
      lumiera_thread_sync_other (threads[i]);
      value -= 123;
    }
  CHECK (value == 0, "final value is not 0, but %d", value);
  lumiera_threadpool_destroy ();
}

TEST (joinable-thread)
{
  int delay = 10000;
  lumiera_threadpool_init ();
  LumieraThread t = lumiera_thread_run (LUMIERA_THREADCLASS_IDLE
                                             | LUMIERA_THREAD_JOINABLE,
                                             &sleep_fn,
                                             (void *)&delay,
                                             "joinable idle thread",
                                             &NOBUG_FLAG (NOBUG_ON));
  lumiera_thread_join(t);
  lumiera_threadpool_destroy ();
}

TEST (sync-joinable)
{
  // NOTE: this test essentially avoids concurrency with _sync() calls
  lumiera_threadpool_init ();

  int value = 42;

  LumieraThread master = lumiera_thread_run (LUMIERA_THREADCLASS_IDLE
                                             | LUMIERA_THREAD_JOINABLE,
                                             &joinable_master_fn,
                                             (void *)&value,
                                             "joinable master thread",
                                             &NOBUG_FLAG (NOBUG_ON));

  lumiera_thread_sync_other (master);
  value = 7732;

  lumiera_thread_join (master);                                         //////////////////////////////TICKET #803  deadlock here
  CHECK (value == 42*2-13, "result is not 42*2-12=71, but %d", value);

  lumiera_threadpool_destroy ();
}

TESTS_END
