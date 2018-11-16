/*
  mmap.c  -  memory mapped access to files

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


/** @file mmap.c
 ** Implementation of management for memory mapped file regions
 ** @todo development in this area is stalled since 2010
 */


#include "include/logging.h"
#include "lib/safeclib.h"

#include "vault/mmap.h"
#include "vault/mmapcache.h"
#include "common/config.h"

#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>



LUMIERA_ERROR_DEFINE (MMAP_NWRITE, "Backing file not writable");
LUMIERA_ERROR_DEFINE (MMAP_SPACE, "Address space exhausted");





LumieraMMap
lumiera_mmap_init (LumieraMMap self, LumieraFile file, off_t start, size_t size)
{
  TRACE (mmap_dbg);

  TODO ("enforce size instead using chunksize (rounded to page size) for parsing headers");

  REQUIRE (self);
  REQUIRE (file);
  REQUIRE (start >= 0);
  REQUIRE (size);

  /**
   * default size for the mmapping window.
   * - 128MB on 32 bit arch
   * - 2GB on 64 bit arch
   * 
   * \par
   * Maintaining the right[tm] mmapping size is a bit tricky:
   *  - We have the default mmap_window_size which will be backed off when address space gets exhausted
   *  - When a bigger size is requested we have to fulfil it
   *  - The last mmapped chunk of a file can be as small as possible when the file is readonly
   *  - When the file is writable, the last chunk should be rounded up to chunksize
   *  - All boundaries will be aligned up/down to chunk boundaries
   *  - Requests beyond the file end must ftruncate and map additional pages
   *  - Create the 'refmap' which contains a refcounter per chunk
   */
  TODO("move the setdefaults somewhere else, backend_defaults.c or so");
#if SIZE_MAX <= 4294967295U
  lumiera_config_setdefault ("vault.mmap.window_size = 134217728");
#else
  lumiera_config_setdefault ("vault.mmap.window_size = 2147483648");
#endif

  long long mmap_window_size = 0;
  lumiera_config_number_get ("vault.mmap.window_size", &mmap_window_size);

  LumieraFiledescriptor descriptor = file->descriptor;

  int fd = lumiera_file_handle_acquire (file);
  TRACE (mmap_dbg, "got fd %d", fd);
  if (fd == -1)
    goto efile;

  void* addr = (void*)-1;
  off_t begin = 0;
  size_t length = 0;
  size_t chunksize = lumiera_file_chunksize_get (file);
  size_t bias = lumiera_file_bias_get (file);

  REQUIRE(start >= (off_t)bias, "begin before first chunk");

  TODO ("error here? or just map as asked for?");
  ENSURE(chunksize);


  /**
   * Recovering address space strategies:
   * mmap() will fail when too much memory got mmapped after some time which is then
   * recovered in the following way
   *  1. create a new mmap while the cachelimit is not reached.
   *  2. All unused mmaps are kept in a mrucache, drop the oldest one.
   *   mmap() still fails..
   *  3.a When the intended mmapping size is the same as mmap_window_size then reduce (/2) the window size and retry.
   *  3.b When the intended mmapping size was bigger than mmap_window_size then free more mmaps from the cache.
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

  while (addr == (void*)-1)
    {
      TODO ("check if current mmapped size exceeds configured as_size (as_size be smaller than retrieved from getrlimit())");

      TODO ("use resourcecllector here");
      switch (strategy++)
        {
        case FIRST_TRY:
          TRACE (mmap_dbg, "FIRST_TRY");
          /* align begin and end to chunk boundaries */
          begin = ((start-bias) & ~(chunksize-1)) + bias;
          length = ((start+size+chunksize-1) & ~(chunksize-1)) - begin;

          if (begin+(off_t)length > descriptor->stat.st_size)
            {
              /* request past the end */
              if ((descriptor->flags & O_ACCMODE) == O_RDWR)
                {
                  /* extend file (writable) */
                  if (ftruncate (fd, begin+length) == -1)
                    {
                      LUMIERA_ERROR_SET (mmap, ERRNO, lumiera_filedescriptor_name (file->descriptor));
                      goto etruncate;
                    };
                  descriptor->stat.st_size = begin+length;
                  descriptor->realsize = start+size;
                }
            }
          else if (length < (size_t)mmap_window_size)
            length = mmap_window_size;

          if ((descriptor->flags & O_ACCMODE) == O_RDONLY)
            {
              /* The last mmapped chunk of a file can be as small as possible when the file is readonly */
              length = start+size - begin;
            }
          break;

        case DROP_FROM_CACHE:
          TRACE (mmap_dbg, "drop a mapping from cache");
          UNIMPLEMENTED ("mmap cache drop");
          break;

        case REDUCE_WINDOW:
          NOTICE (mmap_dbg, "mmaping window reduced to NN MB");
          UNIMPLEMENTED ("mmap window reduce");
          break;

        case REDUCE_IN_USE:
          NOTICE (mmap_dbg, "reduce mmapings in use");
          UNIMPLEMENTED ("mmapings in use reduce");
          break;

        case GIVE_UP:
          LUMIERA_ERROR_SET_ALERT (mmap, MMAP_SPACE, lumiera_filedescriptor_name (file->descriptor));
          goto espace;
        }

      addr = mmap (NULL,
                   length,
                   (descriptor->flags & O_ACCMODE) == O_RDONLY ? PROT_READ : PROT_READ|PROT_WRITE,
                   MAP_SHARED,
                   fd,
                   begin);

      INFO_IF (addr==(void*)-1, mmap_dbg, "mmap failed %s", strerror (errno));
      ENSURE (errno == 0 || errno == ENOMEM, "unexpected mmap error %s", strerror (errno));
    }

  llist_init (&self->cachenode);
  llist_init (&self->searchnode);

  self->start = begin;
  self->size = length;
  self->address = addr;
  self->refmap = lumiera_calloc ((length-1)/chunksize+1, sizeof (unsigned short));
  self->refcnt = 1;
  lumiera_mmapcache_announce (self);

  lumiera_file_handle_release (file);
  return self;

 espace:
 etruncate:
 efile:
  lumiera_file_handle_release (file);
  return NULL;
}


LumieraMMap
lumiera_mmap_init_exact (LumieraMMap self, LumieraFile file, off_t start, size_t size)
{
  TRACE (mmap_dbg);

  REQUIRE (self);
  REQUIRE (file);
  REQUIRE (start >= 0);
  REQUIRE (size);

  LumieraFiledescriptor descriptor = file->descriptor;

  int fd = lumiera_file_handle_acquire (file);
  TRACE (mmap_dbg, "got fd %d", fd);
  if (fd == -1)
    goto efile;

  if ((off_t)(start + size) > descriptor->stat.st_size)
    {
      /* request past the end */
      if ((descriptor->flags & O_ACCMODE) == O_RDWR)
        {
          /* extend file (writable) */
          if (ftruncate (fd, start+size) == -1)
            {
              LUMIERA_ERROR_SET (mmap, ERRNO, lumiera_filedescriptor_name (file->descriptor));
              goto etruncate;
            };
          descriptor->stat.st_size = descriptor->realsize = start+size;
        }
    }

  /////////////////////////TODO use resourcecllector here
  void* addr = mmap (NULL,
                     size,
                     (descriptor->flags & O_ACCMODE) == O_RDONLY ? PROT_READ : PROT_READ|PROT_WRITE,
                     MAP_SHARED,
                     fd,
                     start);

  INFO_IF (addr==(void*)-1, mmap_dbg, "mmap failed %s", strerror (errno));
  ENSURE (errno == 0 || errno == ENOMEM, "unexpected mmap error %s", strerror (errno));

  llist_init (&self->cachenode);
  llist_init (&self->searchnode);

  self->start = start;
  self->size = size;
  self->address = addr;
  self->refmap = NULL;
  self->refcnt = 1;
  lumiera_mmapcache_announce (self);

  lumiera_file_handle_release (file);
  return self;

 etruncate:
 efile:
  lumiera_file_handle_release (file);
  return NULL;
}


LumieraMMap
lumiera_mmap_new (LumieraFile file, off_t start, size_t size)
{
  TRACE (mmap_dbg);

  LumieraMMap self = lumiera_mmapcache_mmap_acquire ();

  if (lumiera_mmap_init (self, file, start, size))
    return self;
  else
    {
      lumiera_free (self);
      return NULL;
    }
}


LumieraMMap
lumiera_mmap_new_exact (LumieraFile file, off_t start, size_t size)
{
  TRACE (mmap_dbg);

  LumieraMMap self = lumiera_mmapcache_mmap_acquire ();

  if (lumiera_mmap_init_exact (self, file, start, size))
    return self;
  else
    {
      lumiera_free (self);
      return NULL;
    }
}


void
lumiera_mmap_delete (LumieraMMap self)
{
  TRACE (mmap_dbg);
  if (self)
    {
      REQUIRE (self->refcnt <= 1);
      lumiera_mmapcache_forget (self);

      /* The matching mappings->lock must be hold or being irrelevant (mappings destructor) here, we can't asset this from here, good luck */
      llist_unlink (&self->searchnode);

      TRACE (mmap_dbg, "unmap at %p with size %zd", self->address, self->size);
      munmap (self->address, self->size);
      lumiera_free (self->refmap);
      lumiera_free (self);
    }
}


void*
lumiera_mmap_destroy_node (LList node)
{
  TRACE (mmap_dbg);
  REQUIRE (llist_is_empty (node));
  LumieraMMap self = (LumieraMMap)node;

  lumiera_mmapcache_forget (self);

  llist_unlink (&self->searchnode); FIXME ("must lock mmappings -> deadlock");

  munmap (self->address, self->size);
  lumiera_free (self->refmap);

  return self;
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
