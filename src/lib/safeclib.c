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
#include "lib/error.h"
#include "lib/safeclib.h"

#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdarg.h>
#include <nobug.h>


LUMIERA_ERROR_DEFINE (NO_MEMORY, "Out of Memory!");

void*
lumiera_malloc (size_t size)
{
  void* o = size ? malloc (size) : NULL;
  if (!o)
    LUMIERA_DIE (NO_MEMORY);
  return o;
}


void*
lumiera_calloc (size_t n, size_t size)
{
  void* o = (n&&size)? calloc (n, size) : NULL;
  if (!o)
    LUMIERA_DIE (NO_MEMORY);
  return o;
}


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


void
lumiera_tmpbuf_freeall (void)
{
  pthread_once (&lumiera_tmpbuf_tls_once, lumiera_tmpbuf_init);
  struct lumiera_tmpbuf_struct* buf = pthread_getspecific (lumiera_tmpbuf_tls_key);
  if (buf)
    {
      pthread_setspecific (lumiera_tmpbuf_tls_key, NULL);
      for (int idx = 0; idx < 64; ++idx)
        lumiera_free (buf->buffers[idx]);
      lumiera_free (buf);
    }
}


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
      lumiera_free (buf->buffers[buf->idx]);
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
  char* buf = lumiera_tmpbuf_provide (len+1);
  va_start (args, fmt);
  vsnprintf (buf, len+1, fmt, args);
  va_end (args);

  return buf;
}


char*
lumiera_tmpbuf_strcat3 (const char* str1, size_t str1_len,
                        const char* str2, size_t str2_len,
                        const char* str3, size_t str3_len)
{
  return lumiera_tmpbuf_snprintf (SIZE_MAX, "%.*s%s%.*s%s%.*s",
                                  str1?str1_len:0, str1?str1:"", str1?".":"",
                                  str2?str2_len:0, str2?str2:"",
                                  str3?".":"", str3?str3_len:0, str3?str3:"");
}


char*
lumiera_tmpbuf_tr (const char* in, const char* from, const char* to, const char* def)
{
  REQUIRE (strlen (from) == strlen (to), "from and to character set must have equal length");

  char* ret = lumiera_tmpbuf_strndup (in, SIZE_MAX);

  char* wpos;
  char* rpos;
  for (wpos = rpos = ret; *rpos; ++rpos, ++wpos)
    {
      char* found = strchr (from, *rpos);
      if (found)
        *wpos = to[found-from];
      else if (def)
        {
          if (*def)
            *wpos = *def;
          else
            {
              ++rpos;
              if (!*rpos)
                break;
            }
        }
      else
        return NULL;
    }
  *wpos = '\0';

  return ret;
}
