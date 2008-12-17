/*
  test-files.c  -  test file management

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

//#include <stdio.h>
//#include <string.h>

#include "lib/llist.h"
#include "lib/config.h"

#include "backend/backend.h"
#include "backend/file.h"
#include "backend/filedescriptor.h"
#include "backend/mmapings.h"
#include "backend/mmap.h"

#include "tests/test.h"

#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>

TESTS_BEGIN

/*
  The first two are not really tests, but some code to check for
  filesystem and kernel semantics which might be moved elsewhere someday
*/
TEST ("mmap_semantic")
{
  int fd = open (",mmaptest", O_RDWR|O_CREAT, 0666);
  printf ("got fd %d\n", fd);
  printf ("error %s\n", strerror (errno));
  ftruncate (fd, 8192);

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

TEST ("fd_semantic")
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


TEST ("mmap_missing_chunksize")
{
  lumiera_config_init ("./");
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_CREATE);

  LumieraMMapings mmaps = lumiera_file_mmapings (file);

  if (!mmaps)
    fprintf (stderr, "%s\n", lumiera_error());

  lumiera_file_delete (file);

  lumiera_backend_destroy ();
  lumiera_config_destroy ();
}


TEST ("mmap_forget_releasing")
{
  lumiera_config_init ("./");
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_CREATE);
  lumiera_file_chunksize_set (file, 4096);

  LumieraMMapings mmaps = lumiera_file_mmapings (file);

  llist user;
  llist_init (&user);

  LumieraMMap mmap = lumiera_mmapings_mmap_acquire (mmaps, file, &user, 0, 100);
 
  (void) mmap;  //lumiera_mmapings_release_mmap (mmaps, &user, mmap);

  lumiera_file_delete (file);

  lumiera_backend_destroy ();
  lumiera_config_destroy ();
}


TEST ("mmap_simple")
{
  lumiera_config_init ("./");
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_CREATE);
  lumiera_file_chunksize_set (file, 4096);

  LumieraMMapings mmaps = lumiera_file_mmapings (file);

  llist user;
  llist_init (&user);
  LumieraMMap mmap = lumiera_mmapings_mmap_acquire (mmaps, file, &user, 0, 100);

  lumiera_mmapings_release_mmap (mmaps, &user, mmap);

  lumiera_file_delete (file);

  /* check that the file got truncated to the desired size */
  struct stat st;
  stat (",tmp-filemmap", &st);
  ENSURE (st.st_size == 100);

  lumiera_backend_destroy ();
  lumiera_config_destroy ();
}



TEST ("mmap_checkout_twice")
{
  lumiera_config_init ("./");
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_CREATE);
  lumiera_file_chunksize_set (file, 4096);

  LumieraMMapings mmaps = lumiera_file_mmapings (file);

  llist user;
  llist_init (&user);
  LumieraMMap mmap = lumiera_mmapings_mmap_acquire (mmaps, file, &user, 0, 100);

  llist user2;
  llist_init (&user2);
  LumieraMMap mmap2 = lumiera_mmapings_mmap_acquire (mmaps, file, &user2, 0, 100);

  ENSURE (lumiera_mmap_address (mmap) == lumiera_mmap_address (mmap2));

  lumiera_mmapings_release_mmap (mmaps, &user, mmap);

  lumiera_mmapings_release_mmap (mmaps, &user2, mmap2);

  lumiera_file_delete (file);

  /* check that the file got truncated to the desired size */
  struct stat st;
  stat (",tmp-filemmap", &st);
  ENSURE (st.st_size == 100);

  lumiera_backend_destroy ();
  lumiera_config_destroy ();
}



TEST ("mmap_checkout_again")
{
  lumiera_config_init ("./");
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_CREATE);
  lumiera_file_chunksize_set (file, 4096);

  LumieraMMapings mmaps = lumiera_file_mmapings (file);

  llist user;
  llist_init (&user);
  LumieraMMap mmap = lumiera_mmapings_mmap_acquire (mmaps, file, &user, 0, 100);
  lumiera_mmapings_release_mmap (mmaps, &user, mmap);

  llist user2;
  llist_init (&user2);
  LumieraMMap mmap2 = lumiera_mmapings_mmap_acquire (mmaps, file, &user2, 0, 100);
  lumiera_mmapings_release_mmap (mmaps, &user2, mmap2);

  lumiera_file_delete (file);

  /* check that the file got truncated to the desired size */
  struct stat st;
  stat (",tmp-filemmap", &st);
  ENSURE (st.st_size == 100);

  lumiera_backend_destroy ();
  lumiera_config_destroy ();
}



TEST ("mmap_grow_existing_file")
{
  lumiera_config_init ("./");
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_READWRITE);

  lumiera_file_chunksize_set (file, 4096);

  LumieraMMapings mmaps = lumiera_file_mmapings (file);

  llist user;
  llist_init (&user);

  LumieraMMap mmap = lumiera_mmapings_mmap_acquire (mmaps, file, &user, 0, 100);
  lumiera_mmapings_release_mmap (mmaps, &user, mmap);

  lumiera_file_delete (file);

  /* check that the file got truncated to the desired size */
  struct stat st;
  stat (",tmp-filemmap", &st);
  ENSURE (st.st_size == 100);

  lumiera_backend_destroy ();
  lumiera_config_destroy ();
}


TEST ("mmap_readonly_file")
{
  lumiera_config_init ("./");
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_READONLY);

  lumiera_file_chunksize_set (file, 4096);

  LumieraMMapings mmaps = lumiera_file_mmapings (file);

  llist user;
  llist_init (&user);

  LumieraMMap mmap = lumiera_mmapings_mmap_acquire (mmaps, file, &user, 0, 100);
  lumiera_mmapings_release_mmap (mmaps, &user, mmap);

  lumiera_file_delete (file);

  /* check that the file got truncated to the desired size */
  struct stat st;
  stat (",tmp-filemmap", &st);
  ENSURE (st.st_size == 100);

  lumiera_backend_destroy ();
  lumiera_config_destroy ();
}





#if 0
TEST ("refactored_usage")
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
