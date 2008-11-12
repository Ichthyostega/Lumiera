/*
  file.c  -  file handling

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

#include "lib/mutex.h"
#include "lib/safeclib.h"

#include "backend/file.h"
#include "backend/filehandlecache.h"
#include "backend/filedescriptor.h"

#include <limits.h>
#include <unistd.h>

NOBUG_DEFINE_FLAG_PARENT (file, file_all);

LUMIERA_ERROR_DEFINE (FILE_CHANGED, "File changed unexpected");


LumieraFile
lumiera_file_init (LumieraFile self, const char* name, int flags, size_t chunksize)
{
  TRACE (file);

  llist_init (&self->node);

  if (!(self->descriptor = lumiera_filedescriptor_acquire (name, flags, &self->node)))
    return NULL;

  if (chunksize && !self->descriptor->mmapings)
    self->descriptor->mmapings = lumiera_mmapings_new (self, chunksize);

  self->name = lumiera_strndup (name, PATH_MAX);

  return self;
}

LumieraFile
lumiera_file_destroy (LumieraFile self)
{
  TRACE (file);

  lumiera_filedescriptor_release (self->descriptor, self->name, &self->node);
  lumiera_free (self->name);
  return self;
}


LumieraFile
lumiera_file_new (const char* name, int flags, size_t chunksize)
{
  TRACE (file);
  LumieraFile self = lumiera_malloc (sizeof (lumiera_file));
  return lumiera_file_init (self, name, flags, chunksize);
}

void
lumiera_file_delete (LumieraFile self)
{
  TRACE (file);
  lumiera_free (lumiera_file_destroy (self));
}


int
lumiera_file_handle_acquire (LumieraFile self)
{
  TRACE (file);
  REQUIRE (self);
  REQUIRE (self->descriptor);
  REQUIRE (lumiera_fhcache);

  return lumiera_filedescriptor_handle (self->descriptor);
}


void
lumiera_file_handle_release (LumieraFile self)
{
  TRACE (file);

  LUMIERA_MUTEX_SECTION (file, &self->descriptor->lock)
    {
      lumiera_filehandlecache_checkin (lumiera_fhcache, self->descriptor->handle);
    }
}

LumieraMMapings
lumiera_file_mmapings (LumieraFile self)
{
  REQUIRE (self->descriptor->mmapings, "mmapings not initialized")
  return self->descriptor->mmapings;
}
