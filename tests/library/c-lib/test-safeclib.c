/*
  TEST-SAFECLIB  -  some checked C operations

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file test-safeclib.c
 ** C unit test to cover size checked C operations
 ** @see safeclib.h
 */


#include "lib/safeclib.h"
#include "lib/tmpbuf.h" /* not factored out yet */
#include "lib/test/test.h"

#include <sys/time.h>
#include <sys/resource.h>

TESTS_BEGIN

TEST (allocation0)
{
  lumiera_malloc (0);
  NOTREACHED();
}

TEST (allocation1024)
{
  void* data[1024];
  for (int i = 0; i < 1024; ++i)
    {
      data[i] = lumiera_malloc (1024);
      CHECK (data[i]);
    }
  for (int i = 0; i < 1024; ++i)
    {
      free (data[i]);
    }
}

TEST (allocationtoobig)
{
  struct rlimit rl;
  rl.rlim_cur = 100*1024*1024;
  rl.rlim_max = 100*1024*1024;
  setrlimit (RLIMIT_AS, &rl);
  lumiera_malloc (200*1024*1024);
  NOTREACHED();
}

TEST (streq)
{
  CHECK (lumiera_streq ("foo", "foo"));
  CHECK (lumiera_streq (NULL, NULL));
  CHECK (!lumiera_streq (NULL, "foo"));
  CHECK (!lumiera_streq ("foo", NULL));
  CHECK (!lumiera_streq ("foo", "bar"));
}

TEST (tmpbuf)
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


TEST (tr0)
{
  char* r = lumiera_tmpbuf_tr (argv[2], "abcdeABCDE0123456789", "ABCDEABCDE0123456789", NULL);
  printf("%s\n", r?r:"failed");
}


TEST (tr)
{
  char* r = lumiera_tmpbuf_tr (argv[2], "abcdeABCDE0123456789", "ABCDEABCDE0123456789", "");
  printf("%s\n", r?r:"failed");
}


TEST (tr_)
{
  printf("%s\n", lumiera_tmpbuf_tr (argv[2], "abcdeABCDE0123456789", "ABCDEABCDE0123456789", "_"));
}


TESTS_END
