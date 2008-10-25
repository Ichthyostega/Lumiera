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
 * Release state of an interface implementation.
 * The interfase subsystem must be able to categorize implementation to present possible
 * upgrade paths to the user. This is done by the tagging it to a certain state in concert
 * with the version and the user supplied version compare function. The respective numbers
 * are choosen in a way that a higher value indicates precedence when selecting an implementation.
 * Note that 'BETA' is higher than 'DEPRECATED' (we make the assumption that BETA is at least
 * maintained code and something gets deprecated for some reason), for common practice it is still
 * suggested to make a stable release before declaring its predcessor version as deprecated.
 */
enum lumiera_interface_state {
  /** some known bugs exist which won't be fixed, don't use this */
  LUMIERA_INTERFACE_BROKEN = -1,
  /** Not finished development code */
  LUMIERA_INTERFACE_EXPERIMENTAL = 0,
  /** Old implementation which is now unmaintained and will be removed soon */
  LUMIERA_INTERFACE_DEPRECATED = 1,
  /** Finished but not finally released implementation for open testing */
  LUMIERA_INTERFACE_BETA = 2,
  /** Finished, released and maintained implementation */
  LUMIERA_INTERFACE_STABLE = 3
};


/**
 * Interface descriptor.
 * This defines an interface for querying metadata common to all interface implementations
 */
LUMIERA_INTERFACE_DECLARE (lumieraorg_interfacedescriptor, 0,
                           /* The following slots are some human-readable descriptions of certain properties */
                           LUMIERA_INTERFACE_SLOT (const char*, name, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, brief, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, homepage, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, version, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, author, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, email, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, copyright, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, license, (LumieraInterface)),

                           /* some flags for properties */
                           LUMIERA_INTERFACE_SLOT (int, state, (LumieraInterface)),

                           /* compare 2 version strings in a custom way */
                           LUMIERA_INTERFACE_SLOT (int, versioncmp, (const char*, const char*))
                           );



#endif /* LUMIERA_INTERFACEDESCRIPTORS_H */
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
