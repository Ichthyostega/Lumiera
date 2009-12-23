/*
  threads.h  -  Manage threads

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

#ifndef LUMIERA_THREADS_H
#define LUMIERA_THREADS_H

//TODO: Support library includes//
#include "lib/reccondition.h"


//TODO: Forward declarations//


//TODO: Lumiera header includes//


//TODO: System includes//
#include <nobug.h>


/**
 * @file
 *
 */

//TODO: declarations go here//

typedef struct lumiera_thread_struct lumiera_thread;
typedef lumiera_thread* LumieraThread;

// this is used for an enum string trick
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

// enum string trick: expands as an enum of thread classes
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
    LUMIERA_THREADCLASS_COUNT,

    // .. various thread flags follow
    /**
     * flag to let the decision to run the function in a thread open to the backend.
     * depending on load it might decide to run it sequentially.
     * This has some constraints:
     *  The condition variable to signal the finish of the thread must not be used.
     *  The Thread must be very careful with locking, better don't.
     **/
    LUMIERA_THREAD_OR_NOT = 1<<16
  };

#undef LUMIERA_THREAD_CLASS

// defined in threads.c
extern const char* lumiera_threadclass_names[];

// there is some confusion between the meaning of this
// on one hand it could be used to tell the current state of the thread
// on the other, it is used to tell the thread which state to enter on next iteration
#define LUMIERA_THREAD_STATES                        \
  LUMIERA_THREAD_STATE(IDLE)                         \
  LUMIERA_THREAD_STATE(ERROR)                        \
  LUMIERA_THREAD_STATE(RUNNING)                      \
  LUMIERA_THREAD_STATE(WAKEUP)                       \
  LUMIERA_THREAD_STATE(SHUTDOWN)                     \
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
  // the function and argument can be passed to the thread at creation time
  // void (*function)(void*);
  // void* arg;
  pthread_t id;
  // TODO: maybe this condition variable should be renamed when we have a better understanding of how it will be used
  lumiera_reccondition signal; // control signal, state change signal
  // the following member could have been called "class" except that it would conflict with C++ keyword
  // as consequence, it's been decided to leave the type name containing the word "class",
  // while all members/variables called "kind"
  enum lumiera_thread_class kind;
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

LumieraThread
lumiera_thread_destroy (LumieraThread self);

void
lumiera_thread_delete (LumieraThread self);

/**
 * Start a thread.
 * Threads are implemented as procedures which take a void* and dont return anything.
 * When a thread wants to pass something back to the application it should use the void* it got for
 * constructing the return.
 *  * Threads must complete (return from their thread function)
 *  * They must not call any exit() function.
 *  * Threads are not cancelable
 *  * Threads shall not handle signals (all signals will be disabled for them) unless explicitly acknowledged
 *
 * @param kind class of the thread to start
 * @param function pointer to a function to execute in a thread (returning void, not void* as in pthreads)
 * @param arg generic pointer passed to the thread
 * @param purpose descriptive name of this thread, used by NoBug
 * @param flag NoBug flag used for logging the thread startup and return
 */
LumieraThread
lumiera_thread_run (enum lumiera_thread_class kind,
                    void (*function)(void *),
                    void * arg,
                    const char* purpose,
                    struct nobug_flag* flag);


#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
