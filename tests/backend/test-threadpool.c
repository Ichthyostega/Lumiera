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
  printf("start by initializing the threadpool");
  lumiera_threadpool_init();
  printf("acquiring thread 1\n");
  LumieraThread t1 =
    lumiera_threadpool_acquire_thread(LUMIERA_THREAD_INTERACTIVE,
				      "test purpose",
				      NULL);
  printf("acquiring thread 2\n");
  LumieraThread t2 =
    lumiera_threadpool_acquire_thread(LUMIERA_THREAD_IDLE,
				      "test purpose",
				      NULL);

  printf("thread 1 type=%d\n", t1.type);
  printf("thread 1 state=%d\n", t1.state);
  printf("thread 2 type=%d\n", t2.type);
  printf("thread 2 state=%d\n", t2.state);

  printf("releasing thread 1\n");
  lumiera_threadpool_release_thread(t1);
  printf("thread 1 has been released\n");

  printf("releasing thread 2\n");
  lumiera_threadpool_release_thread(t2);
  printf("thread 2 has been released\n");
}

TESTS_END
