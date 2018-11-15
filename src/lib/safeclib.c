/*
  safe_clib.c  -  Portable and safe wrapers around some clib functions and some tools

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

* *****************************************************/


/** @file safeclib.c
 ** Implementation of error-safe wrappers for some notorious C-Lib functions.
 */


#include "lib/error.h"
#include "lib/safeclib.h"

#include "vault/resourcecollector.h"

#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <nobug.h>


LUMIERA_ERROR_DEFINE (NO_MEMORY, "Out of Memory!");



/* placeholder function until the resourcecollector gets hooked in */
static int
die_no_mem (enum lumiera_resource which, enum lumiera_resource_try* iteration, void* context)
{
  (void) which; (void) iteration; (void) context;
  LUMIERA_DIE (NO_MEMORY);
  return 0; /* not reached */
}

static lumiera_resourcecollector_run_fn lumiera_safeclib_resourcecollector_run_hook = die_no_mem;

void
lumiera_safeclib_set_resourcecollector (void* hook)
{
  if (hook)
    lumiera_safeclib_resourcecollector_run_hook = (lumiera_resourcecollector_run_fn)hook;
  else
    lumiera_safeclib_resourcecollector_run_hook = die_no_mem;
}


void*
lumiera_malloc (size_t size)
{
  enum lumiera_resource_try iteration = LUMIERA_RESOURCE_ONE;
  void* o = NULL;

  if (size)
    do
      o = malloc (size);
    while (!o && lumiera_safeclib_resourcecollector_run_hook (LUMIERA_RESOURCE_MEMORY, &iteration, &size));

  return o;
}


void*
lumiera_calloc (size_t n, size_t size)
{
  enum lumiera_resource_try iteration = LUMIERA_RESOURCE_ONE;
  void* o = NULL;

  size_t gross = n*size;

  if (n&&size)
    do
      o = calloc (n, size);
    while (!o && lumiera_safeclib_resourcecollector_run_hook (LUMIERA_RESOURCE_MEMORY, &iteration, &gross));

  return o;
}


void*
lumiera_realloc (void* ptr, size_t size)
{
  enum lumiera_resource_try iteration = LUMIERA_RESOURCE_ONE;
  void* o = NULL;

  if (size)
    do
      o = realloc (ptr, size);
    while (!o && lumiera_safeclib_resourcecollector_run_hook (LUMIERA_RESOURCE_MEMORY, &iteration, &size));

  return o;
}


char*
lumiera_strndup (const char* str, size_t len)
{
  enum lumiera_resource_try iteration = LUMIERA_RESOURCE_ONE;
  void* o = NULL;

  do
    if (str && len)
      o = strndup (str, len);
    else
      o = strdup ("");
  while (!o && lumiera_safeclib_resourcecollector_run_hook (LUMIERA_RESOURCE_MEMORY, &iteration, &len));

  return o;
}


int
lumiera_strncmp (const char* a, const char* b, size_t len)
{
  return a == b ? 0 : strncmp (a?a:"", b?b:"", len);
}


int
lumiera_streq (const char* a, const char* b)
{
  return !lumiera_strncmp (a, b, SIZE_MAX);
}




/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
