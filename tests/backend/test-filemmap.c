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

#include "backend/backend.h"
#include "backend/file.h"
#include "backend/filedescriptor.h"
#include "backend/mmapings.h"
#include "backend/mmap.h"

#include "tests/test.h"

TESTS_BEGIN

TEST ("detail_usage")
{
  lumiera_backend_init ();
  LumieraFile file = lumiera_file_new (",tmp-filemmap", LUMIERA_FILE_CREATE, 4096);

  LumieraMMapings mmaps = lumiera_file_mmapings (file);

  llist user;
  llist_init (&user);

  LumieraMMap mmap = lumiera_mmapings_mmap_acquire (mmaps, file, &user, 0, 100);


  lumiera_mmapings_release_mmap (mmaps, &user, mmap);

  lumiera_file_delete (file);
  lumiera_backend_destroy ();
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
