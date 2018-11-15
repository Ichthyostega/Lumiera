/*
  FileDescriptor  -  file handling

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


/** @file filedescriptor.c
 ** Implementation of a file descriptor management framework
 ** @todo development in this area is stalled since 2010
 */


#include "include/logging.h"
#include "lib/mutex.h"
#include "lib/safeclib.h"
#include "lib/tmpbuf.h"

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



/* lookup and creation of files, initialised in backend.c */

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
              TODO ("create mode from config");
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

  RESOURCE_USER_INIT(self->filelock_rh);

  lumiera_mutex_init (&self->lock, "filedescriptor", &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);

  lumiera_rwlock_init (&self->filelock, "filelock", &NOBUG_FLAG (filedescriptor_dbg), NOBUG_CONTEXT);
  self->lock_cnt = 0;

  return self;
}


void
lumiera_filedescriptor_delete (LumieraFiledescriptor self, const char* name)
{
  TRACE (filedescriptor_dbg, "%p %s", self, name);

  REQUIRE (!self->lock_cnt, "File still locked");
  REQUIRE (llist_is_empty (&self->files));

  lumiera_filedescriptorregistry_remove (self);

  TODO ("destroy other members (WIP)");

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

  lumiera_rwlock_destroy (&self->filelock, &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);
  lumiera_mutex_destroy (&self->lock, &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);

  lumiera_free (self);
}


LumieraFiledescriptor
lumiera_filedescriptor_rdlock (LumieraFiledescriptor self)
{
  TRACE (filedescriptor_dbg);

  if (self)
    {
      lumiera_rwlock_rdlock (&self->filelock, &NOBUG_FLAG (filedescriptor_dbg), &self->filelock_rh, NOBUG_CONTEXT);

      int fd = lumiera_filedescriptor_handle_acquire (self);
      int err = 0;

      LUMIERA_MUTEX_SECTION (mutex_dbg, &self->lock)
        {
          if (!self->lock_cnt)
            {

              struct flock lock;
              lock.l_type = F_RDLCK;
              lock.l_whence = SEEK_SET;
              lock.l_start = 0;
              lock.l_len = 0;

              while ((err = fcntl (fd, F_SETLKW, &lock)) == -1 && errno == EINTR)
                ;

            }
          if (!err)
            ++self->lock_cnt;
        }

      if (err)
        {
          lumiera_filedescriptor_handle_release (self);
          lumiera_rwlock_unlock (&self->filelock, &NOBUG_FLAG (filedescriptor_dbg), &self->filelock_rh, NOBUG_CONTEXT);

          LUMIERA_ERROR_SET_WARNING (filedescriptor_dbg, ERRNO, lumiera_filedescriptor_name (self));
          self = NULL;
        }
    }

  return self;
}



LumieraFiledescriptor
lumiera_filedescriptor_wrlock (LumieraFiledescriptor self)
{
  TRACE (filedescriptor_dbg);

  if (self)
    {
      lumiera_rwlock_wrlock (&self->filelock, &NOBUG_FLAG (filedescriptor_dbg), &self->filelock_rh, NOBUG_CONTEXT);

      int fd = lumiera_filedescriptor_handle_acquire (self);
      int err = 0;

      LUMIERA_MUTEX_SECTION(mutex_dbg, &self->lock)
        {

          struct flock lock;
          lock.l_type = F_WRLCK;
          lock.l_whence = SEEK_SET;
          lock.l_start = 0;
          lock.l_len = 0;

          while ((err = fcntl (fd, F_SETLKW, &lock)) == -1 && errno == EINTR)
            ;

          if (!err)
            self->lock_cnt = -1;

        }

      if (err)
        {
          lumiera_filedescriptor_handle_release (self);
          lumiera_rwlock_unlock (&self->filelock, &NOBUG_FLAG (filedescriptor_dbg), &self->filelock_rh, NOBUG_CONTEXT);
          LUMIERA_ERROR_SET_WARNING (filedescriptor_dbg, ERRNO, lumiera_filedescriptor_name (self));
          self = NULL;
        }
    }

  return self;
}


LumieraFiledescriptor
lumiera_filedescriptor_unlock (LumieraFiledescriptor self)
{
  TRACE (filedescriptor_dbg);

  if (self)
    {
      int fd = lumiera_filehandle_get (self->handle);
      REQUIRE (fd >= 0, "was not locked?");
      int err = 0;

      LUMIERA_MUTEX_SECTION(mutex_dbg, &self->lock)
        {
          if (self->lock_cnt == -1)
            self->lock_cnt = 0;
          else
            --self->lock_cnt;

          if (!self->lock_cnt)
            {

              struct flock lock;
              lock.l_type = F_UNLCK;
              lock.l_whence = SEEK_SET;
              lock.l_start = 0;
              lock.l_len = 0;

              while ((err = fcntl (fd, F_SETLK, &lock)) == -1 && errno == EINTR)
                ;
            }
        }

      if (err)
        {
          LUMIERA_ERROR_SET_WARNING (filedescriptor_dbg, ERRNO, lumiera_filedescriptor_name (self));
          self = NULL;
        }
      else
        {
          lumiera_filedescriptor_handle_release (self);
          lumiera_rwlock_unlock (&self->filelock, &NOBUG_FLAG (filedescriptor_dbg), &self->filelock_rh, NOBUG_CONTEXT);
        }
    }

  return self;
}


/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
