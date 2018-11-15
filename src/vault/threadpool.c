/*
  Threadpool  -  Manage pools of threads

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


/** @file threadpool.c
 ** Implementation of a threadpool.
 ** The plan is to manage the massively parallel activities by a scheduler.
 ** @todo development in this area is stalled since 2010
 */


#include "include/logging.h"
#include "lib/safeclib.h"
#include "vault/threadpool.h"

#include <pthread.h>


LUMIERA_ERROR_DEFINE(THREADPOOL_OFFLINE, "tried to acquire thread while threadpool is not available");



static lumiera_threadpool threadpool;



void
lumiera_threadpool_init(void)
{
  TRACE (threadpool);

  for (int i = 0; i < LUMIERA_THREADCLASS_COUNT; ++i)
    {
      llist_init (&threadpool.pool[i].working_list);
      llist_init (&threadpool.pool[i].idle_list);
      threadpool.pool[i].status = LUMIERA_THREADPOOL_ONLINE;

      //TODO: configure each pools' pthread_attrs appropriately
      pthread_attr_init (&threadpool.pool[i].pthread_attrs);
      //cancel...

      lumiera_condition_init (&threadpool.pool[i].sync,"pool of threads", &NOBUG_FLAG (threadpool), NOBUG_CONTEXT);
    }
}

void
lumiera_threadpool_destroy(void)
{
  TRACE (threadpool);

  /* set all threadpools offline must be done first, since running threads may attempt to start new ones */
  for (int i = 0; i < LUMIERA_THREADCLASS_COUNT; ++i)
    LUMIERA_CONDITION_SECTION (cond_sync, &threadpool.pool[i].sync)
      threadpool.pool[i].status = LUMIERA_THREADPOOL_OFFLINE;

  /* wait that all threads have finished */
  for (int i = 0; i < LUMIERA_THREADCLASS_COUNT; ++i)
    {
      LUMIERA_CONDITION_SECTION (cond_sync, &threadpool.pool[i].sync)
        {
          //////////////////////////////////////////TICKET #843 check threads deadlines, kill them when they are stalled"
          //////////////////////////////////////////TICKET #843 for threads without deadline use a timeout from config system, 500ms or so by default

          LUMIERA_CONDITION_WAIT(llist_is_empty (&threadpool.pool[i].working_list));
        }
    }

  /* now we can delete all threads */
  for (int i = 0; i < LUMIERA_THREADCLASS_COUNT; ++i)
    {
      TRACE (threadpool, "destroying individual pool #%d", i);

      LUMIERA_CONDITION_SECTION (cond_sync, &threadpool.pool[i].sync)
        {
          ENSURE (llist_is_empty (&threadpool.pool[i].working_list),
                  "threads are still running");

          LLIST_WHILE_HEAD (&threadpool.pool[i].idle_list, t)
            {
              lumiera_thread_delete ((LumieraThread)t);
            }
        }
      lumiera_condition_destroy (&threadpool.pool[i].sync, &NOBUG_FLAG (threadpool), NOBUG_CONTEXT);
      pthread_attr_destroy (&threadpool.pool[i].pthread_attrs);
    }
}

/**
 * @return thread handle or NULL on error (lumiera error will be set)
 */
LumieraThread
lumiera_threadpool_acquire_thread (enum lumiera_thread_class kind,
                                   const char* purpose,
                                   struct nobug_flag* flag)
{
  TRACE (threadpool);
  LumieraThread ret = NULL;

  REQUIRE (kind < LUMIERA_THREADCLASS_COUNT, "unknown pool kind specified: %d", kind);

  LUMIERA_CONDITION_SECTION (cond_sync, &threadpool.pool[kind].sync)
    {
      if (threadpool.pool[kind].status != LUMIERA_THREADPOOL_ONLINE)
        LUMIERA_ERROR_SET_WARNING (threadpool, THREADPOOL_OFFLINE, purpose);
      else
        {
          if (llist_is_empty (&threadpool.pool[kind].idle_list))
            {
              ret = lumiera_thread_new (kind, purpose, flag,
                                        &threadpool.pool[kind].pthread_attrs);
              TRACE (threadpool, "created thread %p", ret);

              /*
                a newly created thread flows somewhere in the air; it is not yet released into the idle list,
                nor in the working list, While we are holding this CONDITION_SECION we can safely put it on the working list,
                this removes a small race.
              */
              llist_insert_head (&threadpool.pool[kind].working_list, &ret->node);

              ENSURE (ret, "did not create a valid thread");
                //////////////////////////////////////////////////////////////////////TICKET #844 no error must be pending here
                //////////////////////////////////////////////////////////////////////TICKET #844 let the resourcecollector do it, no need when returning the thread
                
              LUMIERA_CONDITION_WAIT (!llist_is_empty (&threadpool.pool[kind].idle_list));
            }
          // use an existing thread, pick the first one
          // remove it from the pool's list
          ret = (LumieraThread) (llist_head (&threadpool.pool[kind].idle_list));
          TRACE (threadpool, "got thread %p", ret);

          REQUIRE (ret->state == LUMIERA_THREADSTATE_IDLE, "trying to return a non-idle thread (state=%s)", lumiera_threadstate_names[ret->state]);

          // move thread to the working_list
          llist_insert_head (&threadpool.pool[kind].working_list, &ret->node);
        }
    }
  return ret;
}

void
lumiera_threadpool_release_thread(LumieraThread thread)
{
  TRACE (threadpool);
  REQUIRE (thread, "invalid thread given");
  thread->kind = thread->kind&0xff;
  REQUIRE (thread->kind < LUMIERA_THREADCLASS_COUNT, "thread belongs to an unknown pool kind: %d", thread->kind);

  REQUIRE (thread->state != LUMIERA_THREADSTATE_IDLE, "trying to park an already idle thread");
  LUMIERA_CONDITION_SECTION (cond_sync, &threadpool.pool[thread->kind].sync)
    {
      REQUIRE (!llist_is_member (&threadpool.pool[thread->kind].idle_list, &thread->node), "thread is already in the idle list");
      REQUIRE (llist_is_member (&threadpool.pool[thread->kind].working_list, &thread->node)
               || thread->state == LUMIERA_THREADSTATE_STARTUP,
               "thread is not in the working list (state=%s)",
               lumiera_threadstate_names[thread->state]);
      thread->state = LUMIERA_THREADSTATE_IDLE;
      // move thread to the idle_list
      llist_insert_head (&threadpool.pool[thread->kind].idle_list, &thread->node);

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
