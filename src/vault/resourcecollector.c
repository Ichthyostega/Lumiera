/*
  ResourceCollector  -  manage/collect resources when they get short

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


/** @file resourcecollector.c
 ** Implementation of a global service to manage scarce system resources
 ** @todo this was a plan from 2008 and never got beyond an initial concept stage
 */


#include "include/logging.h"
#include "lib/llist.h"
#include "lib/mutex.h"
#include "lib/safeclib.h"

#include "vault/resourcecollector.h"

#include <unistd.h>




llist lumiera_resourcecollector_registry[LUMIERA_RESOURCE_END];
lumiera_mutex lumiera_resourcecollector_lock;

struct lumiera_resourcehandler_struct
{
  llist node;
  lumiera_resource_handler_fn handler;
  void* data;
};


void
lumiera_resourcecollector_init (void)
{
  //NOBUG_INIT_FLAG (resourcecollector);
  TRACE (resourcecollector_dbg);

  for (int i = 0; i < LUMIERA_RESOURCE_END; ++i)
    llist_init (&lumiera_resourcecollector_registry[i]);

  lumiera_mutex_init (&lumiera_resourcecollector_lock, "resourcecollector", &NOBUG_FLAG(mutex_dbg), NOBUG_CONTEXT);
}



void
lumiera_resourcecollector_destroy (void)
{
  TRACE (resourcecollector_dbg);

  for (int i = 0; i < LUMIERA_RESOURCE_END; ++i)
    LLIST_WHILE_HEAD (&lumiera_resourcecollector_registry[i], head)
      lumiera_resourcehandler_unregister ((LumieraResourcehandler)head);

  lumiera_mutex_destroy (&lumiera_resourcecollector_lock, &NOBUG_FLAG(mutex_dbg), NOBUG_CONTEXT);
}


int
lumiera_resourcecollector_run (enum lumiera_resource which, enum lumiera_resource_try* iteration, void* context)
{
  TRACE (resourcecollector_dbg);

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_resourcecollector_lock)
    {
      for (enum lumiera_resource_try progress = LUMIERA_RESOURCE_NONE; progress < *iteration; ++*iteration)
        {
          if (*iteration < LUMIERA_RESOURCE_PANIC)
            {
              LLIST_FOREACH (&lumiera_resourcecollector_registry[which], node)
                {
                  LumieraResourcehandler self = (LumieraResourcehandler) node;
                  progress = self->handler (*iteration, self->data, context);

                  if (*iteration < LUMIERA_RESOURCE_ALL)
                    {
                      if (progress >= *iteration)
                        {
                          llist_insert_next (node, &lumiera_resourcecollector_registry[which]);
                          break;
                        }
                    }
                }
            }
          else
            {
              ALERT (resourcecollector, "PANIC, Not enough resources %d", which);
              for (int i = 0; i < LUMIERA_RESOURCE_END; ++i)
                LLIST_FOREACH (&lumiera_resourcecollector_registry[i], node)
                  {
                    LumieraResourcehandler self = (LumieraResourcehandler) node;
                    progress = self->handler (LUMIERA_RESOURCE_PANIC, self->data, NULL);
                  }
              _exit (EXIT_FAILURE);
            }
        }
    }

  return 1;
}



LumieraResourcehandler
lumiera_resourcecollector_register_handler (enum lumiera_resource resource, lumiera_resource_handler_fn handler, void* data)
{
  TRACE (resourcecollector_dbg);

  LumieraResourcehandler self = lumiera_malloc (sizeof (*self));

  llist_init (&self->node);
  self->handler = handler;
  self->data = data;

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_resourcecollector_lock)
    {
      llist_insert_tail (&lumiera_resourcecollector_registry[resource], &self->node);
    }

  return self;
}


void
lumiera_resourcehandler_unregister (LumieraResourcehandler self)
{
  TRACE (resourcecollector_dbg);

  if (self)
    {
      LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_resourcecollector_lock)
        {
          llist_unlink (&self->node);
          self->handler (LUMIERA_RESOURCE_UNREGISTER, self->data, NULL);
        }

      lumiera_free (self);
    }
}


LumieraResourcehandler
lumiera_resourcecollector_handler_find (enum lumiera_resource resource, lumiera_resource_handler_fn handler, void* data)
{
  TRACE (resourcecollector_dbg);
  LumieraResourcehandler self = NULL;

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_resourcecollector_lock)
    {
      LLIST_FOREACH (&lumiera_resourcecollector_registry[resource], node)
        {
          self = (LumieraResourcehandler) node;

          if (((LumieraResourcehandler)node)->handler == handler && ((LumieraResourcehandler)node)->data == data)
            {
              self = (LumieraResourcehandler)node;
              break;
            }
        }
    }

  return self;
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
