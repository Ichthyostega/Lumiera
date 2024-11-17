/*
  InterfaceRegistry  -  registry for extension points

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file interfaceregistry.c
 ** Interface instances are published and activated by registering them
 ** into a global registry, which is defined here. This instances are identified
 ** by their name and major version.
 */

#include "include/logging.h"
#include "lib/error.h"
#include "lib/psplay.h"
#include "lib/safeclib.h"

#include <nobug.h>

#include "common/plugin.h"
#include "common/interfaceregistry.h"



PSplay lumiera_interfaceregistry;
PSplay lumiera_pluginregistry;
lumiera_recmutex lumiera_interface_mutex;

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


/** Initialise the interface registry */
void
lumiera_interfaceregistry_init (void)
{

  TRACE (interfaceregistry_dbg);
  REQUIRE (!lumiera_interfaceregistry);
  REQUIRE (!lumiera_pluginregistry);

  lumiera_interfaceregistry = psplay_new (lumiera_interface_cmp_fn, lumiera_interface_key_fn, NULL);
  if (!lumiera_interfaceregistry)
    LUMIERA_DIE (ERRNO);

  lumiera_pluginregistry = psplay_new (lumiera_plugin_cmp_fn, lumiera_plugin_key_fn, lumiera_plugin_delete_fn);
  if (!lumiera_pluginregistry)
    LUMIERA_DIE (ERRNO);

  lumiera_recmutex_init (&lumiera_interface_mutex, "interfaceregistry", &NOBUG_FLAG(interfaceregistry), NOBUG_CONTEXT);

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

  lumiera_recmutex_destroy (&lumiera_interface_mutex, &NOBUG_FLAG(mutex_dbg), NOBUG_CONTEXT);

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
          if (node)
            {
              REQUIRE (node->refcnt == 0, "but is %d", node->refcnt);
              lumiera_interfacenode_delete ((LumieraInterfacenode) psplay_remove (lumiera_interfaceregistry, &node->node));
            }
          else
            {
              ///////////TICKET #864 : should not happen but does happen in practice, e.g. when there is a copy or another library linked against that module
              ///////////              Guess the pluginloader should not have added the duplicate into the interfaceregistry on discovery.
              
              WARN (interfaceregistry, "ENTRY NOT FOUND in interfaceregistry at clean-up of interface %s, instance %s"
                                     , (*self)->interface
                                     , (*self)->name);
            }

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
