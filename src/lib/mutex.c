/*
  mutex.c  -  mutex

   Copyright (C)
     2008, 2009,      Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.
*/

#include "lib/mutex.h"

/**
 * @file
 * Mutual exclusion locking.
 */

LumieraMutex
lumiera_mutex_init (LumieraMutex self,
                    const char* purpose,
                    struct nobug_flag* flag,
                    const struct nobug_context ctx)
{
  if (self)
    {
      NOBUG_RESOURCE_HANDLE_INIT (self->rh);
      NOBUG_RESOURCE_ANNOUNCE_RAW_CTX (flag, "mutex", purpose, self, self->rh, ctx)
        {
          pthread_mutex_init (&self->mutex, NULL);
        }
    }
  return self;
}


LumieraMutex
lumiera_mutex_destroy (LumieraMutex self,
                       struct nobug_flag* flag,
                       const struct nobug_context ctx)
{
  if (self)
    {
      NOBUG_RESOURCE_FORGET_RAW_CTX (flag, self->rh, ctx)
        {
          if (pthread_mutex_destroy (&self->mutex))
            LUMIERA_DIE (LOCK_DESTROY);
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
