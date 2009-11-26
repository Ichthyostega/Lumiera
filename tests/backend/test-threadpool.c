/*
  test-threadpool.c  -  test thread pool creation and usage

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

#include "tests/test.h"

#include "backend/threadpool.h"

#include <stdio.h>
#include <string.h>

TESTS_BEGIN


TEST ("basic-acquire-release")
{
  ECHO("start by initializing the threadpool");
  lumiera_threadpool_init();
  ECHO("acquiring thread 1");
  LumieraThread t1 =
    lumiera_threadpool_acquire_thread(LUMIERA_THREAD_INTERACTIVE,
				      "test purpose",
				      NULL);
  ECHO("acquiring thread 2");
  LumieraThread t2 =
    lumiera_threadpool_acquire_thread(LUMIERA_THREAD_IDLE,
				      "test purpose",
				      NULL);

  //ECHO("thread 1 kind=%d", t1->kind);
  CHECK(LUMIERA_THREAD_INTERACTIVE == t1->kind);
  //ECHO("thread 1 state=%d", t1->state);
  CHECK(LUMIERA_THREADSTATE_IDLE == t1->state);
  //ECHO("thread 2 kind=%d", t2->kind);
   CHECK(LUMIERA_THREAD_IDLE == t2->kind);
  //ECHO("thread 2 state=%d", t2->state);
  CHECK(LUMIERA_THREADSTATE_IDLE == t2->state);

  ECHO("releasing thread 1");
  lumiera_threadpool_release_thread(t1);
  ECHO("thread 1 has been released");

  ECHO("releasing thread 2");
  lumiera_threadpool_release_thread(t2);
  ECHO("thread 2 has been released");

  lumiera_threadpool_destroy();
}

TEST ("many-acquire-release")
{

  const int threads_per_pool_count = 50;

  lumiera_threadpool_init();
  LumieraThread threads[threads_per_pool_count*LUMIERA_THREADCLASS_COUNT];
  
  for (int kind = 0; kind < LUMIERA_THREADCLASS_COUNT; ++kind)
    {
      for (int i = 0; i < threads_per_pool_count; ++i)
	{
	  threads[i+kind*threads_per_pool_count] =
	    lumiera_threadpool_acquire_thread(kind,
					      "test purpose",
					      NULL);
	}
    }

  for (int i = 0; i < threads_per_pool_count*LUMIERA_THREADCLASS_COUNT; ++i)
    {
      lumiera_threadpool_release_thread(threads[i]);
    }

  lumiera_threadpool_destroy();

}

TESTS_END
