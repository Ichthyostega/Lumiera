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

//TODO: Lumiera header includes//
#include "threads.h"

//TODO: internal/static forward declarations//


//TODO: System includes//
#include <pthread.h>

/**
 * @file
 *
 */

//NOBUG_DEFINE_FLAG_PARENT (threads, lumiera); /*TODO insert a suitable/better parent flag here */


//code goes here//


struct lumiera_thread_mockup
{
  void (*fn)(void*);
  void* arg;
  LumieraReccondition finished;
};


static void* pthread_runner (void* thread)
{
  pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, NULL);

  struct lumiera_thread_mockup* starter = (struct lumiera_thread_mockup*) thread;

  starter->fn (starter->arg);

  LUMIERA_RECCONDITION_SECTION(cond_sync, starter->finished)
    LUMIERA_RECCONDITION_BROADCAST;

  return NULL;
}


static pthread_once_t attr_once = PTHREAD_ONCE_INIT;
static pthread_attr_t attrs;

static void thread_attr_init (void)
{
  pthread_attr_init (&attrs);
  pthread_attr_setdetachstate (&attrs, PTHREAD_CREATE_DETACHED);
  //cancel ...
}

 
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


/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
