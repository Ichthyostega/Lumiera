/*
  error.c  -  Cinelerra Error handling

  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>

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

#include <pthread.h>

#include "lib/error.h"

/**
 * @file C Error handling in Cinelerra.
 */


/*
  predefined errors
*/
CINELERRA_ERROR_DEFINE (ERRNO, "errno");


/* Thread local storage */
static pthread_key_t cinelerra_error_tls;
static pthread_once_t cinelerra_error_initialized = PTHREAD_ONCE_INIT;

static void
cinelerra_error_tls_init (void)
{
  pthread_key_create (&cinelerra_error_tls, NULL);
}

/**
 * Set error state for the current thread.
 * If the error state of the current thread was cleared, then set it, else preserve the old state.
 * @param err name of the error with 'CINELERRA_ERROR_' prefix (example: CINELERRA_ERROR_NO_MEMORY)
 * @return old state, that is NULL for success, when the state was cleared and a pointer to a pending error when the error state was already set
 */
const char*
cinelerra_error_set (const char * nerr)
{
  pthread_once (&cinelerra_error_initialized, cinelerra_error_tls_init);

  const char* err = pthread_getspecific (cinelerra_error_tls);
  if (!err)
    pthread_setspecific (cinelerra_error_tls, nerr);

  return err;
}


/**
 * Get and clear current error state.
 * This function clears the error state, if it needs to be reused, one has to store it in a temporary variable.
 * @return pointer to any pending error of this thread, NULL if no error is pending
 */
const char*
cinelerra_error ()
{
  pthread_once (&cinelerra_error_initialized, cinelerra_error_tls_init);

  const char* err = pthread_getspecific (cinelerra_error_tls);
  if (err)
    pthread_setspecific (cinelerra_error_tls, NULL);
  return err;
}
