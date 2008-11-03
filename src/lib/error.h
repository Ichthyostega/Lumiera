/*
  error.h  -  Lumiera Error handling

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
#ifndef LUMIERA_ERROR_H
#define LUMIERA_ERROR_H

#ifdef __cplusplus
extern "C" {
#elif 0
} /*eek, fixes emacs indenting for now*/
#endif

#include <nobug.h>
#include <stdlib.h>

/**
 * @file
 * C Error handling in Lumiera, header.
 */

typedef const char* lumiera_err;

/**
 * Abort unconditionally with a 'Fatal Error!' message.
 * This macro is used whenever the program end up in a invalid state from which no runtime recovery is possible
 */
#define LUMIERA_DIE(err) \
  do { NOBUG_ERROR(NOBUG_ON, "Fatal Error: %s ", strchr(LUMIERA_ERROR_##err, ':')); abort(); } while(0)

/**
 * Forward declare an error constant.
 * This macro eases the error declaration in header files
 * @param err name of the error without the 'LUMIERA_ERROR_' prefix (example: NO_MEMORY)
 */
#define LUMIERA_ERROR_DECLARE(err) \
extern lumiera_err LUMIERA_ERROR_##err

/**
 * Definition and initialization of an error constant.
 * This macro eases the error definition in implementation files
 * @param err name of the error without the 'LUMIERA_ERROR_' prefix (example: NO_MEMORY)
 * @param msg message describing the error in plain english (example: "memory allocation failed")
 */
#define LUMIERA_ERROR_DEFINE(err, msg) \
lumiera_err LUMIERA_ERROR_##err = "LUMIERA_ERROR_" #err ":" msg

/**
 * Helper macro to raise an error for the current thread.
 * This macro eases setting an error. It adds NoBug logging support to the low level error handling.
 * @param flag NoBug flag describing the subsystem where the error was raised
 * @param err name of the error without the 'LUMIERA_ERROR_' prefix (example: NO_MEMORY)
 */
#define LUMIERA_ERROR_SET(flag, err)                          \
(({ERROR (flag, "%s", strchr(LUMIERA_ERROR_##err, ':')+1);}), \
lumiera_error_set(LUMIERA_ERROR_##err))

/**
 * Set error state for the current thread.
 * If the error state of the current thread was cleared, then set it, else preserve the old state.
 * @param nerr name of the error with 'LUMIERA_ERROR_' prefix (example: LUMIERA_ERROR_NO_MEMORY)
 * @return old state, that is NULL for success, when the state was cleared and a pointer to a pending 
 * error when the error state was already set
 */
lumiera_err
lumiera_error_set (lumiera_err err);

/**
 * Get and clear current error state.
 * This function clears the error state, if it needs to be reused, one has to store it in a temporary
 * variable.
 * @return pointer to any pending error of this thread, NULL if no error is pending
 */
lumiera_err
lumiera_error (void);


/**
 * Check current error state without clearing it
 * Please avoid this function and use lumiera_error() if possible. Errors must be cleared else certain
 * parts of the application refuse to cooperate with you. This shall only be used to decide if one
 * wants to barf out of a loop or subroutine to deliver the error to a higher level.
 * @return pointer to any pending error of this thread, NULL if no error is pending
 */
lumiera_err
lumiera_error_peek (void);


/*
  predefined errors
*/
LUMIERA_ERROR_DECLARE (ERRNO);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* LUMIERA_ERROR_H */
