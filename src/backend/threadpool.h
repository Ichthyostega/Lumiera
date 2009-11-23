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
LumieraThreadpool
lumiera_threadpool_new(enum lumiera_threadpool_type type);

/**
 * Delete/remove a threadpool.
 */
void
lumiera_threadpool_delete(LumieraThreadpool threadpool);


/**
 * Acquire a thread from a threadpool.
 * This may either pick a thread from the list or create a new one when the list is empty.
 * This function doesn't need to be accessible outside of the threadpool implementation.
 */
LumieraThread
lumiera_threadpool_acquire_thread(enum lumiera_thread_class kind,
                                  const char* purpose,
                                  struct nobug_flag* flag);

/**
 * Release a thread
 * This ends up putting a (parked/idle) thread back on the list of a threadpool.
 * This function doesn't need to be accessible outside of the threadpool implementation.
 */
void
lumiera_threadpool_release_thread(LumieraThread thread);


#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
