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

static void* thread_loop (void* arg)
{
  (void)arg;
  return 0;
}

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
  LumieraThread self = lumiera_threadpool_acquire_thread (kind, purpose, flag);

  // TODO: set the function and data to be run
  //  lumiera_thread_set_func_data (self, start_routine, arg, purpose, flag);

  // and let it really run (signal the condition var, the thread waits on it)
  LUMIERA_RECCONDITION_SECTION (cond_sync, self->finished)
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
                    struct nobug_flag* flag,
                    pthread_attr_t* attrs)
{
  // TODO: do something with these:
  (void) purpose;
  (void) flag;
  REQUIRE (kind < LUMIERA_THREADCLASS_COUNT, "invalid thread kind specified: %d", kind);
  REQUIRE (attrs, "invalid pthread attributes structure passed");

  //REQUIRE (finished, "invalid finished flag passed");


  LumieraThread self = lumiera_malloc (sizeof (*self));
  llist_init (&self->node);
  self->finished = finished;
  self->kind = kind;
  self->state = LUMIERA_THREADSTATE_IDLE;

  //REQUIRE (thread_loop);
  int error = pthread_create (&self->id, attrs, &thread_loop, self);
  ENSURE (error == 0 || EAGAIN == error, "pthread returned %d:%s", error, strerror (error));
  if (error)
    {
      // error here can only be EAGAIN, given the above ENSURE
      FIXME ("error is %d:%s, see if this can be improved", error, strerror(error));
      LUMIERA_DIE (ERRNO);
    }
  return self;
}

LumieraThread
lumiera_thread_destroy (LumieraThread self)
{
  REQUIRE (self, "trying to destroy an invalid thread");

  // TODO: stop the pthread
  llist_unlink (&self->node);
  //finished = NULL; // or free(finished)?
  lumiera_reccondition_destroy (self->finished, &NOBUG_FLAG (threads));
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
