/*
  THREADS.h  -  Helper for managing threads

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

*/


/** @file threads.h
 ** Lumiera low-level thread handling framework.
 ** Exposed a limited set of operations to deal with parallelism,
 ** while threads and thread creation is managed by a threadpool.
 ** @todo development in this area is stalled since 2010
 */


#ifndef VAULT_THREADS_H
#define VAULT_THREADS_H

#include "lib/condition.h"

#include <nobug.h>



typedef struct lumiera_thread_struct lumiera_thread;
typedef lumiera_thread* LumieraThread;

/** Helper macro used for an enum string trick */
#define LUMIERA_THREAD_CLASSES                  \
  /** mostly idle, low latency **/              \
  LUMIERA_THREAD_CLASS(INTERACTIVE)             \
  /** busy at average priority **/              \
  LUMIERA_THREAD_CLASS(WORKER)                  \
  /** busy, soft realtime, high priority **/    \
  LUMIERA_THREAD_CLASS(URGENT)                  \
  /** high latency, background jobs **/         \
  LUMIERA_THREAD_CLASS(BATCH)                   \
  /** Something to do when there is really nothing else to do **/       \
  LUMIERA_THREAD_CLASS(IDLE)

/** Macro for enum string trick: expands as an enum of thread classes */
#define LUMIERA_THREAD_CLASS(name) LUMIERA_THREADCLASS_##name,



/**
 * Thread classes.
 * We define some 'classes' of threads for different purposes to abstract
 * priorities and other attributes.
 */
enum lumiera_thread_class
  {
    LUMIERA_THREAD_CLASSES
    /** this just denotes the number of classes listed above,
        it is used to create arrays **/
    LUMIERA_THREADCLASS_COUNT,  /* must be <= 256, thats easy or? */

    // .. various thread flags follow
    /**
     * flag to let the decision to run the function in a thread open to the vault.
     * depending on load it might decide to run it sequentially.
     * This has some constraints:
     * The Thread must be very careful with locking, better don't.
     * @todo explain synchronisation issues
     **/
    LUMIERA_THREAD_OR_NOT = 1<<8,

    /**
     * Thread must be joined finally
     **/
    LUMIERA_THREAD_JOINABLE = 1<<9
  };

#undef LUMIERA_THREAD_CLASS

// defined in threads.c
extern const char* lumiera_threadclass_names[];


// there is some confusion between the meaning of this
// on one hand it could be used to tell the current state of the thread
// on the other, it is used to tell the thread which state to enter on next iteration
#define LUMIERA_THREAD_STATES                        \
  LUMIERA_THREAD_STATE(ERROR)                        \
  LUMIERA_THREAD_STATE(IDLE)                         \
  LUMIERA_THREAD_STATE(RUNNING)                      \
  LUMIERA_THREAD_STATE(SYNCING)                      \
  LUMIERA_THREAD_STATE(WAKEUP)                       \
  LUMIERA_THREAD_STATE(SHUTDOWN)                     \
  LUMIERA_THREAD_STATE(ZOMBIE)                       \
  LUMIERA_THREAD_STATE(JOINED)                       \
  LUMIERA_THREAD_STATE(STARTUP)

#define LUMIERA_THREAD_STATE(name) LUMIERA_THREADSTATE_##name,

/**
 * Thread state.
 * These are the only states our threads can be in.
 */
typedef enum
  {
    LUMIERA_THREAD_STATES
  }
  lumiera_thread_state;

#undef LUMIERA_THREAD_STATE

// defined in threads.c
extern const char* lumiera_threadstate_names[];

#include "threadpool.h"



/**
 * The actual thread data
 */
struct lumiera_thread_struct
{
  llist node; // this should be first for easy casting

  pthread_t id;

  // TODO: maybe this condition variable should be renamed when we have a better understanding of how it will be used
  lumiera_condition signal; // control signal, state change signal

  struct timespec deadline;

  struct nobug_resource_user** rh;

  // the following member could have been called "class" except that it would conflict with C++ keyword
  // as consequence, it's been decided to leave the type name containing the word "class",
  // while all members/variables called "kind"
  int kind;

  // this is used both as a command and as a state tracker
  lumiera_thread_state state;
  void (*function)(void *);
  void * arguments;
};


/**
 * Create a thread structure.
 */
LumieraThread
lumiera_thread_new (enum lumiera_thread_class kind,
                    const char* purpose,
                    struct nobug_flag* flag,
                    pthread_attr_t* attrs);

/**
 * Destroy and de-initialise a thread structure.
 * Memory is not freed by this function.
 */
LumieraThread
lumiera_thread_destroy (LumieraThread self);

/**
 * Actually free the memory used by the thread structure.
 * Make sure to destroy the structure first.
 */
void
lumiera_thread_delete (LumieraThread self);

/**
 * Start a thread.
 * Threads are implemented as procedures which take a void* and don't return anything.
 * When a thread wants to pass something back to the application it should use the void* it got for
 * constructing the return.
 *  * Threads must complete (return from their thread function)
 *  * They must not call any exit() function.
 *  * Threads can not be cancelled
 *  * Threads shall not handle signals (all signals will be disabled for them) unless explicitly acknowledged
 *
 * @param kind class of the thread to start
 * @param function pointer to a function to execute in a thread (returning void, not void* as in pthreads)
 * @param arg generic pointer passed to the thread
 * @param purpose descriptive name of this thread, used by NoBug
 * @param flag NoBug flag used for logging the thread startup and return
 */
LumieraThread
lumiera_thread_run (int kind,
                    void (*function)(void *),
                    void * arg,
                    const char* purpose,
                    struct nobug_flag* flag);

/**
 * Query the LumieraThread handle of the current thread
 *
 *
 * @return pointer to the (opaque) handle of the current lumiera thread or NULL when this is not a lumiera thread
 */
LumieraThread
lumiera_thread_self (void);



/**
 * Set a thread deadline.
 * A thread must finish before its deadline is hit. Otherwise it counts as stalled
 * which is a fatal error which might pull the application down.
 *
 * \par Heartbeat and Deadlines.
 *
 * Any thread can have an optional 'deadline' which must never be hit.
 * This deadlines are lazily checked and if hit this is a fatal error which triggers
 * an emergency shutdown. Thus threads are obliged to set and extend their deadlines
 * accordingly.
 */
LumieraThread
lumiera_thread_deadline_set (struct timespec deadline);


/**
 * Extend the deadline of a thread
 * sets the deadline to \c NOW+ms in future. This can be used to implement a heartbeat.
 */
LumieraThread
lumiera_thread_deadline_extend (unsigned ms);


/**
 * Clear a thread's deadline
 * Threads without deadline will not be checked against deadlocks (this is the default)
 */
LumieraThread
lumiera_thread_deadline_clear (void);



/**
 * Synchronise with another threads state.
 * This blocks until/unless the other thread reaches a synchronisation point.
 * 
 * \par Thread synchronisation
 * The synchronisation primitives act as barrier over 2 threads, any thread reaching
 * a synchronisation point first is blocked until the other one reaches it too.
 */
LumieraThread
lumiera_thread_sync_other (LumieraThread other);

/**
 * Synchronise current thread
 *
 * this blocks until/unless the other thread reaches a synchronisation point
 * @return on success pointer to self (opaque), or NULL on error
 */
LumieraThread
lumiera_thread_sync (void);

// TODO implement timedsync, this is bit tricky because after a timeout, synchronisation points are desynched
// we possibly need some way to reset/resync this
//LumieraThread
//lumiera_thread_timedsync (struct timespec timeout);


/**
 * Joining threads
 * a thread can be set up with the LUMEIRA_THREAD_JOINABLE flag, if so
 * then it must be joined finally. Joining clears the error state of the joined thread
 * and returns it to the joiner.
 *
 */
lumiera_err
lumiera_thread_join (LumieraThread thread);

#endif /*VAULT_THREADS_H*/
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
