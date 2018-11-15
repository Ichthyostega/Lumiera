/*
  TEST-FILEDESCRIPTORS  -  verify filedescriptor management

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

/** @file test-filedescriptors.c
 ** C unit test to cover filedescriptor management
 ** @see filedescriptor.h
 */


#include "lib/safeclib.h"

#include "common/config.h"
#include "vault/backend.h"
#include "vault/filedescriptor.h"

#include "lib/test/test.h"

TESTS_BEGIN

TEST (acquire_existing)
{
  lumiera_backend_init ();
  llist node;
  llist_init (&node);
  LumieraFiledescriptor descriptor = lumiera_filedescriptor_acquire (",tmp_testfile", LUMIERA_FILE_READONLY, &node);
  if (descriptor)
    {
      lumiera_filedescriptor_release (descriptor, ",tmp_testfile", &node);
      lumiera_backend_destroy ();
      lumiera_config_destroy ();
      return 0;
    }
  else
    return 1;
}


TEST (acquire_existing_again)
{
  lumiera_backend_init ();
  llist node;
  llist_init (&node);
  LumieraFiledescriptor descriptor = lumiera_filedescriptor_acquire (",tmp_testfile", LUMIERA_FILE_READONLY, &node);
  if (descriptor)
    {
      llist node2;
      llist_init (&node2);
      LumieraFiledescriptor descriptor2 = lumiera_filedescriptor_acquire (",tmp_testfile", LUMIERA_FILE_READONLY, &node2);
      lumiera_filedescriptor_release (descriptor2, ",tmp_testfile", &node2);
      lumiera_filedescriptor_release (descriptor, ",tmp_testfile", &node);
      lumiera_backend_destroy ();
      lumiera_config_destroy ();
      return 0;
    }
  else
    return 1;
}

TEST (acquire_existing_3files)
{
  lumiera_backend_init ();
  llist node1;
  llist_init (&node1);
  LumieraFiledescriptor descriptor1 = lumiera_filedescriptor_acquire (",tmp_testfile1", LUMIERA_FILE_READONLY, &node1);

  llist node2;
  llist_init (&node2);
  LumieraFiledescriptor descriptor2 = lumiera_filedescriptor_acquire (",tmp_testfile2", LUMIERA_FILE_READONLY, &node2);

  llist node3;
  llist_init (&node3);
  LumieraFiledescriptor descriptor3 = lumiera_filedescriptor_acquire (",tmp_testfile3", LUMIERA_FILE_READONLY, &node3);
  if (descriptor1)
    lumiera_filedescriptor_release (descriptor1, ",tmp_testfile1", &node1);

  if (descriptor2)
    lumiera_filedescriptor_release (descriptor2, ",tmp_testfile2", &node2);

  if (descriptor3)
    lumiera_filedescriptor_release (descriptor3, ",tmp_testfile3", &node3);

  if (descriptor1 && descriptor2 && descriptor3)
    {
      lumiera_backend_destroy ();
      lumiera_config_destroy ();
      return 0;
    }
  else
    return 1;
}

TEST (acquire_create)
{
  lumiera_backend_init ();
  llist node;
  llist_init (&node);
  LumieraFiledescriptor descriptor = lumiera_filedescriptor_acquire (",tmp_testfile", LUMIERA_FILE_CREATE, &node);
  if (descriptor)
    {
      lumiera_filedescriptor_release (descriptor, ",tmp_testfile", &node);
      lumiera_backend_destroy ();
      lumiera_config_destroy ();
      return 0;
    }
  else
    return 1;
}

TEST (acquire_create_dir)
{
  lumiera_backend_init ();
  llist node;
  llist_init (&node);
  LumieraFiledescriptor descriptor =
    lumiera_filedescriptor_acquire (",tmp_testdir/nested/,tmp_testfile", LUMIERA_FILE_CREATE, &node);
  if (descriptor)
    {
      lumiera_filedescriptor_release (descriptor, ",tmp_testdir/nested/,tmp_testfile", &node);
      lumiera_backend_destroy ();
      lumiera_config_destroy ();
      return 0;
    }
  else
    return 1;
}


TESTS_END
