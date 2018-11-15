/*
  FileHandle  -  filehandle management and caching

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


/** @file filehandle.c
 ** Implementation of filehandle representation.
 ** @todo development in this area is stalled since 2010
 */


#include "include/logging.h"
#include "lib/llist.h"
#include "lib/safeclib.h"

#include "backend/file.h"
#include "backend/filehandle.h"
#include "backend/filedescriptor.h"

#include <unistd.h>



LumieraFilehandle
lumiera_filehandle_init (LumieraFilehandle self, LumieraFiledescriptor desc)
{
  TRACE (filehandle_dbg, "%p", self);
  if (self)
    {
      llist_init (&self->cachenode);
      self->fd = -1;
      self->use_cnt = 1;
      self->descriptor = desc;
    }
  return self;
}


LumieraFilehandle
lumiera_filehandle_new (LumieraFiledescriptor desc)
{
  LumieraFilehandle self = lumiera_malloc (sizeof (*self));
  return lumiera_filehandle_init (self, desc);
}


void*
lumiera_filehandle_destroy_node (LList node)
{
  TRACE (filehandle_dbg);
  REQUIRE (llist_is_empty (node));
  LumieraFilehandle self = (LumieraFilehandle)node;
  REQUIRE (self->use_cnt == 0);

  if (self->fd >= 0)
    close (self->fd);
  return self;
}



int
lumiera_filehandle_handle (LumieraFilehandle self)
{
  TRACE (filehandle_dbg);

  int fd = -1;
  if (self->fd == -1)
    {
      fd = open (lumiera_filedescriptor_name (self->descriptor), lumiera_filedescriptor_flags (self->descriptor) & LUMIERA_FILE_MASK);
      if (fd == -1)
        {
          //////////////////////TODO Handle EMFILE etc with the resourcecollector
          LUMIERA_ERROR_SET_CRITICAL (file, ERRNO, lumiera_filedescriptor_name (self->descriptor));
        }
      else
        {
          struct stat st;
          if (fstat (fd, &st) == -1)
            {
              close (fd);
              fd = -1;
              LUMIERA_ERROR_SET_CRITICAL (file, ERRNO, lumiera_filedescriptor_name (self->descriptor));
            }
          else if (!lumiera_filedescriptor_samestat (self->descriptor, &st))
            {
              close (fd);
              fd = -1;
              /* Woops this is not the file we expected to use */
              LUMIERA_ERROR_SET_CRITICAL (file, FILE_CHANGED, lumiera_filedescriptor_name (self->descriptor));
            }
        }
      self->fd = fd;
    }

  return self->fd;
}
