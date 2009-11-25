/*
  threads.c  -  Manage threads

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

//TODO: Support library includes//

#include "include/logging.h"
#include "lib/mutex.h"
#include "lib/safeclib.h"


//TODO: Lumiera header includes//
#include "threads.h"

//TODO: internal/static forward declarations//


//TODO: System includes//
#include <pthread.h>

/**
 * @file
 *
 */

NOBUG_DEFINE_FLAG_PARENT (threads, threads_dbg); /*TODO insert a suitable/better parent flag here */


//code goes here//


struct lumiera_thread_mockup
{
  void (*fn)(void*);
  void* arg;
  LumieraReccondition finished;
};

static void* thread_loop (void* arg)
{
  (void)arg;
  return 0;
}

#if 0
static void* pthread_runner (void* thread)
{
  pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, NULL);

  struct lumiera_thread_mockup* starter = (struct lumiera_thread_mockup*) thread;
  LumieraReccondition thread_end_notification = starter->finished;

  starter->fn (starter->arg);

  if (!thread_end_notification)
    return NULL; // no signalling of thread termination desired

  LUMIERA_RECCONDITION_SECTION(cond_sync, thread_end_notification)
    LUMIERA_RECCONDITION_BROADCAST;

  return NULL;
}
#endif

static pthread_once_t attr_once = PTHREAD_ONCE_INIT;
static pthread_attr_t attrs;

static void thread_attr_init (void)
{
  pthread_attr_init (&attrs);
  pthread_attr_setdetachstate (&attrs, PTHREAD_CREATE_DETACHED);
  //cancel ...
}

#if 0
// TODO: rewrite this using lumiera_threadpool_acquire()
LumieraThread
lumiera_thread_run (enum lumiera_thread_class kind,
                    void (*start_routine)(void *),
                    void * arg,
                    LumieraReccondition finished,
                    const char* purpose,
                    struct nobug_flag* flag)
{
  (void) kind;
  (void) purpose;
  (void) flag;

  if (attr_once == PTHREAD_ONCE_INIT)
    pthread_once (&attr_once, thread_attr_init);

  static struct lumiera_thread_mockup thread;

  thread.fn = start_routine;
  thread.arg = arg;
  thread.finished = finished;

  pthread_t dummy;
  int error = pthread_create (&dummy, &attrs, pthread_runner, &thread);

  if (error) return 0;        /////TODO temporary addition by Ichthyo; probably we'll set lumiera_error?
  return (LumieraThread) 1;
}
#endif

// TODO: new implementation, remove the above one
// maybe this shouldn't return LumieraThread at all
// when this is called it should have already been decided that the function
// shall run in parallel, as a thread
LumieraThread
lumiera_thread_run (enum lumiera_thread_class kind,
                    void (*function)(void *),
                    void * arg,
                    LumieraReccondition finished,
                    const char* purpose,
                    struct nobug_flag* flag)
{
  (void)finished;
  (void)function;
  (void)arg;
  // ask the threadpool for a thread (it might create a new one)
  LumieraThread self = lumiera_threadpool_acquire_thread(kind, purpose, flag);

  // TODO: set the function and data to be run
  //  lumiera_thread_set_func_data (self, start_routine, arg, purpose, flag);

  // and let it really run (signal the condition var, the thread waits on it)
  LUMIERA_RECCONDITION_SECTION(cond_sync, self->finished)
    LUMIERA_RECCONDITION_SIGNAL;

  // NOTE: example only, add solid error handling!

  return self;
}

/**
 * Create a new thread structure with a matching pthread
 */
LumieraThread
lumiera_thread_new (enum lumiera_thread_class kind,
                    LumieraReccondition finished,
                    const char* purpose,
                    struct nobug_flag* flag)
{
  // TODO: do something with these:
  (void) purpose;
  (void) flag;

  if (attr_once == PTHREAD_ONCE_INIT)
    pthread_once (&attr_once, thread_attr_init);

  REQUIRE (kind < LUMIERA_THREADCLASS_COUNT, "invalid thread kind specified: %d", kind);
  //REQUIRE (finished, "invalid finished flag passed");

  LumieraThread self = lumiera_malloc (sizeof (*self));
  ECHO ("allocated thread struct of size %zd", sizeof (*self));
  llist_init(&self->node);
  self->finished = finished;
  self->kind = kind;
  self->state = LUMIERA_THREADSTATE_IDLE;

  REQUIRE (&self->id);
  //REQUIRE (&attrs);
  //REQUIRE (&thread_loop);
  REQUIRE (self);
  int error = pthread_create (&self->id, &attrs, &thread_loop, self);

  if (error)
    {
      free(self);
      return 0;        /////TODO temporary addition by Ichthyo; probably we'll set lumiera_error?
    }

  return self;
}

LumieraThread
lumiera_thread_destroy (LumieraThread self)
{
  REQUIRE (self, "trying to destroy an invalid thread");

  // TODO: stop the pthread
  llist_unlink(&self->node);
  //finished = NULL; // or free(finished)?
  lumiera_reccondition_destroy (self->finished, &NOBUG_FLAG(threads));
  //kind = 0;
  //state = 0;
  return self;
}

void
lumiera_thread_delete (LumieraThread self)
{
  ECHO ("deleting thread");
  lumiera_free (lumiera_thread_destroy (self));
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
