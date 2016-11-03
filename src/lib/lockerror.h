/*
  lockerror.h  -  error declarations for all locks (mutex, rwlocks, cond vars)

  Copyright (C)         Lumiera.org
    2010,               Christian Thaeter <ct@pipapo.org>

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


/** @file lockerror.h
 ** TODO lockerror.h
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
