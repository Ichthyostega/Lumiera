/*
  mmap.c  -  memory mapped acces to files

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

#include "lib/safeclib.h"

#include "backend/mmap.h"
#include "backend/mmapcache.h"
#include "backend/config.h"

#include <unistd.h>
#include <sys/mman.h>


/**
 * @file
 *
 */

NOBUG_DEFINE_FLAG_PARENT (mmap_all, backend);
NOBUG_DEFINE_FLAG_PARENT (mmap, mmap_all);


LUMIERA_ERROR_DEFINE (MMAP_NWRITE, "Backing file not writeable");
LUMIERA_ERROR_DEFINE (MMAP_SPACE, "Address space exhausted");


/**
 * global mmap registry/cache
 *
 */


LumieraMMap
lumiera_mmap_init (LumieraMMap self, LumieraFile file, LList acquirer, off_t start, size_t size, size_t chunksize)
{
  TRACE (mmap);

  REQUIRE (self);
  REQUIRE (file);
  REQUIRE (acquirer);
  REQUIRE (llist_is_empty (acquirer));
  REQUIRE (start >= 0);
  REQUIRE (size);

  static int once = 0;
  if (!once)
    {
      once = 1;
      /**
       * default size for the mmaping window
       * 128MB on 32 bit arch
       * 2GB on 64 bit arch
       */
#if SIZE_MAX <= 4294967295U
      lumiera_config_setdefault ("backend.mmap.window_size = 134217728");
#else
      lumiera_config_setdefault ("backend.mmap.window_size = 2147483648");
#endif
    }

  long long mmap_window_size = 0;
  lumiera_config_number_get ("backend.mmap.window_size", &mmap_window_size);

  LumieraFiledescriptor descriptor = file->descriptor;

  int fd = lumiera_file_handle_acquire (file);
  TRACE (mmap, "got fd %d", fd);
  if (fd == -1)
    goto efile;

  void* addr = NULL;
  off_t begin = 0;
  size_t length = 0;

  /**
   * Maintaining the right[tm] mmaping size is a bit tricky:
   *  - We have the default mmap_window_size which will be backed off when address space gets exhausted
   *  - When a biggier size is requested we have to fullfill it
   *  - The last mmaped chunk of a file can be as small as possible when the file is readonly
   *  - When the file is writeable, the last chunk should be rounded up to chunksize
   *  - All boundaries will be aligned up/down to chunk boundaries
   *  - Requests beyond the file end must ftruncate and map additional pages
   *  - Create the 'refmap' which contains a refounter per chunk
   **/
  while (!addr)
    {
      /**
       * Recovering address space strategies:
       * mmap() will fail when too much memory got mmaped after some time which is then
       * recovered in the following way
       *  1. create a new mmap while the cachelimit is not reached.
       *  2. All unused mmaps are kept in a mrucache, drop the oldest one.
       *   mmap() still fails..
       *  3.a When the intented mmaping size is the same as mmap_window_size then reduce (/2) the window size and retry.
       *  3.b When the intented mmaping size was biggier than mmap_window_size then free more mmaps from the cache.
       *  4 When the cache is empty (that means all mmaps in use), scan the mmaps in use if they can be reduced
       *    mmap_window_size is already reduced now (half of refmap free from either end)
       **/
      enum {
        FIRST_TRY,
        DROP_FROM_CACHE,
        REDUCE_WINDOW,
        REDUCE_IN_USE,
        GIVE_UP
      } strategy = FIRST_TRY;

      switch (strategy++)
        {
        case FIRST_TRY:
          /* align begin and end to chunk boundaries */
          begin = start & ~(chunksize-1);
          length = ((start+size+chunksize-1) & ~(chunksize-1)) - begin;

          if (begin+(off_t)length > descriptor->stat.st_size)
            {
              /* request past the end */
              if ((descriptor->flags & O_ACCMODE) == O_RDWR)
                {
                  /* extend file (writeable) */
                  if (ftruncate (fd, begin+length) == -1)
                    {
                      LUMIERA_ERROR_SET (mmap, ERRNO);
                      goto etruncate;
                    };
                  descriptor->stat.st_size = begin+length;
                  descriptor->realsize = start+size;
                }
              else
                {
                  LUMIERA_ERROR_SET (mmap, MMAP_NWRITE);
                  goto ewrite;
                }
            }
          else if (length < (size_t)mmap_window_size)
            length = mmap_window_size;

          if ((descriptor->flags & O_ACCMODE) == O_RDONLY)
            /* The last mmaped chunk of a file can be as small as possible when the file is readonly */
            length = start+size - begin;
          break;

        case DROP_FROM_CACHE:
          TRACE (mmap, "drop a mapping from cache");
          UNIMPLEMENTED ("mmap cache drop");
          break;

        case REDUCE_WINDOW:
          NOTICE (mmap, "mmaping window reduced to NN MB");
          UNIMPLEMENTED ("mmap window reduce");
          break;

        case REDUCE_IN_USE:
          NOTICE (mmap, "reduce mmapings in use");
          UNIMPLEMENTED ("mmapings in use reduce");
          break;

        case GIVE_UP:
          LUMIERA_ERROR_SET (mmap, MMAP_SPACE);
          goto espace;
        }

      addr = mmap (NULL,
                   length,
                   (descriptor->flags & O_ACCMODE) == O_RDONLY ? PROT_READ : PROT_READ|PROT_WRITE,
                   MAP_SHARED,
                   fd,
                   begin);
    }

  llist_init (&self->cachenode);
  llist_init (&self->searchnode);

  self->start = begin;
  self->size = length;
  self->address = addr;

  self->refmap = lumiera_calloc (length/chunksize, sizeof (unsigned short));

  llist_insert_head (&self->cachenode, acquirer);

  lumiera_mmapcache_announce (lumiera_mcache, self);

  lumiera_file_handle_release (file);
  return self;

 espace:
 etruncate:
 ewrite:
 efile:
  lumiera_file_handle_release (file);
  free (self);
  return NULL;
}

LumieraMMap
lumiera_mmap_new (LumieraFile file, LList acquirer, off_t start, size_t size, size_t chunksize)
{
  TRACE (mmap);

  LumieraMMap self = lumiera_mmapcache_mmap_acquire (lumiera_mcache);

  if (lumiera_mmap_init (self, file, acquirer, start, size, chunksize))
    return self;
  else
    {
      free (self);
      return NULL;
    }
}


void
lumiera_mmap_delete (LumieraMMap self)
{
  TRACE (mmap);
  if (self)
    {
      //LUMIERA_MUTEX_SECTION (mmapings, self->rh, &self->lock)

      lumiera_mmapcache_forget (lumiera_mcache, self);
      llist_unlink (&self->searchnode); TODO ("must lock mmapings -> deadlock");
      munmap (self->address, self->size);
      free (self->refmap);
      free (self);
    }
}


void*
lumiera_mmap_destroy_node (LList node)
{
  TRACE (mmap);
  REQUIRE (llist_is_empty (node));
  LumieraMMap self = (LumieraMMap)node;

  lumiera_mmapcache_forget (lumiera_mcache, self);

  llist_unlink (&self->searchnode); TODO ("must lock mmapings -> deadlock");

  munmap (self->address, self->size);
  free (self->refmap);

  return self;
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
