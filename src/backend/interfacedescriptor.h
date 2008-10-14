/*
  interfacedescriptor.h  -  Metadata interface for Lumiera interfaces

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
#ifndef LUMIERA_INTERFACEDESCRIPTOR_H
#define LUMIERA_INTERFACEDESCRIPTOR_H

#include "backend/interface.h"

/**
 * WIP: interface descriptor, needs some generic metadata interface
 */
LUMIERA_INTERFACE_DECLARE (lumieraorg_interfacedescriptor, 0,
                           /* The following slots are some human-readable descriptions of certain properties */
                           LUMIERA_INTERFACE_SLOT (const char*, name, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, version, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, author, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, copyright, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, license, (LumieraInterface))
                           /* TODO add more things here, dependencies, provisions etc */
                           );



#endif /* LUMIERA_INTERFACEDESCRIPTORS_H */
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
