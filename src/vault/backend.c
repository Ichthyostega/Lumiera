/*
  Backend  -  common lumiera vault layer facilities

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

* *****************************************************/


/** @file backend.c
 ** Lumiera Vault-Layer: implementation of global initialisation and services.
 */


#include "include/logging.h"
#include "lib/safeclib.h"
#include "lib/tmpbuf.h"
#include "lib/mpool.h"

#include "vault/backend.h"
#include "common/config.h"
#include "vault/filehandlecache.h"
#include "vault/filedescriptor.h"
#include "vault/filedescriptorregistry.h"
#include "vault/mmapcache.h"
#include "vault/threadpool.h"
#include "vault/resourcecollector.h"

#include <unistd.h>
#include <sys/resource.h>




static enum lumiera_resource_try
lumiera_backend_mpool_purge (enum lumiera_resource_try itr, void* data, void* context);

static void
lumiera_backend_resourcecollector_register_mpool (MPool self);

static void
lumiera_backend_resourcecollector_unregister_mpool (MPool self);


size_t lumiera_backend_pagesize;


int
lumiera_backend_init (void)
{

  TRACE (backend_dbg);
  lumiera_mutex_init (&lumiera_filecreate_mutex, "fileaccess", &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);

  lumiera_resourcecollector_init ();

  /* hook the resourcecollector into the mpool*/
  mpool_malloc_hook = lumiera_malloc;
  mpool_free_hook = lumiera_free;
  mpool_init_hook = lumiera_backend_resourcecollector_register_mpool;
  mpool_destroy_hook = lumiera_backend_resourcecollector_unregister_mpool;

  /* hook the resourcecollector into the safeclib allocation functions */
  lumiera_safeclib_set_resourcecollector (lumiera_resourcecollector_run);

  PLANNED("The resourcecollector aborts by default when there is no final strategy for recovery, TODO: initiate sane shutdown");

  lumiera_threadpool_init ();
  PLANNED ("hook threadpool into the resourcecollector (maybe in threadpool_init() instead here");

  lumiera_filedescriptorregistry_init ();

  lumiera_backend_pagesize = sysconf(_SC_PAGESIZE);

  /////////////////////////////////////////////////////////////////////TICKET #838 add config options to override following defaults"


  const char* filehandles = lumiera_tmpbuf_snprintf (SIZE_MAX,
                                                     "vault.file.max_handles = %d",
                                                     /* roughly 2/3 of all available filehandles are managed by the Lumiera Vault */
                                                     (sysconf (_SC_OPEN_MAX)-10)*2/3);

  lumiera_config_setdefault (filehandles);

  long long max_entries;
  lumiera_config_number_get ("vault.file.max_handles", &max_entries);
  lumiera_filehandlecache_new (max_entries);

#if SIZE_MAX <= 4294967295UL
  lumiera_config_setdefault ("vault.mmap.as_limit = 3221225469");
#else
  lumiera_config_setdefault ("vault.mmap.as_limit = 211106232532992");
#endif

  struct rlimit as_rlimit;
  getrlimit (RLIMIT_AS, &as_rlimit);

  long long as_limit = (long long)as_rlimit.rlim_cur;
  if (as_rlimit.rlim_cur == RLIM_INFINITY)
    {
      lumiera_config_number_get ("vault.mmap.as_limit", &as_limit);
    }
  else
    {
      INFO (vault, "address space limited to %luMiB", as_rlimit.rlim_cur/1024/1024);
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

  lumiera_safeclib_set_resourcecollector (NULL);

  mpool_init_hook = NULL;
  mpool_destroy_hook = NULL;
  mpool_malloc_hook = malloc;
  mpool_free_hook = free;

  lumiera_resourcecollector_destroy ();

  lumiera_mutex_destroy (&lumiera_filecreate_mutex, &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);
}


static enum lumiera_resource_try
lumiera_backend_mpool_purge (enum lumiera_resource_try itr, void* data, void* context)
{
  (void) context;
  (void) data;
  (void) itr;
  ///////////////////////////////////////////////////////////TICKET #837 actually implement mpool purging
  return LUMIERA_RESOURCE_NONE;
}

static void
lumiera_backend_resourcecollector_register_mpool (MPool self)
{
  self->udata =
     lumiera_resourcecollector_register_handler (LUMIERA_RESOURCE_MEMORY, lumiera_backend_mpool_purge, self);
}

static void
lumiera_backend_resourcecollector_unregister_mpool (MPool self)
{
  lumiera_resourcehandler_unregister ((LumieraResourcehandler) self->udata);
}



/*
//      Local Variables:
//      mode: C
//      c-file-style: "gnu"
//      indent-tabs-mode: nil
//      End:
*/
