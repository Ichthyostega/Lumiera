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

/**
 * Thread classes.
 * We define some 'classes' of threads for different purposes to abstract
 * priorities and other attributes.
 */
enum lumiera_thread_class
  {
    /** mostly idle, low latency **/
    LUMIERA_THREAD_INTERACTIVE,
    /** busy at average priority **/
    LUMIERA_THREAD_WORKER,
    /** busy, soft realtime, high priority **/
    LUMIERA_THREAD_URGEND,
    /** high latency, background jobs **/
    LUMIERA_THREAD_BATCH,
    /** Something to do when there is really nothing else to do **/
    LUMIERA_THREAD_IDLE,

    /**
     * flag to let the decision to run the function in a thread open to the backend.
     * depending on load it might decide to run it sequentially.
     * This has some constraints:
     *  The condition variable to signal the finish of the thread must not be used.
     *  The Thread must be very careful with locking, better don't.
     **/
    LUMIERA_THREAD_OR_NOT = 1<<16
  };

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
 * @param start_routine pointer to a function to execute in a thread (returning void, not void* as in pthreads)
 * @param arg generic pointer passed to the thread
 * @param finished a condition variable to be broadcasted, if not NULL.
 *        The associated mutex should be locked at thread_run time already, else the signal can get lost.
 * @param purpose descriptive name of this thread, used by NoBug
 * @param flag NoBug flag used for logging the thread startup and return
 */
LumieraThread
lumiera_thread_run (enum lumiera_thread_class kind,
                    void (*start_routine)(void *),
                    void * arg,
                    LumieraReccondition finished,
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
