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


//TODO: Forward declarations//
typedef struct lumiera_configitem_struct lumiera_configitem;
typedef struct lumiera_configitem* LumieraConfigitem;


//TODO: Lumiera header includes//


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

enum lumiera_configitem_type
  {
    LUMIERA_CONFIGFILE,
    LUMIERA_CONFISECTION,
    LUMIERA_CONFIGCOMMENT,
    LUMIERA_CONFIGDIRECTIVE,
    LUMIERA_CONFIGENTRY,
    LUMIERA_CONFIGERRONEOUS
  };

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



struct lumiera_configitem_struct
{
  llist link;                   // all lines on the same hierachy level are linked here (see childs)
  LumieraConfigitem parent;     // parent section
  llist childs;                 // root node for all lines below this hierachy

  llist lookup;                 // all lines with the same key are stacked up on the loockup

  char* line;                   // raw line as read in allocated here trailing \n will be replaced with \0
  char* key;                    // pointer into line to start of key
  size_t key_size;
  char* delim;                  // delimiter, value starts at delim+1
};

struct lumiera_configitem_vtable
{

};


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


#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
