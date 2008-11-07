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
#include "backend/mmapcache.h"

#include <unistd.h>
#include <sys/resource.h>

//NOBUG_DEFINE_FLAG_PARENT (backend, lumiera); TODO
NOBUG_DEFINE_FLAG (backend);
NOBUG_DEFINE_FLAG_PARENT (file_all, backend);

NOBUG_DECLARE_FLAG (file);
NOBUG_DECLARE_FLAG (mmap_all);
NOBUG_DECLARE_FLAG (mmap);
NOBUG_DECLARE_FLAG (mmapings);


size_t lumiera_backend_pagesize;

int
lumiera_backend_init (void)
{
  NOBUG_INIT_FLAG (backend);
  NOBUG_INIT_FLAG (file_all);
  NOBUG_INIT_FLAG (file);
  NOBUG_INIT_FLAG (mmap_all);
  NOBUG_INIT_FLAG (mmap);
  NOBUG_INIT_FLAG (mmapings);

  TRACE (backend);
  lumiera_filedescriptor_registry_init ();

  lumiera_backend_pagesize = sysconf(_SC_PAGESIZE);

  TODO ("add config options to override following defaults");

  /* roughly 2/3 of all availables filehandles are managed by the backend */
  int max_entries = (sysconf (_SC_OPEN_MAX)-10)*2/3;
  INFO (backend, "using %d filehandles", max_entries);
  lumiera_filehandlecache_new (max_entries);

  struct rlimit as_limit;
  getrlimit (RLIMIT_AS, &as_limit);

  if (as_limit.rlim_cur == RLIM_INFINITY)
    {
#if SIZE_MAX <= 4294967295UL
      INFO (backend, "address space not limited, backend will mmap at most 1.5GiB");
      as_limit.rlim_cur = 1610612736U;
#else
      INFO (backend, "address space not limited, backend will mmap at most 192TiB");
      as_limit.rlim_cur = 211106232532992U;
#endif
    }
  else
    {
      INFO (backend, "address space limited to %luMiB", as_limit.rlim_cur/1024/1024);
    }

  lumiera_mmapcache_new (as_limit.rlim_cur);

  return 0;
}

void
lumiera_backend_destroy (void)
{
  TRACE (backend);
  lumiera_mmapcache_delete ();
  lumiera_filehandlecache_delete ();
  lumiera_filedescriptor_registry_destroy ();
}
