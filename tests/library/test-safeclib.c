/*
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
#include "lib/safeclib.h"
#include "tests/test.h"

#include <sys/time.h>
#include <sys/resource.h>

TESTS_BEGIN

TEST ("allocation0")
{
  lumiera_malloc (0);
  NOTREACHED;
}

TEST ("allocation1024")
{
  void* data[1024];
  for (int i = 0; i < 1024; ++i)
    {
      data[i] = lumiera_malloc (1024);
      ENSURE (data[i]);
    }
  for (int i = 0; i < 1024; ++i)
    {
      free (data[i]);
    }
}

TEST ("allocationtoobig")
{
  struct rlimit rl;
  rl.rlim_cur = 100*1024*1024;
  rl.rlim_max = 100*1024*1024;
  setrlimit (RLIMIT_AS, &rl);
  lumiera_malloc (200*1024*1024);
  NOTREACHED;
}

TEST ("streq")
{
  if (!lumiera_streq ("foo", "foo"))
    LUMIERA_DIE (TEST);
  if (!lumiera_streq (NULL, NULL))
    LUMIERA_DIE (TEST);
  if (!!lumiera_streq (NULL, "foo"))
    LUMIERA_DIE (TEST);
  if (!!lumiera_streq ("foo", NULL))
    LUMIERA_DIE (TEST);
  if (!!lumiera_streq ("foo", "bar"))
    LUMIERA_DIE (TEST);
}

TEST ("tmpbuf")
{
  for (int i = 0; i < 256; ++i)
    {
      char* buf = lumiera_tmpbuf_provide (1024);

      for (int j = 0; j < 1024; ++j)
        {
          buf[j] = i;
        }
    }
}


TESTS_END
