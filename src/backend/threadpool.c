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
/**
 * Find a suitable thread pool for the given thread type.
 */
static
LumieraThreadpool
lumiera_threadpool_findpool(enum lumiera_thread_class kind);

typedef struct lumiera_threadpoolmanager_struct lumiera_threadpoolmanager;
typedef lumiera_threadpoolmanager* LumieraThreadpoolmanager;

struct lumiera_threadpoolmanager_struct
{
  llist pools;
};

/**
 * Create the singleton threadpool manager
 */
static
void
lumiera_threadpoolmanager_new(void);

/**
 * Delete/remove the singleton threadpool manager
 */
static
void
lumiera_threadpoolmanager_delete(void);

// singleton threadpool manager instance
static LumieraThreadpoolmanager lumiera_tpmanager;

typedef struct lumiera_threadpool_struct lumiera_threadpool;
typedef lumiera_threadpool* LumieraThreadpool;

enum lumiera_threadpool_type
  {
    // TODO: the following types need to be more thought out:
    /** the default kind of threadpool **/
    LUMIERA_DEFAULT_THREADPOOL,
    /** a threadpool for special threads **/
    LUMIERA_SPECIAL_THREADPOOL
  };

struct lumiera_threadpool_struct
{
  /* a list of threadpools for a threadpool manager */
  llist node;

  enum lumiera_threadpool_type type;

  llist threads;
};

/**
 * Create a thread pool.
 */
static
LumieraThreadpool
lumiera_threadpool_new(enum lumiera_threadpool_type type);

/**
 * Delete/remove a threadpool.
 */
static
void
lumiera_threadpool_delete(LumieraThreadpool threadpool);

//TODO: System includes//
#include <pthread.h>

/**
 * @file
 *
 */

//NOBUG_DEFINE_FLAG_PARENT (threadpool, lumiera); /*TODO insert a suitable/better parent flag here */


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

static pthread_once_t attr_once = PTHREAD_ONCE_INIT;
static pthread_attr_t attrs;

static void thread_attr_init (void)
{
  pthread_attr_init (&attrs);
  pthread_attr_setdetachstate (&attrs, PTHREAD_CREATE_DETACHED);
  //cancel ...
}

LumieraThread
lumiera_thread_new (enum lumiera_thread_class kind,
                    LumieraReccondition finished,
                    const char* purpose,
                    struct nobug_flag* flag)
{
  (void) kind;
  (void) purpose;
  (void) flag;

  if (attr_once == PTHREAD_ONCE_INIT)
    pthread_once (&attr_once, thread_attr_init);

  LumieraThread thread = lumiera_malloc (sizeof(*thread));

  thread->finished = finished;

  pthread_t dummy;
  printf("creating a thread\n");
  int error = pthread_create (&dummy, &attrs, &pool_thread_loop, thread);

  if (error) return 0;        /////TODO temporary addition by Ichthyo; probably we'll set lumiera_error?
  return thread;
}

LumieraThreadpool
lumiera_threadpool_new(enum lumiera_threadpool_type type)
{
  LumieraThreadpool self = lumiera_malloc (sizeof (*self));
  self->type = type;

  /* we start with no threads in the list */
  llist_init (&self->threads);

  return self;
}

LumieraThreadpoolmanager lumiera_tpmanager = NULL;

void
lumiera_threadpoolmanager_new()
{
  REQUIRE (!lumiera_tpmanager, "Threadpool manager already initialized");
  lumiera_tpmanager = lumiera_malloc (sizeof (lumiera_threadpoolmanager));
  llist_init (&lumiera_tpmanager->pools);

  // create a default threadpool
  LumieraThreadpool default_tp = lumiera_threadpool_new(LUMIERA_DEFAULT_THREADPOOL);
  llist_insert_head (&lumiera_tpmanager->pools, &default_tp->node);
};

LumieraThread
lumiera_threadpool_acquire_thread(enum lumiera_thread_class kind,
                                  const char* purpose,
                                  struct nobug_flag* flag)
{
  LumieraThreadpool tp =
    lumiera_threadpool_findpool(enum lumiera_thread_class kind);

  if (!tp)
    {
      // ERROR: could not find a suitable threadpool or there are no threadpools
      return (LumieraThreadpool)0;
    }

  // TODO: either get a thread from the pool or create a new one
}

LumieraThreadpool
lumiera_threadpool_findpool(enum lumiera_thread_class thread_kind)
{
  if (!lumiera_tpmanager)
    {
      // create a threadpool manager if it doesn't exist yet
      lumiera_threadpoolmanager_new();
    }
  if (!lumiera_tpmanager)
    {
      // ERROR: if the threadpool manager still doesn't exist, something went wrong
      return (LumieraThreadpool)0;
    }
  else
    {
      // for now, ignore the thread all together and just return the first threadpool
      (void) thread_kind;
      // BUG: what if there are no pools in the list???
      // where/when should I check for that case?
      return (LumieraThreadpool)(llist_head (&lumiera_tpmanager->pools));
    }
}

void
lumiera_threadpoolmanager_new(void)
{
  REQUIRE (!lumiera_tpmanager, "Threadpool manager already initialized");

  lumiera_tpmanager = lumiera_malloc (sizeof (lumiera_tpmanager));
  llist_init (&lumiera_tpmanager->pools);

  // start with just one default threadpool in the list
  LumieraThreadpool tp =
    lumiera_threadpool_new(LUMIERA_DEFAULT_THREADPOOL);
  llist_insert_head (&lumiera_tp->pools, &tp->node);
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
