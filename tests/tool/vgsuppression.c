/*
  vgsuppression.c  -  Helper program to generate valgrind suppression files

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.
*/

/** @file vgsuppression.c
 ** dummy executable to generate _valgrind suppressions_.
 ** When running code under `valgrind` to detect memory leaks, typically a well known set
 ** of false alarms will be reported. The reason is that some facilities, either implemented
 ** by our own or from third party libraries, just choose never to free some working buffers.
 ** To deal with this well known issue, `valgrind` allows to be "primed" with a specifically
 ** crafted executable, which deliberately triggers just these false memory leak alarms.
 */

/*
  just place any problematic calls where valgrind whines about in main (with comments please)
*/

#include "lib/tmpbuf.h"
#include <stdint.h>

typedef const char* lumiera_err;
struct lumiera_errorcontext_struct;

struct lumiera_errorcontext_struct*
lumiera_error_get (void);

lumiera_err
lumiera_error_set (lumiera_err, const char*);

lumiera_err
lumiera_error (void);


int
main ()
{
  /* debian etch glibc is lazy about cleaning up TLS */
  lumiera_tmpbuf_provide (100);
  lumiera_tmpbuf_freeall ();

  /* tempbufs aren't freed by design */
  lumiera_tmpbuf_snprintf (SIZE_MAX, "everyone loves c-strings");

  /* lumiera_error_get() mallocs a LumieraErrorcontext for each thread */
  lumiera_error_get();
  lumiera_error_set("dummy","dummy");
  lumiera_error();

  return 0;
}
