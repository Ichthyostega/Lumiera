/*
  threadpool.h  -  Manage pools of threads

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

#ifndef LUMIERA_THREADPOOL_H
#define LUMIERA_THREADPOOL_H

//TODO: Support library includes//
#include "lib/reccondition.h"
#include "lib/llist.h"
#include "lib/mutex.h"

//TODO: Forward declarations//


//TODO: Lumiera header includes//
#include "threads.h"

//TODO: System includes//
#include <nobug.h>


/**
 * @file
 *
 */

//TODO: declarations go here//

/**
 * Acquire a thread from a threadpool.
 * This may either pick a thread from an appropriate pool or create a new one when the pool is empty.
 * This function doesn't need to be accessible outside of the threadpool implementation.
 */
LumieraThread
lumiera_threadpool_acquire_thread(enum lumiera_thread_class kind,
                                  const char* purpose,
                                  struct nobug_flag* flag);

/**
 * Release a thread
 * This ends up putting a (parked/idle) thread back on the list of an appropriate threadpool.
 * This function doesn't need to be accessible outside of the threadpool implementation.
 */
void
lumiera_threadpool_release_thread(LumieraThread thread);

typedef struct lumiera_threadpool_struct lumiera_threadpool;
typedef lumiera_threadpool* LumieraThreadpool;

struct lumiera_threadpool_struct
{
  struct
  {
    llist pool;
    lumiera_mutex lock;
  } kind[LUMIERA_THREADCLASS_COUNT];
};

/**
 * Initialize the thread pool.
 */
void
lumiera_threadpool_init(void);

#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
