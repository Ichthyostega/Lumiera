/*
  mrucache.h  -  most recent used cache

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

#include "lib/mrucache.h"

/**
 * @file Most recent used cache
 * Elements (addressed by a LList node) are either in the cache and thereby subject of aging
 * or checked out under control of the user.
 */


LumieraMruCache
lumiera_mrucache_init (LumieraMruCache self, lumiera_cache_destructor_fn destructor_cb)
{
  REQUIRE (self);
  llist_init (&self->cache_list);
  self->cached = 0;
  self->destructor_cb = destructor_cb;
  return self;
}

LumieraMruCache
lumiera_mrucache_destroy (LumieraMruCache self)
{
  LLIST_WHILE_TAIL (&self->cache_list, node)
    {
      if (self->destructor_cb)
        free (self->destructor_cb (node));
      else
        free (node);
    }
  self->cached = 0;
  return self;
}


/**
 * destroy and free the nelem oldest elements
 */
int
lumiera_mrucache_age (LumieraMruCache self, size_t int)
{
  REQUIRE (self);
  while (self->cached && nelem--)
    free (lumiera_mrucache_pop (self));

  return nelem;
}
