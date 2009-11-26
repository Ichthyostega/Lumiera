/*
  threadpool.c  -  Manage pools of threads

  Copyright (C)         Lumiera.org
    2009,               Michael Ploujnikov <ploujj@gmail.com>

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

//TODO: Support library includes//

#include "include/logging.h"
#include "lib/safeclib.h"

//TODO: Lumiera header includes//
#include "backend/threadpool.h"

//TODO: internal/static forward declarations//
static lumiera_threadpool threadpool;

//TODO: System includes//
#include <pthread.h>

/**
 * @file
 *
 */
NOBUG_DEFINE_FLAG_PARENT (threadpool, threads_dbg); /*TODO insert a suitable/better parent flag here */


//code goes here//

void* pool_thread_loop(void * arg)
{
  (void) arg;
  while (1)
    {
      ;
    }
  return arg;
}

void
lumiera_threadpool_init(unsigned limit)
{
  for (int i = 0; i < LUMIERA_THREADCLASS_COUNT; ++i)
    {
      llist_init(&threadpool.kind[i].pool);
      threadpool.kind[i].max_threads = limit;
      threadpool.kind[i].working_thread_count = 0;
      threadpool.kind[i].idle_thread_count = 0;
      lumiera_mutex_init(&threadpool.kind[i].lock,"pool of threads", &NOBUG_FLAG(threadpool));
    }
}

void
lumiera_threadpool_destroy(void)
{
  ECHO ("destroying threadpool");
  for (int i = 0; i < LUMIERA_THREADCLASS_COUNT; ++i)
    {
      ECHO ("destroying individual pool #%d", i);
      // no locking is done at this point
      ECHO ("number of threads in the pool=%d", llist_count(&threadpool.kind[i].pool));
      LLIST_WHILE_HEAD(&threadpool.kind[i].pool, thread)
        lumiera_thread_delete((LumieraThread)thread);
      ECHO ("destroying the pool mutex");
      lumiera_mutex_destroy (&threadpool.kind[i].lock, &NOBUG_FLAG (threadpool));
      ECHO ("pool mutex destroyed");
    }
}

LumieraThread
lumiera_threadpool_acquire_thread(enum lumiera_thread_class kind,
                                  const char* purpose,
                                  struct nobug_flag* flag)
{
  LumieraThread ret;

  REQUIRE (kind < LUMIERA_THREADCLASS_COUNT, "unknown pool kind specified: %d", kind);
  REQUIRE (&threadpool.kind[kind].pool, "threadpool kind %d does not exist", kind);
  if (llist_is_empty (&threadpool.kind[kind].pool))
    {
      // TODO: fill in the reccondition argument, currently NULL
      FIXME ("this max thread logic needs to be deeply thought about and made more efficient as well as rebust");
      if (threadpool.kind[kind].working_thread_count
          + threadpool.kind[kind].idle_thread_count
          < threadpool.kind[kind].max_threads) {
        ret = lumiera_thread_new (kind, NULL, purpose, flag);
        threadpool.kind[kind].working_thread_count++;
        ENSURE (ret, "did not create a valid thread");
      }
      else
        {
          //ERROR (threadpool, "did not create a new thread because per-pool limit was reached: %d", threadpool.kind[kind].max_threads);
          LUMIERA_DIE(ERRNO);
        }
    }
 else
   {
     // use an existing thread, pick the first one
     // remove it from the pool's list
     LUMIERA_MUTEX_SECTION (threadpool, &threadpool.kind[kind].lock)
       {
         ret = (LumieraThread)(llist_unlink(llist_head (&threadpool.kind[kind].pool)));
         threadpool.kind[kind].working_thread_count++;
         threadpool.kind[kind].idle_thread_count--; // cheaper than using llist_count
         ENSURE (threadpool.kind[kind].idle_thread_count ==
                 llist_count(&threadpool.kind[kind].pool),
                 "idle thread count %d is wrong, should be %d",
                 threadpool.kind[kind].idle_thread_count,
                 llist_count(&threadpool.kind[kind].pool));
       }
     ENSURE (ret, "did not find a valid thread");
   }
  return ret;
}

void
lumiera_threadpool_release_thread(LumieraThread thread)
{
  REQUIRE (thread, "invalid thread given");
  REQUIRE (thread->kind < LUMIERA_THREADCLASS_COUNT, "thread belongs to an unknown pool kind: %d", thread->kind);
  REQUIRE (&threadpool.kind[thread->kind].lock, "invalid threadpool lock");

  // TOOD: currently, locking produces memory leaks
  //  LUMIERA_MUTEX_SECTION (threadpool, &threadpool.kind[thread->kind].lock)
  //    {
      REQUIRE (llist_is_single(&thread->node), "thread already belongs to some list");
      llist_insert_head(&threadpool.kind[thread->kind].pool, &thread->node);
         threadpool.kind[thread->kind].working_thread_count--;
         threadpool.kind[thread->kind].idle_thread_count++; // cheaper than using llist_count
         ENSURE (threadpool.kind[thread->kind].idle_thread_count ==
                 llist_count(&threadpool.kind[thread->kind].pool),
                 "idle thread count %d is wrong, should be %d",
                 threadpool.kind[thread->kind].idle_thread_count,
                 llist_count(&threadpool.kind[thread->kind].pool));
      //      REQUIRE (!llist_is_empty (&threadpool.kind[thread->kind].pool), "thread pool is still empty after insertion");
      //    }
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
