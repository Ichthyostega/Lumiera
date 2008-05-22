/*
  backend  -  common lumiera backend things

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

#include "backend/backend.h"
#include "backend/filehandlecache.h"
#include "backend/filedescriptor.h"

//NOBUG_DEFINE_FLAG_PARENT (backend, lumiera); TODO
NOBUG_DEFINE_FLAG (backend);
NOBUG_DEFINE_FLAG_PARENT (file_all, backend);

int
lumiera_backend_init (void)
{
  NOBUG_INIT_FLAG (backend);
  NOBUG_INIT_FLAG (file_all);
  NOBUG_INIT_FLAG (file);
  TRACE (backend);
  lumiera_filedescriptor_registry_init ();

  int max_entries = 10;         TODO("determine by sysconf (_SC_OPEN_MAX) minus some (big) safety margin "
                                     "add some override to run tests with few filehandles");

  lumiera_filehandlecache_new (max_entries);

  return 0;
}

void
lumiera_backend_destroy (void)
{
  TRACE (backend);
  lumiera_filedescriptor_registry_destroy ();
  lumiera_filehandlecache_delete ();
}
