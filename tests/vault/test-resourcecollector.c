/*
  TEST-RESOURCECOLLECTOR  -  test the resource collector

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

/** @file test-resourcecollector.c
 ** C unit test to cover management of low-level resources for the Vault
 ** @see resourcecollector.h
 */


#include "vault/resourcecollector.h"

#include "lib/test/test.h"

#include <unistd.h>
#include <errno.h>

static enum lumiera_resource_try
test_memory_handler (enum lumiera_resource_try itr, void* data, void* context)
{
  switch (itr)
    {
    case LUMIERA_RESOURCE_UNREGISTER:
      printf ("unregistering memory handler\n");
      break;

    default:
      printf ("memory handler got called\n");
      return LUMIERA_RESOURCE_ALL;
    }

  return LUMIERA_RESOURCE_NONE;
}




TESTS_BEGIN

TEST (basic)
{
  lumiera_resourcecollector_init ();
  lumiera_resourcecollector_register_handler (LUMIERA_RESOURCE_MEMORY, test_memory_handler, NULL);
  lumiera_resourcecollector_destroy ();
}

TEST (memory_collection_mockup)
{
  CHECK (argv[2]);
  lumiera_resourcecollector_init ();

  lumiera_resourcecollector_register_handler (LUMIERA_RESOURCE_MEMORY, test_memory_handler, NULL);

  size_t size = 1000;
  enum lumiera_resource_try iteration = LUMIERA_RESOURCE_ONE;
  int trying = atoi (argv[2]);
  do {
    --trying;
  } while (trying && lumiera_resourcecollector_run (LUMIERA_RESOURCE_MEMORY, &iteration, &size));

  lumiera_resourcecollector_destroy ();
}


TESTS_END
