/*
  TEST-FILEMAP  -  test file memory mapping

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

/** @file test-filemmap.c
 ** C unit test to cover memory mapping of file contents
 ** @see mmapings.h
 */


//#include <stdio.h>
//#include <string.h>

#include "lib/llist.h"
#include "common/config.h"

#include "backend/backend.h"
#include "backend/file.h"
#include "backend/filedescriptor.h"
#include "backend/mmapings.h"
#include "backend/mmap.h"

#include "lib/test/test.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

TESTS_BEGIN

/*
  The first two are not really tests, but some code to check for
  filesystem and kernel semantics which might be moved elsewhere someday
*/
TEST (mmap_semantic)
{
  int fd = open (",mmaptest", O_RDWR|O_CREAT, 0666);
  printf ("got fd %d\n", fd);
  printf ("error %s\n", strerror (errno));
  int dummy = ftruncate (fd, 8192);
  (void)dummy;
  TODO ("handle error case better");

  void* addr = mmap (NULL,
                     8192,
                     PROT_WRITE,
                     MAP_SHARED,
                     fd,
                     0);

  printf ("mapped at %p\n", addr);
  printf ("error %s\n", strerror (errno));

  int i = 0;
  while (++i && ! errno)
    {
      addr = mmap (NULL,
                   4096,
                   PROT_READ|PROT_WRITE,
                   MAP_SHARED,
                   fd,
                   0);

      printf ("mapped %d again %p\n", i, addr);
      printf ("error %s\n", strerror (errno));
    }
}

TEST (fd_semantic)
{
  mkdir (",testdir", 0777); errno = 0;

  int i = 0;
  while (++i && ! errno)
    {
      char name[256];

      sprintf (name, ",testdir/file_%d", i);
      int fd = open (name, O_RDWR|O_CREAT, 0666);
      printf ("#%d opened %d\n", i, fd);
      printf ("error %s\n", strerror (errno));

      void* addr = mmap (NULL,
                         8192,
                         PROT_WRITE,
                         MAP_SHARED,
                         fd,
                         0);
      printf ("mapped at %p\n", addr);

      close (fd);
    }

}


TEST (mmap_missing_chunksize)
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_CREATE);

  LumieraMMapings mmaps = lumiera_file_mmapings (file);

  if (!mmaps)
    fprintf (stderr, "%s\n", lumiera_error());

  lumiera_file_delete (file);

  lumiera_backend_destroy ();
  lumiera_config_destroy ();
}


TEST (mmap_forget_releasing)
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_CREATE);
  lumiera_file_set_chunksize_bias (file, 4096, 0);

  LumieraMMapings mmaps = lumiera_file_mmapings (file);

  LumieraMMap map = lumiera_mmapings_mmap_acquire (mmaps, file, 0, 100);

  (void) map;  //lumiera_mmapings_release_mmap (mmaps, &user, mmap);

  lumiera_file_delete (file);

  lumiera_backend_destroy ();
  lumiera_config_destroy ();
}


TEST (mmap_simple)
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_CREATE);
  lumiera_file_set_chunksize_bias (file, 4096, 0);

  LumieraMMapings mmaps = lumiera_file_mmapings (file);

  LumieraMMap map = lumiera_mmapings_mmap_acquire (mmaps, file, 0, 100);

  lumiera_mmapings_release_mmap (mmaps, map);

  lumiera_file_delete (file);

  /* check that the file got truncated to the desired size */
  struct stat st;
  stat (",tmp-filemmap", &st);
  CHECK (st.st_size == 100);

  lumiera_backend_destroy ();
  lumiera_config_destroy ();
}



TEST (mmap_checkout_twice)
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_CREATE);
  lumiera_file_set_chunksize_bias (file, 4096, 0);

  LumieraMMapings mmaps = lumiera_file_mmapings (file);

  LumieraMMap map = lumiera_mmapings_mmap_acquire (mmaps, file, 0, 100);

  LumieraMMap map2 = lumiera_mmapings_mmap_acquire (mmaps, file, 0, 100);

  CHECK (map->address == map2->address);

  lumiera_mmapings_release_mmap (mmaps, map);

  lumiera_mmapings_release_mmap (mmaps, map2);

  lumiera_file_delete (file);

  /* check that the file got truncated to the desired size */
  struct stat st;
  stat (",tmp-filemmap", &st);
  CHECK (st.st_size == 100);

  lumiera_backend_destroy ();
  lumiera_config_destroy ();
}



TEST (mmap_checkout_again)
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_CREATE);
  lumiera_file_set_chunksize_bias (file, 4096, 0);

  LumieraMMapings mmaps = lumiera_file_mmapings (file);

  LumieraMMap map = lumiera_mmapings_mmap_acquire (mmaps, file, 0, 100);
  lumiera_mmapings_release_mmap (mmaps, map);

  LumieraMMap map2 = lumiera_mmapings_mmap_acquire (mmaps, file, 0, 100);
  lumiera_mmapings_release_mmap (mmaps, map2);

  lumiera_file_delete (file);

  /* check that the file got truncated to the desired size */
  struct stat st;
  stat (",tmp-filemmap", &st);
  CHECK (st.st_size == 100);

  lumiera_backend_destroy ();
  lumiera_config_destroy ();
}



TEST (mmap_grow_existing_file)
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_READWRITE);

  lumiera_file_set_chunksize_bias (file, 4096, 0);

  LumieraMMapings mmaps = lumiera_file_mmapings (file);

  LumieraMMap map = lumiera_mmapings_mmap_acquire (mmaps, file, 0, 100);
  lumiera_mmapings_release_mmap (mmaps, map);

  lumiera_file_delete (file);

  /* check that the file got truncated to the desired size */
  struct stat st;
  stat (",tmp-filemmap", &st);
  CHECK (st.st_size == 100);

  lumiera_backend_destroy ();
  lumiera_config_destroy ();
}


TEST (mmap_readonly_file)
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_READONLY);

  lumiera_file_set_chunksize_bias (file, 4096, 0);

  LumieraMMapings mmaps = lumiera_file_mmapings (file);

  LumieraMMap map = lumiera_mmapings_mmap_acquire (mmaps, file, 0, 100);
  lumiera_mmapings_release_mmap (mmaps, map);

  lumiera_file_delete (file);

  /* check that the file got truncated to the desired size */
  struct stat st;
  stat (",tmp-filemmap", &st);
  CHECK (st.st_size == 100);

  lumiera_backend_destroy ();
  lumiera_config_destroy ();
}



TEST (file_access)
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_RECREATE);
  lumiera_file_set_chunksize_bias (file, 4096, 0);

  LumieraMMap map = lumiera_file_mmap_acquire (file, 10, 100);

  char* addr = lumiera_mmap_address (map, 20);

  strcpy (addr, "test");

  lumiera_file_release_mmap (file, map);

  lumiera_file_delete (file);
  lumiera_backend_destroy ();
}



TEST (exact_mmap)
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_RECREATE);

  LumieraMMap map = lumiera_mmap_new_exact (file, 0, 6);

  char* addr = lumiera_mmap_address (map, 1);

  strcpy (addr, "test");

  lumiera_mmap_delete (map);

  lumiera_file_delete (file);
  lumiera_backend_destroy ();
}



TEST (mmap_section)
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_RECREATE);
  lumiera_file_set_chunksize_bias (file, 4096, 0);

  LUMIERA_FILE_MMAP_SECTION (NOBUG_ON, file, 20, 20, addr)
    {
      strcpy (addr, "mmap section");
    }

  CHECK(lumiera_error_peek() == NULL);

  lumiera_file_delete (file);
  lumiera_backend_destroy ();
}


TEST (mmap_section_err)
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_RECREATE);
  /* forgot to set lumiera_file_chunksize_set (file, 4096); */

  LUMIERA_FILE_MMAP_SECTION (NOBUG_ON, file, 20, 20, addr)
    {
      strcpy (addr, "mmap section");
    }

  CHECK(lumiera_error_peek() == NULL);

  lumiera_file_delete (file);
  lumiera_backend_destroy ();
}


#if 0
TEST (refactored_usage)
{
  LumieraFile file = lumiera_file_new ("filename", mode);

  LumieraFrameIndex index = lumiera_frameindex_new ("indexfilename", file /*, indexing engine*/);

  LumieraFrame frame = lumiera_frameindex_frame (index, 123);

  //TAG+HINTS could be NEXT|PREV|EXACT|NEAREST|HARD|SOFT

  lumiera_frame_prefetch (index, 123);


  lumiera_frame_release (frame);

  lumiera_frameindex_delete (index);

  lumiera_file_free (file);
}
#endif


TESTS_END
