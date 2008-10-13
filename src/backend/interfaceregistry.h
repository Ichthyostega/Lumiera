/*
  interfaceregistry.h  -  Lumiera interface registry

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
#ifndef LUMIERA_INTERFACEREGISTRY_H
#define LUMIERA_INTERFACEREGISTRY_H

#include "lib/mutex.h"
#include "lib/interface.h"

#include <nobug.h>


/**
 * @file
 * Interface instances are published and activated by registering them
 * into a gloabl registry, which is defined here. This instances are identified
 * by their name and major version.
 */

NOBUG_DECLARE_FLAG (interface_all);
NOBUG_DECLARE_FLAG (interfaceregistry);
NOBUG_DECLARE_FLAG (interface);

extern lumiera_mutex lumiera_interface_mutex;


/**
 * Initialize the interface registry
 */
void
lumiera_interfaceregistry_init (void);

void
lumiera_interfaceregistry_destroy (void);


void
lumiera_interfaceregistry_register_interface (LumieraInterface self);

void
lumiera_interfaceregistry_bulkregister_interfaces (LumieraInterface* self);

void
lumiera_interfaceregistry_remove_interface (LumieraInterface self);

void
lumiera_interfaceregistry_bulkremove_interfaces (LumieraInterface* self);

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
