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

#include "include/logging.h"
#include "lib/safeclib.h"

#include "backend/backend.h"
#include "common/config.h"
#include "backend/filehandlecache.h"
#include "backend/filedescriptor.h"
#include "backend/filedescriptorregistry.h"
#include "backend/mmapcache.h"
#include "backend/threadpool.h"

#include <unistd.h>
#include <sys/resource.h>

//NOBUG_DEFINE_FLAG_PARENT (backend, lumiera); TODO
//NOBUG_DEFINE_FLAG (backend);
//NOBUG_DEFINE_FLAG_PARENT (file_all, backend);
//NOBUG_DEFINE_FLAG_PARENT (filehandle, file_all);

//NOBUG_DEFINE_FLAG_PARENT (mmapings, mmap_all);



//NOBUG_DECLARE_FLAG (file);

//NOBUG_DECLARE_FLAG (mmap_all);
//NOBUG_DECLARE_FLAG (mmap);
//NOBUG_DECLARE_FLAG (mmapings);
//NOBUG_DECLARE_FLAG (mmapcache);


size_t lumiera_backend_pagesize;

int
lumiera_backend_init (void)
{
  //NOBUG_INIT_FLAG (backend);
  //NOBUG_INIT_FLAG (file_all);
  //NOBUG_INIT_FLAG (file);
  //NOBUG_INIT_FLAG (filehandle);
  //NOBUG_INIT_FLAG (mmap_all);
  //NOBUG_INIT_FLAG (mmap);
  //NOBUG_INIT_FLAG (mmapings);
  //NOBUG_INIT_FLAG (mmapcache);

  TRACE (backend_dbg);

  lumiera_mutex_init (&lumiera_filecreate_mutex, "fileaccess", &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);

  lumiera_threadpool_init ();

  lumiera_filedescriptorregistry_init ();

  lumiera_backend_pagesize = sysconf(_SC_PAGESIZE);

  TODO ("add config options to override following defaults");


  const char* filehandles = lumiera_tmpbuf_snprintf (SIZE_MAX,
                                                     "backend.file.max_handles = %d",
                                                     /* roughly 2/3 of all availables filehandles are managed by the backend */
                                                     (sysconf (_SC_OPEN_MAX)-10)*2/3);

  lumiera_config_setdefault (filehandles);

  long long max_entries;
  lumiera_config_number_get ("backend.file.max_handles", &max_entries);
  lumiera_filehandlecache_new (max_entries);

#if SIZE_MAX <= 4294967295UL
  lumiera_config_setdefault ("backend.mmap.as_limit = 3221225469");
#else
  lumiera_config_setdefault ("backend.mmap.as_limit = 211106232532992");
#endif

  struct rlimit as_rlimit;
  getrlimit (RLIMIT_AS, &as_rlimit);

  long long as_limit = (long long)as_rlimit.rlim_cur;
  if (as_rlimit.rlim_cur == RLIM_INFINITY)
    {
      lumiera_config_number_get ("backend.mmap.as_limit", &as_limit);
    }
  else
    {
      INFO (backend, "address space limited to %luMiB", as_rlimit.rlim_cur/1024/1024);
    }

  lumiera_mmapcache_new (as_limit);

  return 0;
}

void
lumiera_backend_destroy (void)
{
  TRACE (backend_dbg);
  lumiera_mmapcache_delete ();
  lumiera_filehandlecache_delete ();
  lumiera_filedescriptorregistry_destroy ();
  lumiera_threadpool_destroy ();

  lumiera_mutex_destroy (&lumiera_filecreate_mutex, &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);
}

