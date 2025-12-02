/*
  CONFIGENTRY.h  -  single entries from configfiles

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file configentry.h
 ** Draft for a configuration system (2008).
 ** @todo as of 2016 this code is unused and
 **       likely to be replaced by a different approach.
 */


#ifndef COMMON_CONFIGENTRY_H
#define COMMON_CONFIGENTRY_H

#include "common/configitem.h"

#include <nobug.h>



typedef struct lumiera_configentry_struct lumiera_configentry;
typedef lumiera_configentry* LumieraConfigentry;


struct lumiera_configentry_struct
{
  lumiera_configitem entry;
};

extern struct lumiera_configitem_vtable lumiera_configentry_funcs;


LumieraConfigitem
lumiera_configentry_new (LumieraConfigitem tmp);


LumieraConfigitem
lumiera_configentry_destroy (LumieraConfigitem self);

#endif /*COMMON_CONFIGENTRY_H*/
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
