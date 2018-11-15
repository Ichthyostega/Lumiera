/*
  Threads  -  Helper for managing threads

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


/** @file threads.c
 ** Implementation of Lumiera's low-level thread handling framework
 ** @todo development in this area is stalled since 2010
 */


#include "include/logging.h"
#include "lib/safeclib.h"
#include "vault/threads.h"

#include <pthread.h>
#include <time.h>
#include <errno.h>


LUMIERA_ERROR_DEFINE(THREAD, "fatal threads initialisation error");



/** Macro for enum string trick: expands as an array of thread class name strings */
#define LUMIERA_THREAD_CLASS(name) #name,


const char* lumiera_threadclass_names[] = {
  LUMIERA_THREAD_CLASSES
};

#undef LUMIERA_THREAD_CLASS

#define LUMIERA_THREAD_STATE(name) #name,
const char* lumiera_threadstate_names[] = {
  LUMIERA_THREAD_STATES
};
#undef LUMIERA_THREAD_STATE


/** thread local storage pointing back to the thread structure of each thread */
static pthread_key_t  lumiera_thread_tls;
static pthread_once_t lumiera_thread_initialised = PTHREAD_ONCE_INIT;


static void
lumiera_thread_tls_init (void)
{
  if (!!pthread_key_create (&lumiera_thread_tls, NULL))
    LUMIERA_DIE (THREAD);      /* should never happen */
}


static void*
thread_loop (void* thread)
{
  TRACE (threads);
  NOBUG_THREAD_ID_SET ("worker");
  LumieraThread t = (LumieraThread)thread;

  pthread_setspecific (lumiera_thread_tls, t);

  pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, NULL);

  REQUIRE (t, "thread does not exist");

  LUMIERA_CONDITION_SECTION (cond_sync, &t->signal)
    {
      t->rh = &lumiera_lock_section_.rh;

      do {
        lumiera_threadpool_release_thread(t);
        LUMIERA_CONDITION_WAIT (t->state != LUMIERA_THREADSTATE_IDLE);
        TRACE (threads, "Thread awaken with state %s", lumiera_threadstate_names[t->state]);

        // NULL function means: no work to do
        TRACE (threads, "function %p", t->function);
        if (t->function)
          t->function (t->arguments);
        TRACE (threads, "function done");

        if (t->kind & LUMIERA_THREAD_JOINABLE)
          {
            TRACE (threads, "Thread zombified");
            /* move error state to data the other thread will it pick up from there */
            t->arguments = (void*)lumiera_error ();
            t->state = LUMIERA_THREADSTATE_ZOMBIE;
            ERROR_IF (t->arguments, threads, "joinable thread ended with error %s", (char*)t->arguments);

            LUMIERA_CONDITION_SIGNAL;
            LUMIERA_CONDITION_WAIT (t->state == LUMIERA_THREADSTATE_JOINED);
            TRACE (threads, "Thread joined");
        }

      } while (t->state != LUMIERA_THREADSTATE_SHUTDOWN);
      // SHUTDOWN state


      TRACE (threads, "Thread done.");
    }
  //////////////////////////////////////////////////////////////////////TICKET #844 no error must be pending here, else do app shutdown
  return 0;
}


/**
 * @remarks when this is called it should have already been decided
 * that the function shall run in parallel, as a thread.
 */
LumieraThread
lumiera_thread_run (int kind,
                    void (*function)(void *),
                    void * arg,
                    const char* purpose,
                    struct nobug_flag* flag)
{
  TRACE (threads);
  //  REQUIRE (function, "invalid function");

  // ask the threadpool for a thread (it might create a new one)
  LumieraThread self = lumiera_threadpool_acquire_thread (kind&0xff, purpose, flag);

  // set the function and data to be run
  self->function = function;
  self->arguments = arg;
  self->kind = kind;
  self->deadline.tv_sec = 0;

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
  pthread_once (&lumiera_thread_initialised, lumiera_thread_tls_init);

  // TODO: do something with this string:
  (void) purpose;
  REQUIRE (attrs, "invalid pthread attributes structure passed");

  LumieraThread self = lumiera_malloc (sizeof (*self));
  llist_init (&self->node);
  lumiera_condition_init (&self->signal, "thread-control", flag, NOBUG_CONTEXT);
  self->kind = kind;
  self->state = LUMIERA_THREADSTATE_STARTUP;
  self->function = NULL;
  self->arguments = NULL;
  self->deadline.tv_sec = 0;
  self->deadline.tv_nsec = 0;

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
  LUMIERA_CONDITION_SECTION (cond_sync, &self->signal)
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
  lumiera_condition_destroy (&self->signal, &NOBUG_FLAG (threads), NOBUG_CONTEXT);

  return self;
}


void
lumiera_thread_delete (LumieraThread self)
{
  TRACE (threads);
  lumiera_free (lumiera_thread_destroy (self));
}


LumieraThread
lumiera_thread_self (void)
{
  pthread_once (&lumiera_thread_initialised, lumiera_thread_tls_init);
  return pthread_getspecific (lumiera_thread_tls);
}



LumieraThread
lumiera_thread_deadline_set (struct timespec deadline)
{
  TRACE (threads);
  LumieraThread self = lumiera_thread_self ();
  if (self)
    self->deadline = deadline;
  return self;
}



LumieraThread
lumiera_thread_deadline_extend (unsigned ms)
{
  TRACE (threads);
  LumieraThread self = lumiera_thread_self ();
  if (self)
    {
      struct timespec deadline;
      clock_gettime (CLOCK_REALTIME, &deadline);
      deadline.tv_sec += ms / 1000;
      deadline.tv_nsec += 1000000 * (ms % 1000);
      if (deadline.tv_nsec >= 1000000000)
        {
          deadline.tv_sec += (deadline.tv_nsec / 1000000000);
          deadline.tv_nsec %= 1000000000;
        }
      self->deadline = deadline;
    }

  return self;
}



LumieraThread
lumiera_thread_deadline_clear (void)
{
  TRACE (threads);
  LumieraThread self = lumiera_thread_self ();
  if (self)
    {
      self->deadline.tv_sec = 0;
      self->deadline.tv_nsec = 0;
    }
  return self;
}



LumieraThread
lumiera_thread_sync_other (LumieraThread other)
{
  TRACE(threads);

  LUMIERA_CONDITION_SECTION (cond_sync, &other->signal)
    {
      LUMIERA_CONDITION_WAIT (other->state == LUMIERA_THREADSTATE_SYNCING);
      other->state = LUMIERA_THREADSTATE_RUNNING;
      LUMIERA_CONDITION_SIGNAL;
    }
  return other;
}


LumieraThread
lumiera_thread_sync (void)
{
  TRACE(threads);

  LumieraThread self = lumiera_thread_self ();
  REQUIRE(self, "not a lumiera thread");

  self->state = LUMIERA_THREADSTATE_SYNCING;
  lumiera_condition_signal (&self->signal, &NOBUG_FLAG(threads), NOBUG_CONTEXT);

  //////////////////////////////////////////TICKET #843 error handing, maybe timed mutex (using the threads heartbeat timeout, shortly before timeout)

  while (self->state == LUMIERA_THREADSTATE_SYNCING) {
    lumiera_condition_wait (&self->signal, &NOBUG_FLAG(threads), self->rh, NOBUG_CONTEXT);
  }

  return self;
}



lumiera_err
lumiera_thread_join (LumieraThread thread)
{
  TRACE(threads);
  lumiera_err ret = NULL;

  LUMIERA_CONDITION_SECTION (cond_sync, &thread->signal)
    {
      LUMIERA_CONDITION_WAIT (thread->state == LUMIERA_THREADSTATE_ZOMBIE);
      ret = (lumiera_err)thread->arguments;
      ERROR_IF (ret, threads, "thread joined with error %s", ret);

      thread->state = LUMIERA_THREADSTATE_JOINED;
      LUMIERA_CONDITION_SIGNAL;         /* kiss it a last goodbye */
    }
  return ret;
}



/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
