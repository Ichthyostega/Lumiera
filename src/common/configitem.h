/*
  CONFIGITEM.h  -  generalised hierarchy of configuration items

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file configitem.h
 ** Draft for a configuration system (2008).
 ** @todo as of 2016 this code is unused and
 **       likely to be replaced by a different approach.
 ** 
 ** # Hierarchy of configuration items.
 ** configitems form a 3 level hierarchy:
 ** 
 ** \verbatim
 ** 1. file:
 **      contain sections
 **
 ** 2.   section:
 **        [prefix suffix]
 **        contain lines
 **
 ** 3.     lines are
 **        comment:
 **          empty line or line only containing spaces and tabs
 **          line starting with spaces and tabs followed by a #
 **        directive:
 **          '@include name' or '@readonly'
 **          directives are only valid at the toplevel section []
 **        configurationentry:
 **          'key = value' or 'key < redirect'
 **        erroneous:
 **          any line which can't be parsed
 ** \endverbatim
 */

#ifndef COMMON_CONFIGITEM_H
#define COMMON_CONFIGITEM_H


#include "lib/llist.h"

/* Forward declarations */
typedef struct lumiera_configitem_struct lumiera_configitem;
typedef lumiera_configitem* LumieraConfigitem;

struct lumiera_configitem_vtable;

#include "common/config-lookup.h"

#include <nobug.h>



struct lumiera_configitem_vtable
{
  LumieraConfigitem (*newitem)(LumieraConfigitem);
  LumieraConfigitem (*destroy)(LumieraConfigitem);
};

struct lumiera_configitem_struct
{
  llist link;                                   // all lines on the same hierarchy level are linked here (see children)
  LumieraConfigitem parent;                     // parent section
  llist children;                               // root node for all lines below this hierarchy

  llist lookup;                                 // all lines with the same key are stacked up on the lookup

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


#endif /*COMMON_CONFIGITEM_H*/
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
