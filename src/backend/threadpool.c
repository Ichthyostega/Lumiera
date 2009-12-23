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
lumiera_threadpool_init()
{
  for (int i = 0; i < LUMIERA_THREADCLASS_COUNT; ++i)
    {
      llist_init(&threadpool.pool[i].list);
       threadpool.pool[i].working_thread_count = 0;
      threadpool.pool[i].idle_thread_count = 0;

      //TODO: configure each pools' pthread_attrs appropriately
      pthread_attr_init (&threadpool.pool[i].pthread_attrs);
      // cehteh prefers that threads are joinable by default
      //pthread_attr_setdetachstate (&threadpool.pool[i].pthread_attrs, PTHREAD_CREATE_DETACHED);
      //cancel...

      lumiera_mutex_init(&threadpool.pool[i].lock,"pool of threads", &NOBUG_FLAG(threadpool));
      lumiera_reccondition_init (&threadpool.pool[i].signal, "thread-signal", &NOBUG_FLAG(threadpool));
    }
}

void
lumiera_threadpool_destroy(void)
{
  ECHO ("destroying threadpool");
  for (int i = 0; i < LUMIERA_THREADCLASS_COUNT; ++i)
    {
      ECHO ("destroying individual pool #%d", i);
      LUMIERA_MUTEX_SECTION (threadpool, &threadpool.pool[i].lock)
        {
          REQUIRE (0 == threadpool.pool[i].working_thread_count, "%d threads are running", threadpool.pool[i].working_thread_count);
          // TODO need to have a stronger assertion that no threads are really running because they will not even be in the list
          ECHO ("number of threads in the pool=%d", llist_count(&threadpool.pool[i].list));
          LLIST_WHILE_HEAD(&threadpool.pool[i].list, t)
            {
              lumiera_thread_delete((LumieraThread)t);
            }
        }
      ECHO ("destroying the pool mutex");
      lumiera_mutex_destroy (&threadpool.pool[i].lock, &NOBUG_FLAG (threadpool));
      ECHO ("pool mutex destroyed");
      pthread_attr_destroy (&threadpool.pool[i].pthread_attrs);
    }
}

void lumiera_threadpool_unlink(LumieraThread thread)
{
  REQUIRE (thread, "invalid thread given");
  REQUIRE (thread->kind < LUMIERA_THREADCLASS_COUNT, "thread belongs to an unknown pool kind: %d", thread->kind);
  llist_unlink(&thread->node);
  ENSURE (llist_is_empty(&thread->node), "failed to unlink the thread");
}


LumieraThread
lumiera_threadpool_acquire_thread(enum lumiera_thread_class kind,
                                  const char* purpose,
                                  struct nobug_flag* flag)
{
  LumieraThread ret;

  REQUIRE (kind < LUMIERA_THREADCLASS_COUNT, "unknown pool kind specified: %d", kind);
  LUMIERA_RECCONDITION_SECTION (threadpool, &threadpool.pool[kind].signal)
  {
    if (llist_is_empty (&threadpool.pool[kind].list))
      {

        ret = lumiera_thread_new (kind, purpose, flag,
                                  &threadpool.pool[kind].pthread_attrs);
        threadpool.pool[kind].idle_thread_count++;
        ENSURE (ret, "did not create a valid thread");
        LUMIERA_RECCONDITION_WAIT (!llist_is_empty (&threadpool.pool[kind].list));
      }
    // use an existing thread, pick the first one
    // remove it from the pool's list
    ret = (LumieraThread)(llist_unlink(llist_head (&threadpool.pool[kind].list)));
    REQUIRE (ret->state == LUMIERA_THREADSTATE_IDLE, "trying to return a non-idle thread (state=%s)", lumiera_threadstate_names[ret->state]);
    threadpool.pool[kind].working_thread_count++;
    threadpool.pool[kind].idle_thread_count--; // cheaper than using llist_count
    ENSURE (threadpool.pool[kind].idle_thread_count ==
            llist_count(&threadpool.pool[kind].list),
            "idle thread count %d is wrong, should be %d",
            threadpool.pool[kind].idle_thread_count,
            llist_count(&threadpool.pool[kind].list));
    ENSURE (ret, "did not find a valid thread");
  }
 return ret;
}

// TODO: rename to lumiera_threadpool_park_thread
void
lumiera_threadpool_park_thread(LumieraThread thread)
{
  REQUIRE (thread, "invalid thread given");
  REQUIRE (thread->kind < LUMIERA_THREADCLASS_COUNT, "thread belongs to an unknown pool kind: %d", thread->kind);

  REQUIRE (thread->state != LUMIERA_THREADSTATE_IDLE, "trying to park an already idle thread");

  LUMIERA_RECCONDITION_SECTION (threadpool, &threadpool.pool[thread->kind].signal)
    {
      thread->state = LUMIERA_THREADSTATE_IDLE;
      REQUIRE (llist_is_single(&thread->node), "thread already belongs to some list");
      llist_insert_head(&threadpool.pool[thread->kind].list, &thread->node);
         threadpool.pool[thread->kind].working_thread_count--;
         threadpool.pool[thread->kind].idle_thread_count++; // cheaper than using llist_count
         ENSURE (threadpool.pool[thread->kind].idle_thread_count ==
                 llist_count(&threadpool.pool[thread->kind].list),
                 "idle thread count %d is wrong, should be %d",
                 threadpool.pool[thread->kind].idle_thread_count,
                 llist_count(&threadpool.pool[thread->kind].list));
      //      REQUIRE (!llist_is_empty (&threadpool.pool[thread->kind].list), "thread pool is still empty after insertion");
         LUMIERA_RECCONDITION_BROADCAST;
    }
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
