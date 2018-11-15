/*
  BACKEND.h  -  common lumiera vault definitions

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

*/


/** @file backend.h
 ** Lumiera Vault-Layer: global initialisation and definitions.
 */


#ifndef BACKEND_BACKEND_H
#define BACKEND_BACKEND_H

#include "lib/mutex.h"

#include <nobug.h>



extern size_t lumiera_backend_pagesize;

/**
 * Protect lookup and creation of files.
 * Trying to access a nonexistent file with O_CREAT would be racy.
 * Defined in filedescriptor.c
 */
extern lumiera_mutex lumiera_filecreate_mutex;


int
lumiera_backend_init (void);

void
lumiera_backend_destroy (void);

#endif /*BACKEND_BACKEND_H*/
