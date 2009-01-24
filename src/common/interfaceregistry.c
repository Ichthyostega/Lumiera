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

#include "include/logging.h"
#include "lib/mutex.h"
#include "lib/error.h"
#include "lib/psplay.h"
#include "lib/safeclib.h"


#include <nobug.h>



#include "common/plugin.h"
#include "common/interfaceregistry.h"

/**
 * @file
 * Interface instances are published and activated by registering them
 * into a global registry, which is defined here. This instances are identified
 * by their name and major version.
 */

//NOBUG_DEFINE_FLAG_PARENT (interface_all, lumiera_all);
//NOBUG_DEFINE_FLAG_PARENT (plugin, interface_all);
//NOBUG_DEFINE_FLAG_PARENT (interfaceregistry, interface_all);
//NOBUG_DEFINE_FLAG_PARENT (interface, interface_all);

PSplay lumiera_interfaceregistry;
PSplay lumiera_pluginregistry;
lumiera_mutex lumiera_interface_mutex;

static int
lumiera_interface_cmp_fn (const void* keya, const void* keyb);

static const void*
lumiera_interface_key_fn (const PSplaynode node);



static LumieraInterfacenode
lumiera_interfacenode_new (LumieraInterface iface, LumieraPlugin plugin)
{
  LumieraInterfacenode self = lumiera_malloc (sizeof (*self));

  psplaynode_init (&self->node);
  self->interface = iface;
  self->refcnt = 0;
  self->plugin = plugin;
  self->lnk = NULL;
  self->deps_size = 0;
  self->deps = NULL;

  return self;
}


static void
lumiera_interfacenode_delete (LumieraInterfacenode self)
{
  if (self)
    {
      REQUIRE (self->refcnt == 0);
      lumiera_free (self->deps);
      lumiera_free (self);
    }
}


/**
 * Initialize the interface registry
 */
void
lumiera_interfaceregistry_init (void)
{
  //NOBUG_INIT_FLAG (interface_all);
  //NOBUG_INIT_FLAG (interfaceregistry);
  //NOBUG_INIT_FLAG (interface);
  //NOBUG_INIT_FLAG (plugin);

  TRACE (interfaceregistry_dbg);
  REQUIRE (!lumiera_interfaceregistry);
  REQUIRE (!lumiera_pluginregistry);

  lumiera_interfaceregistry = psplay_new (lumiera_interface_cmp_fn, lumiera_interface_key_fn, NULL);
  if (!lumiera_interfaceregistry)
    LUMIERA_DIE (ERRNO);

  lumiera_pluginregistry = psplay_new (lumiera_plugin_cmp_fn, lumiera_plugin_key_fn, lumiera_plugin_delete_fn);
  if (!lumiera_pluginregistry)
    LUMIERA_DIE (ERRNO);

  lumiera_recmutex_init (&lumiera_interface_mutex, "interfaceregistry", &NOBUG_FLAG(interfaceregistry));

  lumiera_interface_init ();
}


void
lumiera_interfaceregistry_destroy (void)
{
  TRACE (interfaceregistry_dbg);

  lumiera_interface_destroy ();

  if (lumiera_pluginregistry)
    psplay_delete (lumiera_pluginregistry);
  lumiera_pluginregistry = NULL;

  lumiera_mutex_destroy (&lumiera_interface_mutex, &NOBUG_FLAG(mutex_dbg));

  REQUIRE (!psplay_nelements (lumiera_interfaceregistry), "some interfaces still registered at shutdown");

  if (lumiera_interfaceregistry)
    psplay_destroy (lumiera_interfaceregistry);
  lumiera_interfaceregistry = NULL;
}


void
lumiera_interfaceregistry_register_interface (LumieraInterface self, LumieraPlugin plugin)
{
  TRACE (interfaceregistry_dbg);
  REQUIRE (self);

  LUMIERA_RECMUTEX_SECTION (mutex_sync, &lumiera_interface_mutex)
    {
      TRACE (interfaceregistry, "interface %s, version %d, instance %s", self->interface, self->version, self->name);
      psplay_insert (lumiera_interfaceregistry, &lumiera_interfacenode_new (self, plugin)->node, 100);
    }
}


void
lumiera_interfaceregistry_bulkregister_interfaces (LumieraInterface* self, LumieraPlugin plugin)
{
  TRACE (interfaceregistry_dbg);
  REQUIRE (self);

  LUMIERA_RECMUTEX_SECTION (mutex_sync, &lumiera_interface_mutex)
    {
      while (*self)
        {
          TRACE (interfaceregistry, "interface %s, version %d, instance %s", (*self)->interface, (*self)->version, (*self)->name);
          psplay_insert (lumiera_interfaceregistry, &lumiera_interfacenode_new (*self, plugin)->node, 100);
          ++self;
        }
    }
}


void
lumiera_interfaceregistry_remove_interface (LumieraInterface self)
{
  TRACE (interfaceregistry_dbg);
  REQUIRE (self);

  LUMIERA_RECMUTEX_SECTION (mutex_sync, &lumiera_interface_mutex)
    {
      TRACE (interfaceregistry, "interface %s, version %d, instance %s", self->interface, self->version, self->name);
      LumieraInterfacenode node = (LumieraInterfacenode) psplay_find (lumiera_interfaceregistry, self, 0);
      REQUIRE (node->refcnt == 0, "but is %d", node->refcnt);

      lumiera_interfacenode_delete ((LumieraInterfacenode)psplay_remove (lumiera_interfaceregistry, &node->node));
    }
}


void
lumiera_interfaceregistry_bulkremove_interfaces (LumieraInterface* self)
{
  TRACE (interfaceregistry_dbg);
  REQUIRE (self);

  LUMIERA_RECMUTEX_SECTION (mutex_sync, &lumiera_interface_mutex)
    {
      while (*self)
        {
          TRACE (interfaceregistry, "interface %s, version %d, instance %s", (*self)->interface, (*self)->version, (*self)->name);

          LumieraInterfacenode node = (LumieraInterfacenode) psplay_find (lumiera_interfaceregistry, *self, 0);
          REQUIRE (node->refcnt == 0, "but is %d", node->refcnt);

          lumiera_interfacenode_delete ((LumieraInterfacenode) psplay_remove (lumiera_interfaceregistry, &node->node));

          ++self;
        }
    }
}


LumieraInterfacenode
lumiera_interfaceregistry_interfacenode_find (const char* interface, unsigned version, const char* name)
{
  TRACE (interfaceregistry_dbg);
  struct lumiera_interface_struct cmp;
  cmp.interface = interface;
  cmp.version = version;
  cmp.name = name;

  LumieraInterfacenode ret = NULL;

  LUMIERA_RECMUTEX_SECTION (mutex_sync, &lumiera_interface_mutex)
    {
      ret = (LumieraInterfacenode)psplay_find (lumiera_interfaceregistry, &cmp, 100);
    }

  return ret;
}


LumieraInterface
lumiera_interfaceregistry_interface_find (const char* interface, unsigned version, const char* name)
{
  return lumiera_interfaceregistry_interfacenode_find (interface, version, name)->interface;
}


static int
lumiera_interface_cmp_fn (const void* keya, const void* keyb)
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
lumiera_interface_key_fn (const PSplaynode node)
{
  return ((LumieraInterfacenode)node)->interface;
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
