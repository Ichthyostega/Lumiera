/*
  configitem.c  -  generalized hierachy of configuration items

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>
                        Simeon Voelkel <simeon_voelkel@arcor.de>

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
#include <ctype.h>
#include <stdint.h>

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
  self->vtable = NULL;

  return self;
}

LumieraConfigitem
lumiera_configitem_destroy (LumieraConfigitem self, LumieraConfigLookup lookup)
{
  TRACE (config_item);

  if (self)
    {
      LLIST_WHILE_HEAD (&self->childs, node)
        lumiera_configitem_delete ((LumieraConfigitem) node, lookup);

      ENSURE (llist_is_empty (&self->childs), "destructor didn't remove childs");

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
  TRACE (config_item);

  lumiera_configitem tmp;
  lumiera_configitem_init (&tmp);

  lumiera_configitem_parse (&tmp, line);

  LumieraConfigitem self = tmp.vtable && tmp.vtable->new
    ? tmp.vtable->new (&tmp)
    : lumiera_configitem_move (lumiera_malloc (sizeof (*self)), &tmp);

  TRACE (config_item, "key size of %s is %d", tmp.key, tmp.key_size);
  return self;
}


void
lumiera_configitem_delete (LumieraConfigitem self, LumieraConfigLookup lookup)
{
  TRACE (config_item);
  lumiera_free (lumiera_configitem_destroy (self, lookup));
}



LumieraConfigitem
lumiera_configitem_move (LumieraConfigitem self, LumieraConfigitem source)
{
  TRACE (config_item);
  REQUIRE (self);
  REQUIRE (source);

  llist_init (&self->link);
  llist_insertlist_next (&self->link, &source->link);

  self->parent = source->parent;

  llist_init (&self->childs);
  llist_insertlist_next (&self->childs, &source->childs);

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
  TRACE (config_item);

  self->line = lumiera_strndup (line, SIZE_MAX);

  FIXME ("MOCKUP START");

  TODO ("parsing here");
  /*
    HOWTO parse (for simav)
    in self->line liegt jetzt der 'rohe' string

    parsen setzt folgende werte in self:  .key, .key_size, .delim und vtable. den rest macht dann die 'new' funktion aus der vtable

    es geht jetzt da drum rauszufinden ob diese zeile einses der folgenden sachen ist:
    (ich zeig hier nur die grundsyntax, das parsen sollte auch entartete situationen behandeln, insbesondere leerzeichen/tabulatoren an allen moeglichen stellen)
    auserdem sollt hier alles soweit wie moeglich validiert werden z.b. keys auf erlaubte zeichen gescheckt (siehe die _tr function)

    section:
      '[prefix suffix]'
       .key == prefix
       .delim == das leerzeichen (oder tab) vor suffix oder aufs abschliessende ] wenn kein suffix

    kommentar:
      leere zeile, zeile nur aus leerzeichen und tabulatoren, leerzeichen und tabulatoren gefolgt von # bis zum zeilenende
      alles ausser vtable auf NULL

    direktive:
      '@direktive argumente'
       .key == @
       .delim == leerzeichen oder tab vor argumente, NULL wenn keine argumente

    configentry:
      'key = value'
       .key == key begin
       .delim == '='
      'key < redirect'
       .key == key begin
       .delim == '>'

   */
  /*
   * What should be working (for cehteh) or not yet..
   *
   * die Elemente sollten bereits richtig unterschieden werden, die {} sind noch zu füllen.
   *
   * */

  char* itr = self->line;

  /*skip leading whitespaces*/
  while (*itr && isspace (*itr))
    itr++;

  /*decide what this line represents*/
  if (!*itr || *itr == '#' )
    {
      /*this is an empty line or a  a comment*/
    }
  else if (*itr == '@' )
    {
      /*this is a directive*/
    }
  else if (*itr == '[' )
    {
      /*this is a section*/

      /*skip blanks before prefix*/
      itr++;
      while (*itr && isspace(*itr))
        itr++;

      /*itr points now to the begin of the key*/
      self->key = itr;

      /*now look for the end of the key and set the keysize*/
      self->key_size = strspn (itr, LUMIERA_CONFIG_KEY_CHARS);

      itr += self->key_size;

      /*if the line ends ends with prefix] delim points to ] 
       * and not the last (blank) character before the final square bracket*/
      if (self->key_size && *itr && *itr == ']')
        {
          self->delim = itr;
          TODO("self->vtable = &lumiera_configsection_funcs;");
        }
      else if (self->key_size && *itr && isspace(*itr))
        {
          /* skip blanks until we reach the suffix or the final square bracket*/
          while (*itr && isspace(*itr))
            itr++;

          if (*itr && *itr == ']')
            {
              /*final square bracket reached, so place delim one char before the 
               * actual position which must be a whitespace: no extra check necessary*/
              self->delim = itr - 1;
              TODO("self->vtable = &lumiera_configsection_funcs;");
            }
          else if (*itr)
            {
              TODO("check wheter suffix is made of legal characters");

              /*delim points to the last whitespace before the actual position;
               * no extra check needed*/
              self->delim = itr - 1;
              TODO("self->vtable = &lumiera_configsection_funcs;");
            }
          else
            {
              /*malformed section line, treat this line like a comment*/
              self->key = NULL;
              self->key_size = 0;

              LUMIERA_ERROR_SET (config_item, CONFIG_SYNTAX);

            }
        }
      else
        {
          /*error: either *itr is false, points neither to a blank nor to a closed square 
           * bracket or the key_size is zero*/

          /*treat this line like a comment*/
          self->key = NULL;
          self->key_size = 0;

          LUMIERA_ERROR_SET (config_item, CONFIG_SYNTAX);

        }
    }
  else
    {
      /*this is probably a configentry*/

      /*itr points now to the first not-whitespace-character*/
      self->key = itr;

      /*now look for the end of the key and set the keysize*/
      self->key_size = strspn (itr, LUMIERA_CONFIG_KEY_CHARS);

      /* skip blanks */
      itr += self->key_size;
      while (*itr && isspace (*itr))
        itr++;

      if (self->key_size && *itr == '=')
        {
          /*this configentry assigns a value to a key*/
          self->delim = itr;
          self->vtable = &lumiera_configentry_funcs;
        }
      else if (self->key_size && *itr == '<')
        {
          /*this configentry is a redirect*/
          self->delim = itr;
          self->vtable = &lumiera_configentry_funcs;
        }
      else
        {
          /*this is not a valid configentry; treat this line like a comment*/
          self->key = NULL;
          self->key_size = 0;

          LUMIERA_ERROR_SET (config_item, CONFIG_SYNTAX);
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
