/*
  ERROR-STATE  -  Lumiera C error flag implementation

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file error-state.c
 ** implementation of C-style error handling in Lumiera.
 ** This implementation is based on a thread local error flag.
 ** C++ code uses a custom exception hierarchy, which automatically
 ** sets the error flag whenever an exception is thrown.
 ** 
 ** @see error.h
 ** @see error-exception.cpp
 **
 */


#include "lib/error.h"

#include <pthread.h>


/*
  predefined errors
*/
LUMIERA_ERROR_DEFINE (ERRNO, "errno");
LUMIERA_ERROR_DEFINE (EERROR, "could not initialise error system");
LUMIERA_ERROR_DEFINE (UNKNOWN, "unknown error");


/* Thread local storage */
static pthread_key_t lumiera_error_tls;
static pthread_once_t lumiera_error_initialized = PTHREAD_ONCE_INIT;

/**
 * Holding error and some context data.
 */
struct lumiera_errorcontext_struct
{
  lumiera_err err;
  char* extra;
};

typedef struct lumiera_errorcontext_struct lumiera_errorcontext;
typedef lumiera_errorcontext* LumieraErrorcontext;


static void
lumiera_error_tls_delete (void* err)
{
  if (err)
    free (((LumieraErrorcontext)err)->extra);
  free(err);
}

static void
lumiera_error_tls_init (void)
{
  if (!!pthread_key_create (&lumiera_error_tls, lumiera_error_tls_delete))
    LUMIERA_DIE (EERROR);
}


LumieraErrorcontext
lumiera_error_get (void)
{
  pthread_once (&lumiera_error_initialized, lumiera_error_tls_init);

  LumieraErrorcontext self = pthread_getspecific (lumiera_error_tls);
  if (!self)
    {
      /* malloc() and not lumiera_malloc() here because nothing else might be initialised when calling this */
      self = malloc (sizeof *self);
      if (!self)
        LUMIERA_DIE (EERROR);

      self->err = NULL;
      self->extra = NULL;
      pthread_setspecific (lumiera_error_tls, self);
    }

  return self;
}


lumiera_err
lumiera_error_set (lumiera_err nerr, const char* extra)
{
  LumieraErrorcontext self = lumiera_error_get ();

  if (!self->err)
    {
      self->err = nerr;
      free (self->extra);
      if (extra)
        self->extra = strdup (extra);
      else
        self->extra = NULL;
    }

  return self->err;
}


lumiera_err
lumiera_error (void)
{
  LumieraErrorcontext self = lumiera_error_get ();
  lumiera_err err = self->err;

  if (err)
    self->err = NULL;
  return err;
}

const char*
lumiera_error_extra (void)
{
  return lumiera_error_get ()->extra;
}


lumiera_err
lumiera_error_peek (void)
{
  return lumiera_error_get ()->err;
}

int
lumiera_error_expect (lumiera_err expected)
{
  LumieraErrorcontext self = lumiera_error_get ();
  lumiera_err err = self->err;

  if (err == expected)
    {
      if (err)
        self->err = NULL;
      return 1;
    }
  else
    return 0;
}
