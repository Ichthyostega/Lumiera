/*
  filedescriptor.c  -  file handling

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

#include "include/logging.h"
#include "lib/mutex.h"
#include "lib/safeclib.h"

#include "backend/file.h"
#include "backend/filedescriptor.h"
#include "backend/filedescriptorregistry.h"
#include "backend/filehandle.h"
#include "backend/filehandlecache.h"

#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>


//NOBUG_DEFINE_FLAG_PARENT (filedescriptor, file_all);

/* lookup and creation of files, initialized in backend.c */

lumiera_mutex lumiera_filecreate_mutex;


LumieraFiledescriptor
lumiera_filedescriptor_acquire (const char* name, int flags, LList filenode)
{
  TRACE (filedescriptor_dbg, "%s", name);
  REQUIRE (llist_is_empty (filenode));

  LumieraFiledescriptor dest = NULL;

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_filecreate_mutex)
    {
      lumiera_filedescriptor fdesc;
      fdesc.flags = flags;

      if (stat (name, &fdesc.stat) != 0)
        {
          if (errno == ENOENT && flags&O_CREAT)
            {
              errno = 0;
              char* dir = lumiera_tmpbuf_strndup (name, PATH_MAX);
              char* slash = dir;
              while ((slash = strchr (slash+1, '/')))
                {
                  *slash = '\0';
                  INFO (filedescriptor_dbg, "try creating dir: %s", dir);
                  if (mkdir (dir, 0777) == -1 && errno != EEXIST)
                    {
                      LUMIERA_ERROR_SET_CRITICAL (file, ERRNO, name);
                      goto error;
                    }
                  *slash = '/';
                }
              int fd;
              INFO (filedescriptor_dbg, "try creating file: %s", name);
              TODO ("creat mode from config");
              fd = creat (name, 0666);
              if (fd == -1)
                {
                  LUMIERA_ERROR_SET_CRITICAL (file, ERRNO, name);
                  goto error;
                }
              close (fd);
              if (stat (name, &fdesc.stat) != 0)
                {
                  /* finally, no luck */
                  LUMIERA_ERROR_SET_CRITICAL (file, ERRNO, name);
                  goto error;
                }
            }
        }

      /* lookup/create descriptor */
      dest = lumiera_filedescriptorregistry_ensure (&fdesc);

      if (dest)
        llist_insert_head (&dest->files, filenode);

    error:
      ;
    }

  return dest;
}




void
lumiera_filedescriptor_release (LumieraFiledescriptor self, const char* name, LList filenode)
{
  TRACE (filedescriptor_dbg);

  if (filenode)
    LUMIERA_MUTEX_SECTION (mutex_sync, &self->lock)
      {
        REQUIRE (llist_is_member (&self->files, filenode));
        llist_unlink (filenode);
      }

  if (llist_is_empty (&self->files))
    lumiera_filedescriptor_delete (self, name);
}


int
lumiera_filedescriptor_handle_acquire (LumieraFiledescriptor self)
{
  TRACE (filedescriptor_dbg);

  int fd = -1;

  LUMIERA_MUTEX_SECTION (mutex_sync, &self->lock)
    {
      if (!self->handle)
        /* no handle yet, get a new one */
        lumiera_filehandlecache_handle_acquire (self);
      else
        lumiera_filehandlecache_checkout (self->handle);

      fd = lumiera_filehandle_handle (self->handle);
    }

  return fd;
}


void
lumiera_filedescriptor_handle_release (LumieraFiledescriptor self)
{
  TRACE (filedescriptor_dbg);
  REQUIRE (self->handle);

  LUMIERA_MUTEX_SECTION (mutex_sync, &self->lock)
    lumiera_filehandlecache_checkin (self->handle);
}


const char*
lumiera_filedescriptor_name (LumieraFiledescriptor self)
{
  REQUIRE (!llist_is_empty (&self->files));

  return ((LumieraFile)(llist_head (&self->files)))->name;
}


int
lumiera_filedescriptor_flags (LumieraFiledescriptor self)
{
  return self->flags;
}


int
lumiera_filedescriptor_samestat (LumieraFiledescriptor self, struct stat* stat)
{
  if (self->stat.st_dev == stat->st_dev && self->stat.st_ino == stat->st_ino)
    return 1;

  return 0;
}


LumieraFiledescriptor
lumiera_filedescriptor_new (LumieraFiledescriptor template)
{
  LumieraFiledescriptor self = lumiera_malloc (sizeof (lumiera_filedescriptor));
  TRACE (filedescriptor_dbg, "at %p", self);

  psplaynode_init (&self->node);
  self->stat = template->stat;

  self->realsize = template->stat.st_size;
  self->flags = template->flags;
  self->handle = NULL;
  self->mmapings = NULL;
  llist_init (&self->files);

  lumiera_mutex_init (&self->lock, "filedescriptor", &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);

  return self;
}


void
lumiera_filedescriptor_delete (LumieraFiledescriptor self, const char* name)
{
  TRACE (filedescriptor_dbg, "%p %s", self, name);

  REQUIRE (llist_is_empty (&self->files));

  lumiera_filedescriptorregistry_remove (self);

  TODO ("destruct other members (WIP)");

  lumiera_mmapings_delete (self->mmapings);

  if (self->handle && name && ((self->flags & O_RDWR) == O_RDWR))
    {
      TRACE (filedescriptor_dbg, "truncate %s to %lld", name, (long long)self->realsize);
      lumiera_filehandlecache_checkout (self->handle);
      int dummy = ftruncate (lumiera_filehandle_handle (self->handle), self->realsize);
      (void) dummy; /* this is present to silence a warning */
      TODO ("handle error case better");
      lumiera_filehandlecache_checkin (self->handle);
      TODO ("really release filehandle");
    }

  lumiera_mutex_destroy (&self->lock, &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);
  lumiera_free (self);
}
