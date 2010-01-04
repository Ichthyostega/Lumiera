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

void
lumiera_threadpool_init(void)
{
  NOBUG_INIT_FLAG (threadpool);
  TRACE (threadpool);

  for (int i = 0; i < LUMIERA_THREADCLASS_COUNT; ++i)
    {
      llist_init (&threadpool.pool[i].list);
      threadpool.pool[i].working_thread_count = 0;
      threadpool.pool[i].idle_thread_count = 0;

      //TODO: configure each pools' pthread_attrs appropriately
      pthread_attr_init (&threadpool.pool[i].pthread_attrs);
      //cancel...

      lumiera_condition_init (&threadpool.pool[i].sync,"pool of threads", &NOBUG_FLAG (threadpool));
    }
}

void
lumiera_threadpool_destroy(void)
{
  TRACE (threadpool);
  for (int i = 0; i < LUMIERA_THREADCLASS_COUNT; ++i)
    {
      TRACE (threadpool, "destroying individual pool #%d", i);
      LUMIERA_CONDITION_SECTION (threadpool, &threadpool.pool[i].sync)
        {
          REQUIRE (0 == threadpool.pool[i].working_thread_count, "%d threads are still running", threadpool.pool[i].working_thread_count);
          // TODO need to have a stronger assertion that no threads are really running because they will not even be in the list
          INFO (threadpool, "number of threads in the pool=%d", llist_count(&threadpool.pool[i].list));
          LLIST_WHILE_HEAD (&threadpool.pool[i].list, t)
            {
              lumiera_thread_delete ((LumieraThread)t);
            }
        }
      lumiera_condition_destroy (&threadpool.pool[i].sync, &NOBUG_FLAG (threadpool));
      pthread_attr_destroy (&threadpool.pool[i].pthread_attrs);
    }
}

LumieraThread
lumiera_threadpool_acquire_thread(enum lumiera_thread_class kind,
                                  const char* purpose,
                                  struct nobug_flag* flag)
{
  TRACE (threadpool);
  LumieraThread ret = NULL;

  REQUIRE (kind < LUMIERA_THREADCLASS_COUNT, "unknown pool kind specified: %d", kind);

  LUMIERA_CONDITION_SECTION (threadpool, &threadpool.pool[kind].sync)
    {
      if (llist_is_empty (&threadpool.pool[kind].list))
        {
          ret = lumiera_thread_new (kind, purpose, flag,
                                    &threadpool.pool[kind].pthread_attrs);
          ENSURE (ret, "did not create a valid thread");
          TODO ("no error handing, let the resourcecollector do it, no need when returning the thread");
          LUMIERA_CONDITION_WAIT (!llist_is_empty (&threadpool.pool[kind].list));
        }
      // use an existing thread, pick the first one
      // remove it from the pool's list
      ret = (LumieraThread)(llist_unlink (llist_head (&threadpool.pool[kind].list)));
      REQUIRE (ret->state == LUMIERA_THREADSTATE_IDLE, "trying to return a non-idle thread (state=%s)", lumiera_threadstate_names[ret->state]);
      threadpool.pool[kind].working_thread_count++;
      threadpool.pool[kind].idle_thread_count--; // cheaper than using llist_count
      ENSURE (threadpool.pool[kind].idle_thread_count ==
              llist_count (&threadpool.pool[kind].list),
              "idle thread count %d is wrong, should be %d",
              threadpool.pool[kind].idle_thread_count,
              llist_count (&threadpool.pool[kind].list));
      ENSURE (ret, "did not find a valid thread");
    }
  return ret;
}

void
lumiera_threadpool_release_thread(LumieraThread thread)
{
  TRACE (threadpool);
  REQUIRE (thread, "invalid thread given");
  REQUIRE (thread->kind < LUMIERA_THREADCLASS_COUNT, "thread belongs to an unknown pool kind: %d", thread->kind);

  REQUIRE (thread->state != LUMIERA_THREADSTATE_IDLE, "trying to park an already idle thread");
  LUMIERA_CONDITION_SECTION (threadpool, &threadpool.pool[thread->kind].sync)
    {
      thread->state = LUMIERA_THREADSTATE_IDLE;
      REQUIRE (llist_is_empty (&thread->node), "thread already belongs to some list");
      llist_insert_head (&threadpool.pool[thread->kind].list, &thread->node);

      threadpool.pool[thread->kind].working_thread_count--;
      threadpool.pool[thread->kind].idle_thread_count++; // cheaper than using llist_count

      ENSURE (threadpool.pool[thread->kind].idle_thread_count ==
              llist_count (&threadpool.pool[thread->kind].list),
              "idle thread count %d is wrong, should be %d",
              threadpool.pool[thread->kind].idle_thread_count,
              llist_count (&threadpool.pool[thread->kind].list));
      REQUIRE (!llist_is_empty (&threadpool.pool[thread->kind].list), "thread pool is still empty after insertion");
      LUMIERA_CONDITION_BROADCAST;
    }
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
