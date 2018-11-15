/*
  FILEDESCRIPTORREGISTRY.h  -  registry for tracking all files in use

  Copyright (C)                 Lumiera.org
    2008, 2010,                 Christian Thaeter <ct@pipapo.org>

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

*/


/** @file filedescriptorregistry.h
 ** Registry for used file descriptors.
 ** This registry stores all acquired file descriptors for lookup,
 ** they will be freed when not referenced anymore.
 */

#ifndef BACKEND_FILEDESCRIPTORREGISTRY_H
#define BACKEND_FILEDESCRIPTORREGISTRY_H

#include "vault/filedescriptor.h"


/**
 * Initialise the global file descriptor registry.
 * Opening hard linked files will be targeted to the same file descriptor.
 * This function never fails but dies on error.
 * @todo proper backend/subsystem failure
 */
void
lumiera_filedescriptorregistry_init (void);

/**
 * Destroy and free the global file descriptor registry.
 * Never fails.
 */
void
lumiera_filedescriptorregistry_destroy (void);

/**
 * Ensures that a filedescriptor is in the registry.
 * Looks template up and if not found, create a new one from template.
 * @return filedescriptor from registry
 */
LumieraFiledescriptor
lumiera_filedescriptorregistry_ensure (LumieraFiledescriptor template);


/** Removes a file descriptor from the registry. */
void
lumiera_filedescriptorregistry_remove (LumieraFiledescriptor self);



#endif /*BACKEND_FILEDESCRIPTORREGISTRY_H*/
