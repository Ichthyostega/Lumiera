/*
  interfaceregistry.c  -  Lumiera interface registry

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
#include "lib/error.h"
#include "lib/psplay.h"


#include <nobug.h>



#include "backend/interfaceregistry.h"

/**
 * @file
 * Interface instances are published and activated by registering them
 * into a global registry, which is defined here. This instances are identified
 * by their name and major version.
 */

NOBUG_DEFINE_FLAG_PARENT (interfaceregistry, backend);

static PSplay interfaceregistry;
lumiera_mutex lumiera_interface_mutex;


static int
cmp_fn (const void* keya, const void* keyb);

static const void*
key_fn (const PSplaynode node);



/**
 * Initialize the interface registry
 */
void
lumiera_interfaceregistry_init (void)
{
  NOBUG_INIT_FLAG (interfaceregistry);
  TRACE (interfaceregistry);
  REQUIRE (!interfaceregistry);

  interfaceregistry = psplay_new (cmp_fn, key_fn, NULL);
  if (!interfaceregistry)
    LUMIERA_DIE (ERRNO);

  lumiera_recmutex_init (&lumiera_interface_mutex, "interfaceregistry", &NOBUG_FLAG(interfaceregistry));
}

void
lumiera_interfaceregistry_destroy (void)
{
  TRACE (interfaceregistry);
  REQUIRE (!psplay_nelements (interfaceregistry));

  lumiera_mutex_destroy (&lumiera_interface_mutex, &NOBUG_FLAG(interfaceregistry));

  if (interfaceregistry)
    psplay_destroy (interfaceregistry);
  interfaceregistry = NULL;
}


void
lumiera_interfaceregistry_register_interface (LumieraInterface self)
{
  TRACE (interfaceregistry);
  REQUIRE (self);

  LUMIERA_RECMUTEX_SECTION (interfaceregistry, &lumiera_interface_mutex)
    {
      TRACE (interfaceregistry, "interface %s, version %d, instance %s", self->interface, self->version, self->name);
      psplay_insert (interfaceregistry, &self->node, 100);
    }
}


void
lumiera_interfaceregistry_bulkregister_interfaces (LumieraInterface* self)
{
  TRACE (interfaceregistry);
  REQUIRE (self);

  LUMIERA_RECMUTEX_SECTION (interfaceregistry, &lumiera_interface_mutex)
    {
      while (*self)
        {
          psplay_insert (interfaceregistry, &(*self)->node, 100);
          ++self;
        }
    }
}


void
lumiera_interfaceregistry_remove_interface (LumieraInterface self)
{
  TRACE (interfaceregistry);
  REQUIRE (self);

  LUMIERA_RECMUTEX_SECTION (interfaceregistry, &lumiera_interface_mutex)
    {
      psplay_remove (interfaceregistry, &self->node);
    }
}


void
lumiera_interfaceregistry_bulkremove_interfaces (LumieraInterface* self)
{
  TRACE (interfaceregistry);
  REQUIRE (self);

  LUMIERA_RECMUTEX_SECTION (interfaceregistry, &lumiera_interface_mutex)
    {
      while (*self)
        {
          psplay_remove (interfaceregistry, &(*self)->node);
          ++self;
        }
    }
}


LumieraInterface
lumiera_interfaceregistry_interface_find (const char* interface, unsigned version, const char* name)
{
  TRACE (interfaceregistry);
  struct lumiera_interface_struct cmp;
  cmp.interface = interface;
  cmp.version = version;
  cmp.name = name;

  LumieraInterface ret = NULL;

  LUMIERA_RECMUTEX_SECTION (interfaceregistry, &lumiera_interface_mutex)
    {
      ret = (LumieraInterface)psplay_find (interfaceregistry, &cmp, 100);
    }

  return ret;
}


static int
cmp_fn (const void* keya, const void* keyb)
{
  const LumieraInterface a = (const LumieraInterface)keya;
  const LumieraInterface b = (const LumieraInterface)keyb;

  int r = strcmp (a->interface, b->interface);
  if (r<0)
    return -1;
  else if (r>0)
    return 1;

  if (a->version < b->version)
    return -1;
  else if (a->version > b->version)
    return 1;

  r = strcmp (a->name, b->name);
  if (r<0)
    return -1;
  else if (r>0)
    return 1;

  return 0;
}


static const void*
key_fn (const PSplaynode node)
{
  return node;
}


/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
