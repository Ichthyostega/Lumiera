/*
  RESOURCECOLLECTOR.h  -  manage/collect resources when they get short

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


/** @file resourcecollector.h
 ** A global manager for scarce system resources
 ** @todo this was a plan from 2008 and never got beyond an initial concept stage
 */


#ifndef BACKEND_RESOURCECOLLECTOR_H
#define BACKEND_RESOURCECOLLECTOR_H

#include <nobug.h>



/**
 * Resources known to the resource collector
 */
enum lumiera_resource
  {
    /** memory blocks, context is a pointer to the size_t required **/
    LUMIERA_RESOURCE_MEMORY,
    /** OS filehandles **/
    LUMIERA_RESOURCE_FILEHANDLE,
    /** CPU time, as in threads and such  **/
    LUMIERA_RESOURCE_CPU,
    /** mmaped regions **/
    LUMIERA_RESOURCE_MMAP,
    /** disk space for the storage area, context is a pointer to the filename indication the device **/
    LUMIERA_RESOURCE_DISKSTORAGE,
    /** disk bandwidth for the storage area, context is a pointer to the filename indication the device **/
    LUMIERA_RESOURCE_STORAGEBANDWIDTH,
    /** disk space for the caching area, context is a pointer to the filename indication the device **/
    LUMIERA_RESOURCE_DISKCACHE,
    /** disk bandwidth for the caching area, context is a pointer to the filename indication the device **/
    LUMIERA_RESOURCE_CACHEBANDWIDTH,

    LUMIERA_RESOURCE_END /* last entry */
  };


/**
 * Iteration indicator
 * Resource collection works iteratively freeing more and more resources.
 * Handlers do not need to obey the request and shall return LUMIERA_RESOURCE_NONE
 * which will then continue with the next handler.
 * This goes through all available handlers until one returns a higher or same value
 * than the current iteration to indicate that it freed enough resources to continue the task.
 * Then control is passed back to the calling loop which retries the resource allocation.
 * LUMIERA_RESOURCE_PANIC is somewhat special since it will always call all registered handlers
 * for all resources, not only the queried one and finally _exit() the application.
 * The exact amounts of resources to be freed for ONE, SOME and MANY in intentionally
 * kept vague the handlers are free to interpret this in some sensible way.
 */
enum lumiera_resource_try
  {
    /** No op, returned by a handler when it did nothing **/
    LUMIERA_RESOURCE_NONE,
    /** try to free one or really few of this resources **/
    LUMIERA_RESOURCE_ONE,
    /** try to free a small reasonable implementation defined amount of resources **/
    LUMIERA_RESOURCE_SOME,
    /** try to free a bigger implementation defined amount of resources **/
    LUMIERA_RESOURCE_MANY,
    /** free as much as possible **/
    LUMIERA_RESOURCE_ALL,
    /** die! **/
    LUMIERA_RESOURCE_PANIC,
    /** When a handler gets unregistered it will be called with this value to give it a chance to clean up the user 'data' **/
    LUMIERA_RESOURCE_UNREGISTER
  };


/**
 * The type for the resource collector handler functions.
 * Handlers are always run with a global resourcecollector mutex locked, the user does not need to
 * care about synchronisation.
 * @param itr the current iteration try in freeing resources
 * @param data user supplied data at registration time for the handler
 * @param context context pointer for this collection run, might be NULL (at least for UNREGISTER and PANIC)
 * @return indication what the the handler really did (LUMIERA_RESOURCE_NONE when it didn't obey the request)
 */
typedef enum lumiera_resource_try (*lumiera_resource_handler_fn)(enum lumiera_resource_try itr, void* data, void* context);
typedef int (*lumiera_resourcecollector_run_fn) (enum lumiera_resource which, enum lumiera_resource_try* iteration, void* context);

typedef struct lumiera_resourcehandler_struct lumiera_resourcehandler;
typedef lumiera_resourcehandler* LumieraResourcehandler;


/**
 * Initialise the Resourcecollector.
 * The Resourcecollector is singleton and can be used after initialised once.
 */
void
lumiera_resourcecollector_init (void);


/**
 * Destroy the resource collector registry.
 * Unregisters and deletes all handlers.
 */
void
lumiera_resourcecollector_destroy (void);


/**
 * Try to free resources.
 *
 * @param which The kind of resource to be acquired
 * @param iteration a pointer to a local iterator, initialised with the start
 *                  value for the loop
 * @param context NULL or some context dependent data for the needed resource
 *                this is a pointer to a size_t for MEMORY and a pointer to a filename
 *                (to find out about the device) for STORAGE and CACHE resources
 * @return either returns 1 or calls _exit()
 *
 * @code
 * void* data;
 * size_t size = 1000;
 * enum lumiera_resource_try iteration = LUMIERA_RESOURCE_ONE;
 * do {
 *   data = malloc (size);
 * } while (!data && lumiera_resourcecollector_run (LUMIERA_RESOURCE_MEMORY, &iteration, &size));
 * @endcode
 */
int
lumiera_resourcecollector_run (enum lumiera_resource which, enum lumiera_resource_try* iteration, void* context);


/**
 * Registers a new collector handler
 * @param resource resource for which this handler shall be registered
 * @param handler pointer to the handler function
 * @param data opaque user-data pointer which will be passed to the handler
 * @return pointer to the internal handler structure. This can be used to unregister the handler.
 */
LumieraResourcehandler
lumiera_resourcecollector_register_handler (enum lumiera_resource resource, lumiera_resource_handler_fn handler, void* data);


/**
 * Unregisters a collector handle
 * Removes the handler from the registry and calls it once with LUMIERA_RESOURCE_UNREGISTER
 * to give it a chance to free the user supplied data. Must not be called after lumiera_resourcecollector_destroy()
 * @param self pointer to internal handler structure, obtained from register_handler() or handler_find(), might be NULL
 */
void
lumiera_resourcehandler_unregister (LumieraResourcehandler self);

/**
 * Looks up a handler.
 * Used to find a registered handler when the return value of register_handler() was not practical to store.
 * @param resource resource for which this handler was registered
 * @param handler pointer to the handler function, same as used for registering
 * @param data opaque user-data pointer, same as used for registering
 * @return pointer to the internal handler structure or NULL if no handler was found
 */
LumieraResourcehandler
lumiera_resourcecollector_handler_find (enum lumiera_resource resource, lumiera_resource_handler_fn handler, void* data);



#endif /*BACKEND_RESOURCECOLLECTOR_H*/
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
