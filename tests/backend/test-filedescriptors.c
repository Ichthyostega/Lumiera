/*
  test-filedescriptors.c

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

#include "backend/config.h"
#include "backend/backend.h"
#include "backend/filedescriptor.h"

#include "tests/test.h"

TESTS_BEGIN

TEST ("acquire_existing")
{
  lumiera_config_init ("./");
  lumiera_backend_init ();
  LumieraFiledescriptor descriptor = lumiera_filedescriptor_acquire (",tmp_testfile", LUMIERA_FILE_READONLY);
  if (descriptor)
    {
      lumiera_filedescriptor_release (descriptor);
      lumiera_backend_destroy ();
      lumiera_config_destroy ();
      return 0;
    }
  else
    return 1;
}

TEST ("acquire_existing_again")
{
  lumiera_config_init ("./");
  lumiera_backend_init ();
  LumieraFiledescriptor descriptor = lumiera_filedescriptor_acquire (",tmp_testfile", LUMIERA_FILE_READONLY);
  if (descriptor)
    {
      LumieraFiledescriptor descriptor2 = lumiera_filedescriptor_acquire (",tmp_testfile", LUMIERA_FILE_READONLY);
      lumiera_filedescriptor_release (descriptor2);
      lumiera_filedescriptor_release (descriptor);
      lumiera_backend_destroy ();
      lumiera_config_destroy ();
      return 0;
    }
  else
    return 1;
}

TEST ("acquire_existing_3files")
{
  lumiera_config_init ("./");
  lumiera_backend_init ();
  LumieraFiledescriptor descriptor1 = lumiera_filedescriptor_acquire (",tmp_testfile1", LUMIERA_FILE_READONLY);

  LumieraFiledescriptor descriptor2 = lumiera_filedescriptor_acquire (",tmp_testfile2", LUMIERA_FILE_READONLY);

  LumieraFiledescriptor descriptor3 = lumiera_filedescriptor_acquire (",tmp_testfile3", LUMIERA_FILE_READONLY);
  if (descriptor1)
    lumiera_filedescriptor_release (descriptor1);

  if (descriptor2)
    lumiera_filedescriptor_release (descriptor2);

  if (descriptor3)
    lumiera_filedescriptor_release (descriptor3);

  if (descriptor1 && descriptor2 && descriptor3)
    {
      lumiera_backend_destroy ();
      lumiera_config_destroy ();
      return 0;
    }
  else
    return 1;
}

TEST ("acquire_create")
{
  lumiera_config_init ("./");
  lumiera_backend_init ();
  LumieraFiledescriptor descriptor = lumiera_filedescriptor_acquire (",tmp_testfile", LUMIERA_FILE_CREATE);
  if (descriptor)
    {
      lumiera_filedescriptor_release (descriptor);
      lumiera_backend_destroy ();
      lumiera_config_destroy ();
      return 0;
    }
  else
    return 1;
}

TEST ("acquire_create_dir")
{
  lumiera_config_init ("./");
  lumiera_backend_init ();
  LumieraFiledescriptor descriptor =
    lumiera_filedescriptor_acquire (",tmp_testdir/nested/,tmp_testfile", LUMIERA_FILE_CREATE);
  if (descriptor)
    {
      lumiera_filedescriptor_release (descriptor);
      lumiera_backend_destroy ();
      lumiera_config_destroy ();
      return 0;
    }
  else
    return 1;
}

TESTS_END
