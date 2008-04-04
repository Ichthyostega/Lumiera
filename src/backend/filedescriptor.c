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

#include "backend/filedescriptor.h"

#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>


NOBUG_DEFINE_FLAG (filedescriptor);

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
  return i->stat.st_dev^i->stat.st_ino^i->flags^((i->stat.st_dev^i->stat.st_ino^i->flags)>>7)^r;
}

static size_t
h2 (const void* item, const uint32_t r)
{
  const LumieraFiledescriptor i = *(const LumieraFiledescriptor*)item;
  return i->stat.st_dev^i->stat.st_ino^i->flags^((i->stat.st_dev^i->stat.st_ino^i->flags)>>5)^r;
}

static size_t
h3 (const void* item, const uint32_t r)
{
  const LumieraFiledescriptor i = *(const LumieraFiledescriptor*)item;
  return i->stat.st_dev^i->stat.st_ino^i->flags^((i->stat.st_dev^i->stat.st_ino^i->flags)>>3)^r;
}


static int
cmp (const void* keya, const void* keyb)
{
  const LumieraFiledescriptor a = *(const LumieraFiledescriptor*)keya;
  const LumieraFiledescriptor b = *(const LumieraFiledescriptor*)keyb;
  return a->stat.st_dev == b->stat.st_dev && a->stat.st_ino == b->stat.st_ino && a->flags == b->flags;
}

static Cuckoo
lumiera_filedescriptor_registry_new (void)
{
  NOBUG_INIT_FLAG (filedescriptor);

  Cuckoo registry = cuckoo_new (h1, h2, h3, cmp,
                                sizeof (LumieraFiledescriptor),
                                3);
  if (!registry)
    LUMIERA_DIE (NO_MEMORY);

  return registry;
}

static void
lumiera_filedescriptor_registry_delete (void)
{
  cuckoo_free (registry);
}

/**
 * Find existing filedescriptor or create one
 * @param descriptor strong reference to be initialized with the filedescriptor,
 *        a filedescriptor will be deleted when its last string reference gets deleted.
 * @param name name of the file
 * @param flags opening flags for the filedescriptor
 * @return descriptor on success or NULL on error
 */
LumieraFiledescriptor
lumiera_filedescriptor_acquire (const char* name, int flags)
{
  TRACE (filedescriptor);
  UNCHECKED;
  lumiera_mutexacquirer registry_lock;
  lumiera_mutexacquirer_init_mutex (&registry_lock, &registry_mutex, LUMIERA_LOCKED);

  if (!registry)
    {
      registry = lumiera_filedescriptor_registry_new ();
      atexit (lumiera_filedescriptor_registry_delete);
    }

  lumiera_filedescriptor fdesc;
  fdesc.flags = flags;

  for (int retry = 0; !retry; ++retry)
    {
      if (stat (name, &fdesc.stat) == 0)
        break;
      else
        {
          if (errno == ENOENT && flags&O_CREAT)
            {
              INFO (filedescriptor, "try creating file: %s %d", name, PATH_MAX);
              char* dir = lumiera_tmpbuf_strndup (name, PATH_MAX);
              char* slash = dir;

              while ((slash = strchr (slash+1, '/')))
                {
                  *slash = '\0';
                  INFO (filedescriptor, "try creating dir: %s", dir);
                  if (mkdir (dir, 0777) == -1)
                    {
                      LUMIERA_ERROR_SET (filedescriptor, ERRNO);
                      goto efile;
                    }
                  *slash = '/';
                }

              int fd;
              fd = creat (name, 0777);
              if (fd == -1)
                {
                  LUMIERA_ERROR_SET (filedescriptor, ERRNO);
                  goto efile;
                }
              close (fd);
              continue; /* will now retry the fstat once */
            }
        }
      /* finally, no luck */
      LUMIERA_ERROR_SET (filedescriptor, ERRNO);
      goto efile;
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

/**
 * Allocate a new filedescriptor cloned from a template
 * @param template the source filedescriptor
 * @return the constrccted filedescriptor
 */
LumieraFiledescriptor
lumiera_filedescriptor_new (LumieraFiledescriptor template)
{
  LumieraFiledescriptor self = lumiera_malloc (sizeof (lumiera_filedescriptor));
  TRACE (filedescriptor, "at %p", self);

  self->stat = template->stat;

  self->flags = template->flags;
  lumiera_mutex_init (&self->lock);
  self->reopened = 0;
  self->refcount = 1;
  self->handle = 0;

  RESOURCE_ANNOUNCE (filedescriptor, "mutex", "filedescriptor", self, self->rh);

  return self;
}

/**
 * delete a filedescriptor and free its resources
 * @param descriptor filedescriptor to be freed
 */
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
