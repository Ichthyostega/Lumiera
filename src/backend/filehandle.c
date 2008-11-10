/*
  filehandle  -  filehandle management and caching

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

#include "lib/llist.h"
#include "lib/safeclib.h"

#include "backend/file.h"
#include "backend/filehandle.h"
#include "backend/filedescriptor.h"

#include <unistd.h>

LumieraFilehandle
lumiera_filehandle_new ()
{
  LumieraFilehandle self = lumiera_malloc (sizeof (lumiera_filehandle));
  TRACE (filehandle, "%p", self);

  llist_init (&self->cachenode);
  self->fd = -1;
  self->use_cnt = 1;
  self->descriptor = NULL;

  return self;
}


void*
lumiera_filehandle_destroy_node (LList node)
{
  TRACE (filehandle);
  REQUIRE (llist_is_empty (node));
  LumieraFilehandle self = (LumieraFilehandle)node;
  REQUIRE (self->use_cnt == 0);

  if (self->fd >= 0)
    close (self->fd);
  self->fd = -1;
  self->descriptor = NULL;
  return self;
}


int
lumiera_filehandle_get (LumieraFilehandle self)
{
  REQUIRE (self->descriptor);
  return -1;
}

