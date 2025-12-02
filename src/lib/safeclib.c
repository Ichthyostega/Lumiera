/*
  safe_clib.c  -  Portable and safe wrapers around some clib functions and some tools

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file safeclib.c
 ** Implementation of error-safe wrappers for some notorious C-Lib functions.
 */


#include "lib/error.h"
#include "lib/safeclib.h"

#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <nobug.h>


LUMIERA_ERROR_DEFINE (NO_MEMORY, "Out of Memory!");


/**
 * Resources known to the resource collector
 * @todo 10/2023 this is a left-over of the »resource-collector« plan
 */
enum lumiera_resource
  {
    /** memory blocks, context is a pointer to the size_t required **/
    LUMIERA_RESOURCE_MEMORY,
//  /** OS filehandles **/
//  LUMIERA_RESOURCE_FILEHANDLE,
//  /** CPU time, as in threads and such  **/
//  LUMIERA_RESOURCE_CPU,
//  /** mmaped regions **/
//  LUMIERA_RESOURCE_MMAP,
//  /** disk space for the storage area, context is a pointer to the filename indication the device **/
//  LUMIERA_RESOURCE_DISKSTORAGE,
//  /** disk bandwidth for the storage area, context is a pointer to the filename indication the device **/
//  LUMIERA_RESOURCE_STORAGEBANDWIDTH,
//  /** disk space for the caching area, context is a pointer to the filename indication the device **/
//  LUMIERA_RESOURCE_DISKCACHE,
//  /** disk bandwidth for the caching area, context is a pointer to the filename indication the device **/
//  LUMIERA_RESOURCE_CACHEBANDWIDTH,

    LUMIERA_RESOURCE_END /* last entry */
  };


/**
 * Iteration indicator
 * @todo 10/2023 this is a left-over of the »resource-collector« plan
 * Resource collection works iteratively freeing more and more resources.
 * Handlers do not need to obey the request and shall return LUMIERA_RESOURCE_NONE
 * which will then continue with the next handler.
 * This goes through all available handlers until one returns a higher or same value
 * than the current iteration to indicate that it freed enough resources to continue the task.
 * Then control is passed back to the calling loop which retries the resource allocation.
 * LUMIERA_RESOURCE_PANIC is somewhat special since it will always call all registered handlers
 * for all resources, not only the queried one and finally _exit() the application.
 * The exact amounts of resources to be freed for ONE, SOME and MANY in intentionally
 * kept vague the handlers are free to interpret this in some sensible way.
 */
enum lumiera_resource_try
  {
    /** No op, returned by a handler when it did nothing **/
    LUMIERA_RESOURCE_NONE,
    /** try to free one or really few of this resources **/
    LUMIERA_RESOURCE_ONE,
    /** try to free a small reasonable implementation defined amount of resources **/
    LUMIERA_RESOURCE_SOME,
    /** try to free a bigger implementation defined amount of resources **/
    LUMIERA_RESOURCE_MANY,
    /** free as much as possible **/
    LUMIERA_RESOURCE_ALL,
    /** die! **/
    LUMIERA_RESOURCE_PANIC,
    /** When a handler gets unregistered it will be called with this value to give it a chance to clean up the user 'data' **/
    LUMIERA_RESOURCE_UNREGISTER
  };


/**
 * @internal placeholder function in case the resource-collector was not installed
 * @todo 10/2023 in the early stages of the Lumiera project a centralised resource pooling
 *       was considered. On resource shortage, the (planned) resource-collector would attempt
 *       to reclaim excess resources. This would allow to use resources (e.g. memory) wastefully
 *       as trade-off for performance gains.
 *       This concept was never developed beyond an interface draft; most notably it was never
 *       clarified how excess resources could have been identified on-demand while in-flight.
 *       Today I view this concept as an immature plan and remove the remaining draft code.
 */
static int
die_no_mem (enum lumiera_resource which, enum lumiera_resource_try* iteration, void* context)
{
  (void) which; (void) iteration; (void) context;
  LUMIERA_DIE (NO_MEMORY);
  return 0; /* not reached */
}


void*
lumiera_malloc (size_t size)
{
  enum lumiera_resource_try iteration = LUMIERA_RESOURCE_ONE;
  void* o = NULL;

  if (size)
    {
      o = malloc (size);
      if (!o)
        die_no_mem (LUMIERA_RESOURCE_MEMORY, &iteration, &size);
    }

  return o;
}


void*
lumiera_calloc (size_t n, size_t size)
{
  enum lumiera_resource_try iteration = LUMIERA_RESOURCE_ONE;
  void* o = NULL;

  size_t gross = n*size;

  if (n&&size)
    {
      o = calloc (n, size);
      if (!o)
        die_no_mem (LUMIERA_RESOURCE_MEMORY, &iteration, &gross);
    }

  return o;
}


void*
lumiera_realloc (void* ptr, size_t size)
{
  enum lumiera_resource_try iteration = LUMIERA_RESOURCE_ONE;
  void* o = NULL;

  if (size)
    {
      o = realloc (ptr, size);
      if (!o)
        die_no_mem (LUMIERA_RESOURCE_MEMORY, &iteration, &size);
    }

  return o;
}


char*
lumiera_strndup (const char* str, size_t len)
{
  enum lumiera_resource_try iteration = LUMIERA_RESOURCE_ONE;
  void* o = NULL;

    if (str && len)
      o = strndup (str, len);
    else
      o = strdup ("");

  if (!o)
    die_no_mem (LUMIERA_RESOURCE_MEMORY, &iteration, &len);

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
