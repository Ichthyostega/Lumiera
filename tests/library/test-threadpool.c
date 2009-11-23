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

#include "backend/threads.h"

#include <stdio.h>
#include <string.h>

TESTS_BEGIN


TEST ("faketest")
{
  printf("this is a fake test\n");
  /* create a threadpool manager,
     it will automatically create all the threadpools */
  lumiera_threadpoolmanager_new();
#if 0
  /* create some jobs */
     LumieraJob myjob = lumiera_job_new(functionpointer, parameters);   // create a non timed (immediate job) */

     /* find a suitable threadpool for a given job */
     LumieraThreadpool somepool = lumiera_threadpool_findpool(myjob); //you want to find a pool which is suitable to run a given job by asking: "hey on which pool can I run this job?" */

     /* pass the jobs to the thread pool: */
     lumiera_threadpool_start_job(somepool, myjob);
#endif
     /* wait for the job to finish */

     /* retrive the job result from myjob */
}

TESTS_END
