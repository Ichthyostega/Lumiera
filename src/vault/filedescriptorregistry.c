/*
  FiledescriptorRegistry  -  registry for tracking all files in use

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

* *****************************************************/


/** @file filedescriptorregistry.c
 ** Implementation of a registry to manage used filedescriptors
 ** @todo development in this area is stalled since 2010
 */


#include "include/logging.h"
#include "lib/safeclib.h"
#include "lib/mutex.h"
#include "lib/psplay.h"

#include "vault/file.h"
#include "vault/filedescriptor.h"
#include "vault/filedescriptorregistry.h"


/**
 * the global registry for file descriptors.
 */
static PSplay filedescriptorregistry = NULL;
static lumiera_mutex filedescriptorregistry_mutex;


static int
cmp_fn (const void* keya, const void* keyb)
{
  const LumieraFiledescriptor a = (const LumieraFiledescriptor)keya;
  const LumieraFiledescriptor b = (const LumieraFiledescriptor)keyb;

  if (a->stat.st_dev < b->stat.st_dev)
    return -1;
  else if (a->stat.st_dev > b->stat.st_dev)
    return 1;

  if (a->stat.st_ino < b->stat.st_ino)
    return -1;
  else if (a->stat.st_ino > b->stat.st_ino)
    return 1;

  if ((a->flags&LUMIERA_FILE_MASK) < (b->flags&LUMIERA_FILE_MASK))
    return -1;
  else if ((a->flags&LUMIERA_FILE_MASK) > (b->flags&LUMIERA_FILE_MASK))
    return 1;

  return 0;
}


static void
delete_fn (PSplaynode node)
{
  TODO ("figure name out? or is the handle here always closed");
  lumiera_filedescriptor_delete ((LumieraFiledescriptor) node, NULL);
}


static const void*
key_fn (const PSplaynode node)
{
  return node;
}



void
lumiera_filedescriptorregistry_init (void)
{
  TRACE (filedescriptor_dbg);
  REQUIRE (!filedescriptorregistry);

  filedescriptorregistry = psplay_new (cmp_fn, key_fn, delete_fn);
  if (!filedescriptorregistry)
    LUMIERA_DIE (NO_MEMORY);

  lumiera_mutex_init (&filedescriptorregistry_mutex, "filedescriptorregistry", &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);
}

void
lumiera_filedescriptorregistry_destroy (void)
{
  TRACE (filedescriptor_dbg);
  REQUIRE (!psplay_nelements (filedescriptorregistry));

  lumiera_mutex_destroy (&filedescriptorregistry_mutex, &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);

  if (filedescriptorregistry)
    psplay_delete (filedescriptorregistry);

  filedescriptorregistry = NULL;
}


LumieraFiledescriptor
lumiera_filedescriptorregistry_ensure (LumieraFiledescriptor template)
{
  REQUIRE (filedescriptorregistry, "not initialised");

  LumieraFiledescriptor ret = NULL;

  LUMIERA_MUTEX_SECTION (mutex_sync, &filedescriptorregistry_mutex)
    {
      /* lookup/create descriptor */
      ret = (LumieraFiledescriptor) psplay_find (filedescriptorregistry, template, 100);

      if (!ret)
        {
          ret = lumiera_filedescriptor_new (template);
          if (!ret)
            goto error;

          psplay_insert (filedescriptorregistry, &ret->node, 100);
        }

    error:
      ;
    }

  return ret;
}


void
lumiera_filedescriptorregistry_remove (LumieraFiledescriptor self)
{
  LUMIERA_MUTEX_SECTION (mutex_sync, &filedescriptorregistry_mutex)
    psplay_remove (filedescriptorregistry, &self->node);
}


