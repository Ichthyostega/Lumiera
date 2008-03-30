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
#include "backend/filedescriptor.h"

#include <limits.h>


NOBUG_DEFINE_FLAG (lumiera_file);

LumieraFile
lumiera_file_init (LumieraFile self, const char* name, int flags)
{
  if (!lumiera_filedescriptor_acquire (&self->descriptor, name, flags))
    return NULL;
  self->name = lumiera_strndup (name, PATH_MAX);

  return self;
}

LumieraFile
lumiera_file_destroy (LumieraFile self)
{
  lumiera_reference_destroy (&self->descriptor);
  free ((void*)self->name);
  return self;
}


LumieraFile
lumiera_file_new (const char* name, int flags)
{
  LumieraFile self = lumiera_malloc (sizeof (lumiera_file));
  return lumiera_file_init (self, name, flags);
}

void
lumiera_file_delete (LumieraFile self)
{
  free (lumiera_file_destroy (self));
}

