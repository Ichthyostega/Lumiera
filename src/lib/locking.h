/*
  locking.h  -  shared declarations for all locking primitives

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

#ifndef LUMIERA_LOCKING_H
#define LUMIERA_LOCKING_H

#include <pthread.h>
#include <errno.h>
#include <nobug.h>

#include "lib/error.h"


LUMIERA_ERROR_DECLARE (MUTEX_LOCK);
LUMIERA_ERROR_DECLARE (MUTEX_UNLOCK);
LUMIERA_ERROR_DECLARE (MUTEX_DESTROY);

/**
 * @file Shared declarations for all locking primitives.
 */

/**
 * used to store the current lock state.
 *
 *
 */
enum lumiera_lockstate
  {
    LUMIERA_UNLOCKED,
    LUMIERA_LOCKED,
    LUMIERA_RDLOCKED,
    LUMIERA_WRLOCKED
  };

#endif
