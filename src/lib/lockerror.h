/*
  lockerror.h  -  error declarations for all locks (mutex, rwlocks, cond vars)

   Copyright (C)
     2010,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.
*/


/** @file lockerror.h
 ** definitions and declarations for error-handling on low-level locking
 */

#ifndef LUMIERA_LOCKERRORS_H
#define LUMIERA_LOCKERRORS_H

#include "lib/error.h"

/* fatal errors (EINVAL usually), we DIE on these, shall never ever happen on a correct program */
LUMIERA_ERROR_DECLARE (LOCK_ACQUIRE);
LUMIERA_ERROR_DECLARE (LOCK_RELEASE);
LUMIERA_ERROR_DECLARE (LOCK_DESTROY);

/* runtime errors */
LUMIERA_ERROR_DECLARE (LOCK_INVAL);
LUMIERA_ERROR_DECLARE (LOCK_BUSY);
LUMIERA_ERROR_DECLARE (LOCK_DEADLK);
LUMIERA_ERROR_DECLARE (LOCK_PERM);
LUMIERA_ERROR_DECLARE (LOCK_TIMEOUT);
LUMIERA_ERROR_DECLARE (LOCK_AGAIN);

/**
 * Translate pthread error code into lumiera error
 */
void
lumiera_lockerror_set (int err, struct nobug_flag* flag, const struct nobug_context ctx);

#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
