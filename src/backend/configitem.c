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
lumiera_configitem_destroy (LumieraConfigitem self)
{
  TRACE (config_item);

  if (self)
    {
      if (self->vtable && self->vtable->destroy)
        self->vtable->destroy (self);

      ENSURE (!llist_is_empty (&self->lookup), "destructor didn't cleaned lookup up");
      ENSURE (llist_is_empty (&self->childs), "destructor didn't remove childs");

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

  return self;
}


void
lumiera_configitem_delete (LumieraConfigitem self)
{
  TRACE (config_item);
  lumiera_free (lumiera_configitem_destroy (self));
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
   * TODO: include für verwendete Funkionen wie strlen und isspace
   *
   * */

  int linelength = strlen(self->line);
  int pos = 0;

  char* tmp1 = self->line;

  /*skip leading whitespaces*/
  while ( isspace(tmp1[0]) && pos < linelength )
    {
      tmp1++;
      pos++:
    }

  /*decide what this line represents*/
  if ( tmp1[0] == '\0' || pos == linelenght )
    {
      /*this was an empty line*/
    }
  else
  if ( tmp1[0] == '#' )
    {
      /*this was a comment*/
    }
  else
  if ( tmp1[0] == '@' )
    {
      /*this was a directive*/
    }
  else
  if ( tmp1[0] == '[' )
    {
      /*this was a section*/
    }
  else
    {
      /*this was a configentry*/
      
      /*tmp1 points now to the first not-whitespace-character*/
      self->key = tmp1;

      /*now look for the end of the key and set the keysize*/
      self->keysize = 0;

      while ( ! isspace(tmp1[0]) && pos < linelength )
        {
          tmp1++;
          self->keysize++;
          pos++;
        }

      if ( tmp1[0] == '\0' || pos == linelength )
        {
          /*error: line ended with end of the key*/
        }
      else 
        {
          /*skip the following whitespaces until we reach the delimeter*/
          while ( isspace(tmp1[0]) && pos < linelength )
            {
              tmp1++;
              pos++;
            }
          /*TODO: keep on parsing... ;^)*/
        }

    }


  self->vtable = &lumiera_configentry_funcs;  // MOCKUP pretend this is a configentry

  return self;
}


/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
