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
#include "lib/safeclib.h"


//TODO: Lumiera header includes//
#include "threads.h"

//TODO: internal/static forward declarations//


//TODO: System includes//
#include <pthread.h>
#include <errno.h>

/**
 * @file
 *
 */

NOBUG_DEFINE_FLAG_PARENT (threads, threads_dbg); /*TODO insert a suitable/better parent flag here */


//code goes here//

#define LUMIERA_THREAD_CLASS(name) #name,
// enum string trick: expands as an array of thread class name strings
const char* lumiera_threadclass_names[] = {
  LUMIERA_THREAD_CLASSES
};

#undef LUMIERA_THREAD_CLASS

#define LUMIERA_THREAD_STATE(name) #name,
const char* lumiera_threadstate_names[] = {
  LUMIERA_THREAD_STATES
};
#undef LUMIERA_THREAD_STATE

static void* thread_loop (void* thread)
{
  TRACE (threads);
  NOBUG_THREAD_ID_SET ("worker");
  LumieraThread t = (LumieraThread)thread;

  pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, NULL);

  REQUIRE (t, "thread does not exist");

  LUMIERA_CONDITION_SECTION (threads, &t->signal)
    {
      do {
        // NULL function means: no work to do
        INFO (threads, "function %p", t->function);
        if (t->function)
          t->function (t->arguments);
        lumiera_threadpool_release_thread(t);
        LUMIERA_CONDITION_WAIT (t->state != LUMIERA_THREADSTATE_IDLE);
        INFO (threads, "Thread awaken with state %d", t->state);
      } while (t->state != LUMIERA_THREADSTATE_SHUTDOWN);
        // SHUTDOWN state

      INFO (threads, "Thread Shutdown");
    }
  return 0;
}

// when this is called it should have already been decided that the function
// shall run in parallel, as a thread
LumieraThread
lumiera_thread_run (enum lumiera_thread_class kind,
                    void (*function)(void *),
                    void * arg,
                    const char* purpose,
                    struct nobug_flag* flag)
{
  TRACE (threads);
  //  REQUIRE (function, "invalid function");

  // ask the threadpool for a thread (it might create a new one)
  LumieraThread self = lumiera_threadpool_acquire_thread (kind, purpose, flag);

  // set the function and data to be run
  self->function = function;
  self->arguments = arg;

  // and let it really run (signal the condition var, the thread waits on it)
  self->state = LUMIERA_THREADSTATE_WAKEUP;

  LUMIERA_CONDITION_SECTION (cond_sync, &self->signal)
    LUMIERA_CONDITION_SIGNAL;

  // NOTE: example only, add solid error handling!

  return self;
}

/**
 * Create a new thread structure with a matching pthread
 */
LumieraThread
lumiera_thread_new (enum lumiera_thread_class kind,
                    const char* purpose,
                    struct nobug_flag* flag,
                    pthread_attr_t* attrs)
{
  // TODO: do something with these:
  (void) purpose;
  REQUIRE (kind < LUMIERA_THREADCLASS_COUNT, "invalid thread kind specified: %d", kind);
  REQUIRE (attrs, "invalid pthread attributes structure passed");

  LumieraThread self = lumiera_malloc (sizeof (*self));
  llist_init (&self->node);
  lumiera_condition_init (&self->signal, "thread-control", flag);
  self->kind = kind;
  self->state = LUMIERA_THREADSTATE_STARTUP;
  self->function = NULL;
  self->arguments = NULL;

  int error = pthread_create (&self->id, attrs, &thread_loop, self);
  if (error)
    {
      LUMIERA_DIE (ERRNO);
    }
  return self;
}

LumieraThread
lumiera_thread_destroy (LumieraThread self)
{
  TRACE (threads);
  REQUIRE (self, "trying to destroy an invalid thread");

  llist_unlink (&self->node);

  // get the pthread out of the processing loop
  // need to signal to the thread that it should start quitting
  // should this be within the section?
  LUMIERA_CONDITION_SECTION (threads, &self->signal)
    {
      REQUIRE (self->state == LUMIERA_THREADSTATE_IDLE, "trying to delete a thread in state other than IDLE (%s)", lumiera_threadstate_names[self->state]);
      self->state = LUMIERA_THREADSTATE_SHUTDOWN;
      self->function = NULL;
      self->arguments = NULL;
      LUMIERA_CONDITION_SIGNAL;
    }

  int error = pthread_join (self->id, NULL);
  ENSURE (0 == error, "pthread_join returned %d:%s", error, strerror (error));

  // condition has to be destroyed after joining with the thread
  lumiera_condition_destroy (&self->signal, &NOBUG_FLAG (threads));

  return self;
}

void
lumiera_thread_delete (LumieraThread self)
{
  TRACE (threads);
  lumiera_free (lumiera_thread_destroy (self));
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
