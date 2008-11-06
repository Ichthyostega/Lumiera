/*
  configitem.h  -  generalized hierachy of configuration items

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

#ifndef LUMIERA_CONFIGITEM_H
#define LUMIERA_CONFIGITEM_H

//TODO: Support library includes//
#include "lib/llist.h"


//TODO: Forward declarations//
typedef struct lumiera_configitem_struct lumiera_configitem;
typedef lumiera_configitem* LumieraConfigitem;

struct lumiera_configitem_vtable;

//TODO: Lumiera header includes//
#include "lumiera/config_lookup.h"


//TODO: System includes//
#include <nobug.h>


/**
 * @file
 * configitems build a 3 level hierachy:
 *
 * 1. file:
 *      contain sections
 *
 * 2.   section:
 *        [prefix suffix]
 *        contain lines
 *
 * 3.     lines are
 *        comment:
 *          empty line or line only containing spaces and tabs
 *          line starting with spaces and tabs followed by a #
 *        directive:
 *          '@include name' or '@readonly'
 *          directives are only valid at the toplevel section []
 *        configurationentry:
 *          'key = value' or 'key < redirect'
 */


//TODO: declarations go here//
/**
 * @file
 * configitems build a 3 level hierachy:
 *
 * 1. file:
 *      contain sections
 *
 * 2.   section:
 *        [prefix suffix]
 *        contain lines
 *
 * 3.     lines are
 *        comment:
 *          empty line or line only containing spaces and tabs
 *          line starting with spaces and tabs followed by a #
 *        directive:
 *          '@include name' or '@readonly'
 *          directives are only valid at the toplevel section []
 *        configurationentry:
 *          'key = value' or 'key < redirect'
 *        errorneous:
 *          any line which cant be parsed
 */

struct lumiera_configitem_vtable
{
  LumieraConfigitem (*new)(LumieraConfigitem);
  LumieraConfigitem (*destroy)(LumieraConfigitem);
};

struct lumiera_configitem_struct
{
  llist link;                                   // all lines on the same hierachy level are linked here (see childs)
  LumieraConfigitem parent;                     // parent section
  llist childs;                                 // root node for all lines below this hierachy

  llist lookup;                                 // all lines with the same key are stacked up on the loockup

  char* line;                                   // raw line as read in allocated here trailing \n will be replaced with \0
  char* key;                                    // pointer into line to start of key
  size_t key_size;
  char* delim;                                  // delimiter, value starts at delim+1
  struct lumiera_configitem_vtable* vtable;     // functiontable for subclassing
};

LumieraConfigitem
lumiera_configitem_init (LumieraConfigitem self);

LumieraConfigitem
lumiera_configitem_destroy (LumieraConfigitem self, LumieraConfigLookup lookup);

LumieraConfigitem
lumiera_configitem_new (const char* line);

void
lumiera_configitem_delete (LumieraConfigitem self, LumieraConfigLookup lookup);

LumieraConfigitem
lumiera_configitem_set_value (LumieraConfigitem self, const char* delim_value);

LumieraConfigitem
lumiera_configitem_parse (LumieraConfigitem self, const char* line);

LumieraConfigitem
lumiera_configitem_move (LumieraConfigitem self, LumieraConfigitem dest);

#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
