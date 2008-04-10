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

#include "lib/mutex.h"
#include "lib/safeclib.h"
#include "lib/cuckoo.h"

#include "backend/file.h"
#include "backend/filedescriptor.h"
#include "backend/filehandle.h"
#include "backend/filehandlecache.h"

#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>


NOBUG_DEFINE_FLAG_PARENT (filedescriptor, file_all);

/*
  Filedescriptor registry

  This registry stores all acquired filedescriptors for lookup, they will be freed when not referenced anymore.
 */
static Cuckoo registry = NULL;
static lumiera_mutex registry_mutex = {PTHREAD_MUTEX_INITIALIZER};

/*
 * setup hashing and compare functions for cuckoo hashing
 */
static size_t
h1 (const void* item, const uint32_t r)
{
  const LumieraFiledescriptor i = *(const LumieraFiledescriptor*)item;
  return i->stat.st_dev^i->stat.st_ino^(i->flags&LUMIERA_FILE_MASK)
    ^((i->stat.st_dev^i->stat.st_ino^(i->flags&LUMIERA_FILE_MASK))>>7)^r;
}

static size_t
h2 (const void* item, const uint32_t r)
{
  const LumieraFiledescriptor i = *(const LumieraFiledescriptor*)item;
  return i->stat.st_dev^i->stat.st_ino^(i->flags&LUMIERA_FILE_MASK)
    ^((i->stat.st_dev^i->stat.st_ino^(i->flags&LUMIERA_FILE_MASK))>>5)^r;
}

static size_t
h3 (const void* item, const uint32_t r)
{
  const LumieraFiledescriptor i = *(const LumieraFiledescriptor*)item;
  return i->stat.st_dev^i->stat.st_ino^(i->flags&LUMIERA_FILE_MASK)
    ^((i->stat.st_dev^i->stat.st_ino^(i->flags&LUMIERA_FILE_MASK))>>3)^r;
}


static int
cmp (const void* keya, const void* keyb)
{
  const LumieraFiledescriptor a = *(const LumieraFiledescriptor*)keya;
  const LumieraFiledescriptor b = *(const LumieraFiledescriptor*)keyb;
  return a->stat.st_dev == b->stat.st_dev && a->stat.st_ino == b->stat.st_ino
    && (a->flags&LUMIERA_FILE_MASK) == (b->flags&LUMIERA_FILE_MASK);
}

void
lumiera_filedescriptor_registry_init (void)
{
  NOBUG_INIT_FLAG (filedescriptor);
  TRACE (filedescriptor);
  REQUIRE (!registry);

  registry = cuckoo_new (h1, h2, h3, cmp,
                         sizeof (LumieraFiledescriptor),
                         3);
  if (!registry)
    LUMIERA_DIE (NO_MEMORY);
}

void
lumiera_filedescriptor_registry_destroy (void)
{
  TRACE (filedescriptor);
  REQUIRE (!cuckoo_nelements (registry));
  if (registry)
    cuckoo_free (registry);
  registry = NULL;
}


LumieraFiledescriptor
lumiera_filedescriptor_acquire (const char* name, int flags)
{
  TRACE (filedescriptor, "%s", name);
  REQUIRE (registry, "not initialized");

  lumiera_mutexacquirer registry_lock;
  lumiera_mutexacquirer_init_mutex (&registry_lock, &registry_mutex, LUMIERA_LOCKED);

  lumiera_filedescriptor fdesc;
  fdesc.flags = flags;

  if (stat (name, &fdesc.stat) != 0)
    {
      if (errno == ENOENT && flags&O_CREAT)
        {
          char* dir = lumiera_tmpbuf_strndup (name, PATH_MAX);
          char* slash = dir;
          while ((slash = strchr (slash+1, '/')))
            {
              *slash = '\0';
              INFO (filedescriptor, "try creating dir: %s", dir);
              if (mkdir (dir, 0777) == -1 && errno != EEXIST)
                {
                  LUMIERA_ERROR_SET (filedescriptor, ERRNO);
                  goto efile;
                }
              *slash = '/';
            }
          int fd;
          INFO (filedescriptor, "try creating file: %s", name);
          fd = creat (name, 0777);
          if (fd == -1)
            {
              LUMIERA_ERROR_SET (filedescriptor, ERRNO);
              goto efile;
            }
          close (fd);
          if (stat (name, &fdesc.stat) != 0)
            {
              /* finally, no luck */
              LUMIERA_ERROR_SET (filedescriptor, ERRNO);
              goto efile;
            }
        }
    }

  /* lookup/create descriptor */
  LumieraFiledescriptor dest = &fdesc;
  LumieraFiledescriptor* found = cuckoo_find (registry, &dest);

  if (!found)
    {
      TRACE (filedescriptor, "Descriptor not found");

      dest = lumiera_filedescriptor_new (&fdesc);
      if (!dest)
        goto ecreate;

      cuckoo_insert (registry, &dest);
    }
  else
    {
      TRACE (filedescriptor, "Descriptor already existing");
      dest = *found;
      ++dest->refcount;
    }

  lumiera_mutexacquirer_unlock (&registry_lock);
  return dest;

 efile:
 ecreate:
  lumiera_mutexacquirer_unlock (&registry_lock);
  return NULL;
}


void
lumiera_filedescriptor_release (LumieraFiledescriptor self)
{
  TRACE (filedescriptor);
  if (!--self->refcount)
    lumiera_filedescriptor_delete (self);
}


LumieraFiledescriptor
lumiera_filedescriptor_new (LumieraFiledescriptor template)
{
  LumieraFiledescriptor self = lumiera_malloc (sizeof (lumiera_filedescriptor));
  TRACE (filedescriptor, "at %p", self);

  self->stat = template->stat;

  self->flags = template->flags;
  lumiera_mutex_init (&self->lock);
  self->refcount = 1;
  self->handle = 0;

  const char* type = "mutex";
  const char* name = "filedescriptor";

  RESOURCE_ANNOUNCE (filedescriptor, type, name, self, self->rh);

  return self;
}


void
lumiera_filedescriptor_delete (LumieraFiledescriptor self)
{
  TRACE (filedescriptor, "%p", self);
  lumiera_mutexacquirer registry_lock;
  lumiera_mutexacquirer_init_mutex (&registry_lock, &registry_mutex, LUMIERA_LOCKED);

  REQUIRE (self->refcount == 0);

  RESOURCE_FORGET (filedescriptor, self->rh);

  cuckoo_remove (registry, cuckoo_find (registry, &self));

  TODO ("destruct other members (WIP)");


  TODO ("release filehandle");

  lumiera_mutex_destroy (&self->lock);
  free (self);

  lumiera_mutexacquirer_unlock (&registry_lock);
}
