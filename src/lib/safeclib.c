/*
  safe_clib.c  -  Portable and safe wrapers around some clib functions and some tools

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
#include "error.h"

#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdarg.h>

/**
 * @file Portable and safe wrapers around some clib functions and some tools
 */

LUMIERA_ERROR_DEFINE (NO_MEMORY, "Out of Memory!");

/**
 * Allocate memory.
 * always succeeds or dies
 * @param size memory to be allocated
 * @return pointer to the allocated memory
 */
void*
lumiera_malloc (size_t size)
{
  void* o = size ? malloc (size) : NULL;
  if (!o)
    LUMIERA_DIE (NO_MEMORY);
  return o;
}


/**
 * Allocate cleared memory for an array.
 * always succeeds or dies
 * @param n number of elements
 * @param size memory to be allocated
 * @return pointer to the allocated memory
 */
void*
lumiera_calloc (size_t n, size_t size)
{
  void* o = (n&&size)? calloc (n, size) : NULL;
  if (!o)
    LUMIERA_DIE (NO_MEMORY);
  return o;
}


/**
 * Duplicate a C string.
 * always succeeds or dies
 * @param str string to be copied
 * @param len maximal length to be copied
 * @return pointer to the new string, "" if NULL was passed as str
 */
char*
lumiera_strndup (const char* str, size_t len)
{
  char* o;
  if (str)
    o = strndup (str, len);
  else
    o = strdup ("");

  if (!o)
    LUMIERA_DIE (NO_MEMORY);
  return o;
}


/**
 * Compare two C strings.
 * Handles NULL pointers as "", shortcut for same addresses
 * @param a first string for comparsion
 * @param b second string for comparsion
 * @param len maximal length for the comparsion
 * @return 0 if the strings are identical, -1 if smaller 1 if bigger.
 */
int
lumiera_strncmp (const char* a, const char* b, size_t len)
{
  return a == b ? 0 : strncmp (a?a:"", b?b:"", len);
}


/**
 * check 2 strings for identity.
 * @param a first string for comparsion
 * @param b second string for comparsion
 * @return 1 when the strings are the the same, 0 if not.
 */
int
lumiera_streq (const char* a, const char* b)
{
  return !lumiera_strncmp (a, b, SIZE_MAX);
}


/**
 * Round robin temporary buffers.
 * This provides 64 buffers per thread which are recycled with each use.
 * The idea here is to have fast buffers for temporal data without need for explicit heap management.
 */
struct lumiera_tmpbuf_struct
{
  void* buffers[64];
  size_t sizes[64];
  unsigned idx;
};

static pthread_once_t lumiera_tmpbuf_tls_once = PTHREAD_ONCE_INIT;
static pthread_key_t lumiera_tmpbuf_tls_key;

void
lumiera_tmpbuf_freeall (void);

static void
lumiera_tmpbuf_destroy (void* buf)
{
  lumiera_tmpbuf_freeall ();
}

static void
lumiera_tmpbuf_init (void)
{
  pthread_key_create (&lumiera_tmpbuf_tls_key, lumiera_tmpbuf_destroy);
}


/**
 * free all buffers associated with this thread.
 * This function is called automatically, usually one doesnt need to call it.
 */
void
lumiera_tmpbuf_freeall (void)
{
  pthread_once (&lumiera_tmpbuf_tls_once, lumiera_tmpbuf_init);
  struct lumiera_tmpbuf_struct* buf = pthread_getspecific (lumiera_tmpbuf_tls_key);
  if (buf)
    {
      pthread_setspecific (lumiera_tmpbuf_tls_key, NULL);
      for (int idx = 0; idx < 64; ++idx)
        free (buf->buffers[idx]);
      free (buf);
    }
}


/**
 * Query a thread local buffer.
 * @param size minimal needed size for the buffer
 * @return the buffer
 */
void*
lumiera_tmpbuf_provide (size_t size)
{
  pthread_once (&lumiera_tmpbuf_tls_once, lumiera_tmpbuf_init);
  struct lumiera_tmpbuf_struct* buf = pthread_getspecific (lumiera_tmpbuf_tls_key);
  if (!buf)
    pthread_setspecific (lumiera_tmpbuf_tls_key,
                         buf = lumiera_calloc (1, sizeof (struct lumiera_tmpbuf_struct)));

  buf->idx = (buf->idx + 1) & 0x3f;

  if (buf->sizes[buf->idx] < size || buf->sizes[buf->idx] > 8*size)
    {
      free (buf->buffers[buf->idx]);
      buf->sizes[buf->idx] = (size+4*sizeof(long)) & ~(4*sizeof(long)-1);
      buf->buffers[buf->idx] = lumiera_malloc (buf->sizes[buf->idx]);
    }
  return buf->buffers[buf->idx];
}


char*
lumiera_tmpbuf_strndup (const char* src, size_t size)
{
  size_t len = strlen (src);
  len = len > size ? size : len;

  char* buf = lumiera_tmpbuf_provide (len + 1);
  strncpy (buf, src, len);
  buf[len] = '\0';

  return buf;
}


char*
lumiera_tmpbuf_snprintf (size_t size, const char* fmt, ...)
{
  va_list args;

  va_start (args, fmt);
  size_t len = vsnprintf (NULL, 0, fmt, args);
  va_end (args);

  len = len > size ? size : len;
  char* buf = lumiera_tmpbuf_provide (len);
  va_start (args, fmt);
  vsnprintf (buf, len, fmt, args);
  va_end (args);

  return buf;
}
