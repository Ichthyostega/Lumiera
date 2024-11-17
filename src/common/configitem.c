/*
  Configitem  -  generalised hierarchy of configuration items

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>
                        Simeon Voelkel <simeon_voelkel@arcor.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.
*/


/** @file configitem.c
 ** Implementation draft for a configuration system (2008).
 ** Create a configitem from a single line of the config file.
 ** @todo as of 2016 this code is unused and
 **       likely to be replaced by a different approach.
 */


#include "include/logging.h"
#include "lib/llist.h"
#include "lib/safeclib.h"
#include "lib/tmpbuf.h"

#include "common/config.h"
#include "common/configitem.h"
#include "common/configentry.h"

#include <ctype.h>
#include <stdint.h>


static LumieraConfigitem parse_directive (LumieraConfigitem self, char* itr);

static LumieraConfigitem parse_section (LumieraConfigitem self, char* itr);

static LumieraConfigitem parse_configentry (LumieraConfigitem self, char* itr);





LumieraConfigitem
lumiera_configitem_init (LumieraConfigitem self)
{
  TRACE (configitem_dbg);
  REQUIRE (self);

  llist_init (&self->link);
  self->parent = NULL;
  llist_init (&self->children);

  llist_init (&self->lookup);

  self->line = NULL;

  self->key = NULL;
  self->key_size = 0;
  self->delim = NULL;
  self->vtable = NULL;

  return self;
}


LumieraConfigitem
lumiera_configitem_destroy (LumieraConfigitem self, LumieraConfigLookup lookup)
{
  TRACE (configitem_dbg);

  if (self)
    {
      LLIST_WHILE_HEAD (&self->children, node)
        lumiera_configitem_delete ((LumieraConfigitem) node, lookup);

      ENSURE (llist_is_empty (&self->children), "destructor didn't remove children");

      if (self->vtable && self->vtable->destroy)
        self->vtable->destroy (self);

      if (!llist_is_empty (&self->lookup))
        lumiera_config_lookup_remove (lookup, self);

      llist_unlink (&self->link);
      lumiera_free (self->line);
    }

  return self;
}


LumieraConfigitem
lumiera_configitem_new (const char* line)
{
  TRACE (configitem_dbg, "%s", line);

  lumiera_configitem tmp;
  lumiera_configitem_init (&tmp);

  lumiera_configitem_parse (&tmp, line);

  LumieraConfigitem self = tmp.vtable && tmp.vtable->newitem
    ? tmp.vtable->newitem (&tmp)
    : lumiera_configitem_move (lumiera_malloc (sizeof (*self)), &tmp);

  return self;
}


void
lumiera_configitem_delete (LumieraConfigitem self, LumieraConfigLookup lookup)
{
  TRACE (configitem_dbg);
  lumiera_free (lumiera_configitem_destroy (self, lookup));
}


LumieraConfigitem
lumiera_configitem_set_value (LumieraConfigitem self, const char* delim_value)
{
  REQUIRE (self->key);
  REQUIRE (self->delim);

  char* line = lumiera_tmpbuf_snprintf (SIZE_MAX, "%.*s%s", self->delim - self->line, self->line, delim_value);
  lumiera_configitem_parse (self, line);

  return self;
}


LumieraConfigitem
lumiera_configitem_move (LumieraConfigitem self, LumieraConfigitem source)
{
  TRACE (configitem_dbg);
  REQUIRE (self);
  REQUIRE (source);

  llist_init (&self->link);
  llist_insertlist_next (&self->link, &source->link);

  self->parent = source->parent;

  llist_init (&self->children);
  llist_insertlist_next (&self->children, &source->children);

  llist_init (&self->lookup);
  llist_insertlist_next (&self->lookup, &source->lookup);

  self->line = source->line;
  source->line = NULL;

  self->key = source->key;
  self->key_size = source->key_size;
  self->delim = source->delim;
  self->vtable = source->vtable;

  return self;
}


LumieraConfigitem
lumiera_configitem_parse (LumieraConfigitem self, const char* line)
{
  TRACE (configitem_dbg);

  lumiera_free (self->line);
  self->line = lumiera_strndup (line, SIZE_MAX);

  /////////////////////////TODO do a real startup here

  char* itr = self->line;

  /* skip leading whitespace */
  while (*itr && isspace (*itr))
    itr++;

  /* decide what this line represents */
  if (!*itr || *itr == '#' )
    {
      /* this is an empty line or a  a comment */
    }
  else if (*itr == '@' )
    {
      /* this is a directive */
      self = parse_directive (self, itr);
    }
  else if (*itr == '[' )
    {
      /* this is a section */
      self = parse_section (self, itr);
    }
  else
    {
      /* this is probably a configentry */
      self = parse_configentry (self, itr);
    }

  return self;
}


static LumieraConfigitem
parse_directive (LumieraConfigitem self, char* itr)
{
  /* itr points now to @ */
  self->key = itr;

  /* check whether there are illegal whitespace after @ */
  itr++;
  if (*itr && !isspace(*itr))
    {
      /* now look for the end of the directive and set the keysize */
      self->key_size = strspn (itr, LUMIERA_CONFIG_KEY_CHARS);

      itr += self->key_size;

      /* we need a key with a length greater than zero and */
      /* either end of line or whitespace after key */

      if ( self->key_size && ( !*itr || (*itr && isspace(*itr)) ))
        {
          /* look for given arguments */

          /* skip blanks */
          while (*itr && isspace (*itr))
            itr++;

          if (*itr)
            {
              /* there are arguments given, thus set delim */
              self->delim = itr - 1;
            }
          else
            {
              /* no arguments were given */
              self->delim = NULL;
            }
        }
      else
        {
          /* malformed lines shall be treated like if they were comments */
          self->key = NULL;
          self->key_size = 0;

          LUMIERA_ERROR_SET (config, CONFIG_SYNTAX, self->line);
        }
    }
  else
    {
      /* there occurred already an error right after the @! */
      /* malformed lines shall be treated like if they were comments */
      self->key = NULL;
      self->key_size = 0;

      LUMIERA_ERROR_SET (config, CONFIG_SYNTAX, self->line);
    }
  return self;
}


static LumieraConfigitem
parse_section (LumieraConfigitem self, char* itr)
{
  /* skip blanks before prefix */
  itr++;
  while (*itr && isspace(*itr))
    itr++;

  /* itr points now to the begin of the key */
  self->key = itr;

  /* now look for the end of the key and set the keysize */
  self->key_size = strspn (itr, LUMIERA_CONFIG_KEY_CHARS);

  itr += self->key_size;

  /* if the line ends ends with prefix] delim points to ] */
  /* and not the last (blank) character before the final square bracket */
  if (self->key_size && *itr && *itr == ']')
    {
      self->delim = itr;
      TODO("self->vtable = &lumiera_configsection_funcs;");
    }
  else if (self->key_size && *itr && isspace(*itr))
    {
      /* skip blanks until we reach the suffix or the final square bracket */
      while (*itr && isspace(*itr))
        itr++;

      if (*itr && *itr == ']')
        {
          /* final square bracket reached, so place delim one char before the */
          /* actual position which must be a whitespace: no extra check necessary */
          self->delim = itr - 1;
          TODO("self->vtable = &lumiera_configsection_funcs;");
        }
      else if (*itr)
        {
          TODO("check wheter suffix is made of legal characters");

          /* delim points to the last whitespace before the actual position; */
          /* no extra check needed */
          self->delim = itr - 1;
          TODO("self->vtable = &lumiera_configsection_funcs;");
        }
      else
        {
          /* malformed section line, treat this line like a comment */
          self->key = NULL;
          self->key_size = 0;

          LUMIERA_ERROR_SET (config, CONFIG_SYNTAX, self->line);
        }
    }
  else
    {
      /* error: either *itr is false, points neither to a blank nor to a closed square */
      /* bracket or the key_size is zero */

      /* treat this line like a comment */
      self->key = NULL;
      self->key_size = 0;

      LUMIERA_ERROR_SET (config, CONFIG_SYNTAX, self->line);
    }

  return self;
}


static LumieraConfigitem
parse_configentry (LumieraConfigitem self, char* itr)
{
  /* itr points now to the first not-whitespace-character */
  self->key = itr;

  /* now look for the end of the key and set the keysize */
  self->key_size = strspn (itr, LUMIERA_CONFIG_KEY_CHARS);

  /* skip blanks  */
  itr += self->key_size;
  while (*itr && isspace (*itr))
    itr++;

  if (self->key_size && *itr == '=')
    {
      /* this configentry assigns a value to a key */
      self->delim = itr;
      self->vtable = &lumiera_configentry_funcs;
    }
  else if (self->key_size && *itr == '<')
    {
      /* this configentry is a redirect */
      self->delim = itr;
      self->vtable = &lumiera_configentry_funcs;
    }
  else
    {
      /* this is not a valid configentry; treat this line like a comment */
      self->key = NULL;
      self->key_size = 0;

      LUMIERA_ERROR_SET (config, CONFIG_SYNTAX, self->line);
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
