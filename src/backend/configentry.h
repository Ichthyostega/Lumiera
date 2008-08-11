/*
  configentry.h  -  single entries from configfiles

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

#ifndef LUMIERA_CONFIGENTRY_H
#define LUMIERA_CONFIGENTRY_H

//TODO: Support library includes//


//TODO: Forward declarations//
typedef struct lumiera_configentry_struct lumiera_configentry;
typedef lumiera_configentry* LumieraConfigentry;


//TODO: Lumiera header includes//
#include "backend/configitem.h"

//TODO: System includes//
#include <nobug.h>


/**
 * @file
 */

//TODO: declarations go here//
struct lumiera_configentry_struct
{
  lumiera_configitem entry;
};

void
lumiera_configentry_delete ();


#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
