/*
  vgsuppression.c  -  Helper program to generate valgrind suppression files

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

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

  return 0;
}
