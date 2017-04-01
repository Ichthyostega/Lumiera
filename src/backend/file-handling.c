/*
  FILE-HANDLING  -  file access and handling

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

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

* *****************************************************/


/** @file file-handling.c
 ** Implementation of file management functions
 */


#include "include/logging.h"
#include "lib/mutex.h"
#include "lib/safeclib.h"

#include "backend/file.h"
#include "backend/filehandlecache.h"

#include <limits.h>
#include <unistd.h>


LUMIERA_ERROR_DEFINE (FILE_CHANGED, "File changed unexpected");
LUMIERA_ERROR_DEFINE (FILE_NOMMAPINGS, "MMapings (chunksize/bias) not initialised");



LumieraFile
lumiera_file_init (LumieraFile self, const char* name, int flags)
{
  TRACE (file_dbg);

  if (self)
    {
      llist_init (&self->node);

      if (!(self->descriptor = lumiera_filedescriptor_acquire (name, flags, &self->node)))
        return NULL;

      self->name = lumiera_strndup (name, PATH_MAX);
    }

  return self;
}

LumieraFile
lumiera_file_destroy (LumieraFile self, int do_unlink)
{
  TRACE (file_dbg);

  lumiera_filedescriptor_release (self->descriptor, self->name, &self->node);
  if (do_unlink)
    unlink (self->name);

  lumiera_free (self->name);
  return self;
}


LumieraFile
lumiera_file_new (const char* name, int flags)
{
  TRACE (file_dbg);
  TRACE (file, "opening file '%s' with flags '%x'", name, flags);

  LumieraFile self = lumiera_malloc (sizeof (lumiera_file));
  if (!lumiera_file_init (self, name, flags))
    {
      lumiera_free (self);
      self = NULL;
    }

  return self;
}

void
lumiera_file_delete (LumieraFile self)
{
  TRACE (file_dbg);
  TRACE (file, "close file '%s'", self->name);
  lumiera_free (lumiera_file_destroy (self, 0));
}


void
lumiera_file_delete_unlink (LumieraFile self)
{
  TRACE (file_dbg);
  TRACE (file, "close and unlink file '%s'", self->name);
  lumiera_free (lumiera_file_destroy (self, 1));
}


int
lumiera_file_handle_acquire (LumieraFile self)
{
  TRACE (file_dbg);
  REQUIRE (self);
  REQUIRE (self->descriptor);

  return lumiera_filedescriptor_handle_acquire (self->descriptor);
}


void
lumiera_file_handle_release (LumieraFile self)
{
  TRACE (file_dbg);
  REQUIRE (self);
  REQUIRE (self->descriptor);

  return lumiera_filedescriptor_handle_release (self->descriptor);
}


size_t
lumiera_file_set_chunksize_bias (LumieraFile self, size_t chunksize, size_t bias)
{
  if (chunksize && !self->descriptor->mmapings)
    self->descriptor->mmapings = lumiera_mmapings_new (self, chunksize, bias);

  return self->descriptor->mmapings->chunksize;
}


size_t
lumiera_file_chunksize_get (LumieraFile self)
{
  if (!self->descriptor->mmapings)
    {
      LUMIERA_ERROR_SET (file, FILE_NOMMAPINGS, lumiera_filedescriptor_name (self->descriptor));
      return 0;
    }

  return self->descriptor->mmapings->chunksize;
}


size_t
lumiera_file_bias_get (LumieraFile self)
{
  if (!self->descriptor->mmapings)
    {
      LUMIERA_ERROR_SET (file, FILE_NOMMAPINGS, lumiera_filedescriptor_name (self->descriptor));
      return 0;
    }

  return self->descriptor->mmapings->bias;
}


LumieraMMapings
lumiera_file_mmapings (LumieraFile self)
{
  if (!self->descriptor->mmapings)
    LUMIERA_ERROR_SET (file, FILE_NOMMAPINGS, lumiera_filedescriptor_name (self->descriptor));

  return self->descriptor->mmapings;
}


int
lumiera_file_checkflags (LumieraFile self, int flags)
{
  return self->descriptor->flags & flags;
}


LumieraMMap
lumiera_file_mmap_acquire (LumieraFile self, off_t start, size_t size)
{
  TRACE (file_dbg);
  return lumiera_mmapings_mmap_acquire (lumiera_file_mmapings (self), self, start, size);
}


void
lumiera_file_release_mmap (LumieraFile self, LumieraMMap map)
{
  TRACE (file_dbg);
  lumiera_mmapings_release_mmap (lumiera_file_mmapings (self), map);
}



LumieraFile
lumiera_file_rdlock (LumieraFile self)
{
  if (self && !lumiera_filedescriptor_rdlock (self->descriptor))
    return NULL;

  return self;
}


LumieraFile
lumiera_file_wrlock (LumieraFile self)
{
  if (self && !lumiera_filedescriptor_wrlock (self->descriptor))
    return NULL;

  return self;
}


LumieraFile
lumiera_file_unlock (LumieraFile self)
{
  if (self && !lumiera_filedescriptor_unlock (self->descriptor))
    return NULL;

  return self;
}




/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
