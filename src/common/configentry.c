/*
  Configentry  -  single entries from configfiles

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file configentry.c
 ** Draft for a configuration system (2008).
 ** @todo as of 2016 this code is unused and
 **       likely to be replaced by a different approach.
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



