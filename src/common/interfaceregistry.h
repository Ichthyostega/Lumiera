/*
  INTERFACEREGISTRY.h  -  Lumiera interface registry

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file interfaceregistry.h
 ** Global registry for interfaces (extension points).
 ** Interface instances are published and activated by registering them
 ** into a global registry, which is defined here. This instances are identified
 ** by their name and major version.
 */

#ifndef LUMIERA_INTERFACEREGISTRY_H
#define LUMIERA_INTERFACEREGISTRY_H

#include "lib/mutex.h"
#include "lib/recmutex.h"
#include "lib/psplay.h"

#include "common/interface.h"
#include "common/plugin.h"

#include <nobug.h>


extern PSplay lumiera_interfaceregistry;
extern lumiera_recmutex lumiera_interface_mutex;


/**
 * Interface management node.
 * All active interfaces managed through this node which contains the dynamic data for
 * dependency tracking and reference counting.
 */
typedef struct lumiera_interfacenode_struct lumiera_interfacenode;
typedef lumiera_interfacenode* LumieraInterfacenode;

struct lumiera_interfacenode_struct
{
  /** all known interfaces are registered in a tree */
  psplaynode node;

  /** the interface itself */
  LumieraInterface interface;

  /** reference counters and link used for internal reference management */
  unsigned refcnt;

  /** backreference to its plugin if it comes from a plugin, else NULL */
  LumieraPlugin plugin;

  /** temporary used to stack interfaces when recursively opening/closing them */
  LumieraInterfacenode lnk;

  /** allocated size of the following dependency table */
  size_t deps_size;

  /** NULL terminated table of all dependencies (interfaces opened on initialisation) */
  LumieraInterfacenode* deps;
};


/**
 * Initialise the interface registry
 */
void
lumiera_interfaceregistry_init (void);

void
lumiera_interfaceregistry_destroy (void);


void
lumiera_interfaceregistry_register_interface (LumieraInterface self, LumieraPlugin plugin);

void
lumiera_interfaceregistry_bulkregister_interfaces (LumieraInterface* self, LumieraPlugin plugin);

void
lumiera_interfaceregistry_remove_interface (LumieraInterface self);

void
lumiera_interfaceregistry_bulkremove_interfaces (LumieraInterface* self);

LumieraInterfacenode
lumiera_interfaceregistry_interfacenode_find (const char* interface, unsigned version, const char* name);

LumieraInterface
lumiera_interfaceregistry_interface_find (const char* interface, unsigned version, const char* name);




#endif /* LUMIERA_INTERFACEREGISTRY_H */
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
