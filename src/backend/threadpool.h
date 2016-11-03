/*
  THREADPOOL.h  -  Manage pools of threads

  Copyright (C)         Lumiera.org
    2009,               Michael Ploujnikov <ploujj@gmail.com>

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


/** @file threadpool.h
 ** @todo development in this area is stalled since 2010
 */


#ifndef BACKEND_THREADPOOL_H
#define BACKEND_THREADPOOL_H

#include "lib/condition.h"
#include "lib/llist.h"
#include "threads.h"

#include <nobug.h>




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
 * Park a thread
 * This ends up putting a finished thread back on the list of an appropriate threadpool.
 * This function doesn't need to be accessible outside of the threadpool implementation.
 */
void
lumiera_threadpool_release_thread(LumieraThread thread);

typedef struct lumiera_threadpool_struct lumiera_threadpool;
typedef lumiera_threadpool* LumieraThreadpool;

enum lumiera_threadpool_state {
  LUMIERA_THREADPOOL_OFFLINE,
  LUMIERA_THREADPOOL_ONLINE
};

struct lumiera_threadpool_struct
{
  struct
  {
    llist working_list;
    llist idle_list;
    pthread_attr_t pthread_attrs;
    lumiera_condition sync;
    enum lumiera_threadpool_state status;
  } pool[LUMIERA_THREADCLASS_COUNT];
};


/** Initialise the thread pool. */
void
lumiera_threadpool_init(void);

void
lumiera_threadpool_destroy(void);


#endif /*BACKEND_THREADPOOL_H*/
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
