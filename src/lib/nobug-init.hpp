/*
  NOBUG-INIT.hpp  -  pull up NoBug automagically in static initialisation
 

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

*/

/** @file nobug-init.hpp
 ** Trigger the basic NoBug initialisation by placing a static variable.
 ** Any facility which uses NoBug (especially logging) already within the
 ** static initialisation phase of the application, should do the include
 ** via this header, which additionally installs a Lifecycle callback to
 ** be run as early as possible.
 **
 ** @see logging.h
 ** 
 */


#ifndef NOBUG_INIT_H
#define NOBUG_INIT_H

#include "include/lifecycle.h"

#include <nobug.h>



namespace lumiera { 
  void initialise_NoBug ();
  
  namespace {
    LifecycleHook trigger_init_ (ON_BASIC_INIT, &initialise_NoBug);         
} }


#endif /* NOBUG_INIT_H */
