/*
  configitem.c  -  generalized hierachy of configuration items

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

//TODO: Support library includes//
#include "lib/llist.h"
#include "lib/safeclib.h"


//TODO: Lumiera header includes//
#include "backend/config.h"
#include "backend/configitem.h"
#include "backend/configentry.h"

//TODO: internal/static forward declarations//


//TODO: System includes//


/**
 * @file
 *
 */


//code goes here//

LumieraConfigitem
lumiera_configitem_init (LumieraConfigitem self)
{
  TRACE (config_item);
  REQUIRE (self);

  llist_init (&self->link);
  self->parent = NULL;
  llist_init (&self->childs);

  llist_init (&self->lookup);

  self->line = NULL;

  self->key = NULL;
  self->key_size = 0;
  self->delim = NULL;
  self->type = LUMIERA_CONFIGERRONEOUS;

  return self;
}

LumieraConfigitem
lumiera_configitem_destroy (LumieraConfigitem self)
{
  TRACE (config_item);
  REQUIRE (self);

#if 0
  if (!llist_is_empty (&self->lookup))
    TODO ("remove from the lookup hash");

  ENSURE (llist_is_empty (&self->childs), "TODO recursively remove childs (needs some kind of typed destructor)");
  llist_unlink (&self->link);
  lumiera_free (self->line);
#endif
  return self;
}


LumieraConfigitem
lumiera_configitem_new (const char* line)
{
  TRACE (config_item);

  /* MOCKUP */

  // create a temporary  configitem for parsing


  lumiera_configitem tmp;
  lumiera_configitem_init (&tmp);

  lumiera_configitem_parse (&tmp, line);

  FIXME ("MOCKUP only");

  LumieraConfigitem self = NULL;

  switch (tmp.type)
    {
    case LUMIERA_CONFIGFILE:

      break;

    case LUMIERA_CONFIGSECTION:
      break;

    case LUMIERA_CONFIGCOMMENT:
      break;

    case LUMIERA_CONFIGDIRECTIVE:
      break;

    case LUMIERA_CONFIGENTRY:
      self = lumiera_malloc (sizeof (lumiera_configentry));

      TODO ("call lumiera_configentry ctor");
      break;

    case LUMIERA_CONFIGERRONEOUS:
      break;
    }

  lumiera_configitem_move (self, &tmp);

  return self;
}


void
lumiera_configitem_delete (LumieraConfigitem self)
{
  TRACE (config_item);

  UNIMPLEMENTED ();
}



LumieraConfigitem
lumiera_configitem_move (LumieraConfigitem self, LumieraConfigitem source)
{
  TRACE (config_item);
  REQUIRE (self);
  REQUIRE (source);

  llist_move (&self->link, &source->link);

  self->parent = source->parent;

  llist_move (&self->childs, &source->childs);
  llist_move (&self->lookup, &source->lookup);

  self->line = source->line;
  source->line = NULL;

  self->key = source->key;
  self->key_size = source->key_size;
  self->delim = source->delim;
  self->type = source->type;
  return self;
}


LumieraConfigitem
lumiera_configitem_parse (LumieraConfigitem self, const char* line)
{
  TRACE (config_item);

  FIXME ("MOCKUP only");

  self->line = lumiera_strndup (line, SIZE_MAX);
  TODO ("parsing here");

  // MOCKUP pretrend this is an entry
  self->type = LUMIERA_CONFIGENTRY;

  return self;
}


/*
brainstorm:

identify the type of a configitem:

file:
  parent == NULL
  line = filename (might be NULL for virtual files)
  delim = NULL
section:
  *delim == ' ' or ']'
  *key != '@'
comment:
  *key == NULL
directive:
  *key == '@'
  *delim == ' '
configurationentry:
  *delim == '='

*/




/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
