/*
  Configentry  -  single entries from configfiles

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

* *****************************************************/


/** @file configentry.c
 ** TODO configentry.c
 */


#include "lib/safeclib.h"
#include "common/configentry.h"



LumieraConfigitem
lumiera_configentry_new (LumieraConfigitem tmp)
{
  LumieraConfigentry self = lumiera_malloc (sizeof (*self));
  lumiera_configitem_move ((LumieraConfigitem)self, tmp);

  //////////////////////////////////////////////////////////////////TICKET #839 initialise other stuff here (lookup, parent, ...)

  return (LumieraConfigitem)self;
}


LumieraConfigitem
lumiera_configentry_destroy (LumieraConfigitem self)
{
  //////////////////////////////////////////////////////////////////TICKET #839 cleanup other stuff here (lookup, parent, ...)

  return self;
}

struct lumiera_configitem_vtable lumiera_configentry_funcs =
  {
    .newitem = lumiera_configentry_new,
    .destroy = lumiera_configentry_destroy
  };



