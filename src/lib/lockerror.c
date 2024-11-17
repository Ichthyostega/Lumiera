/*
  lockerror.c  -  error declarations for all locks (mutex, rwlocks, cond vars)

   Copyright (C)
     2010,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file lockerror.c
 ** implementation and definitions for error-handling on low-level locking
 */


#include "lib/lockerror.h"

#include <errno.h>

/* fatal errors (EINVAL usually) */
LUMIERA_ERROR_DEFINE (LOCK_ACQUIRE, "locking failed");
LUMIERA_ERROR_DEFINE (LOCK_RELEASE, "unlocking failed");
LUMIERA_ERROR_DEFINE (LOCK_DESTROY, "lock destroy failed");

/* runtime errors */
LUMIERA_ERROR_DEFINE (LOCK_INVAL, "lock initialisation error");
LUMIERA_ERROR_DEFINE (LOCK_BUSY, "already locked");
LUMIERA_ERROR_DEFINE (LOCK_DEADLK, "already locked by this thread");
LUMIERA_ERROR_DEFINE (LOCK_PERM, "not locked by this thread");
LUMIERA_ERROR_DEFINE (LOCK_TIMEOUT, "timeout");
LUMIERA_ERROR_DEFINE (LOCK_AGAIN, "too much recursive locks");



void
lumiera_lockerror_set (int err, struct nobug_flag* flag, const struct nobug_context ctx)
{
  switch (err)
    {
    case 0:
      break;
    case ETIMEDOUT:
      lumiera_error_set(LUMIERA_ERROR_LOCK_TIMEOUT, ctx.func);
      // no implicit logging, since timeout can be intentional
      break;
    case EINVAL:
      LUMIERA_ERROR_SET_ALERT(NOBUG_FLAG_RAW(flag), LOCK_INVAL, ctx.func);
      break;
    case EBUSY:
      LUMIERA_ERROR_SET(NOBUG_FLAG_RAW(flag), LOCK_BUSY, ctx.func);
      break;
    case EDEADLK:
      LUMIERA_ERROR_SET(NOBUG_FLAG_RAW(flag), LOCK_DEADLK, ctx.func);
      break;
    case EPERM:
      LUMIERA_ERROR_SET_ALERT(NOBUG_FLAG_RAW(flag), LOCK_PERM, ctx.func);
      break;
    case EAGAIN:
      LUMIERA_ERROR_SET_WARNING(NOBUG_FLAG_RAW(flag), LOCK_AGAIN, ctx.func);
      break;
    default:
      LUMIERA_ERROR_SET_CRITICAL(NOBUG_FLAG_RAW(flag), UNKNOWN, ctx.func);
      break;
    }
}


/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
