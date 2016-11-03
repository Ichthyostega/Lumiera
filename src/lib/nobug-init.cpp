/*
  NoBugInit  -  NoBug static initialisation and definition of logging vars 

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

* *****************************************************/


/** @file nobug-init.cpp
 ** TODO nobug-init.cpp
 */


#include "lib/nobug-init.hpp"

/// magic to generate NoBug logging definitions
#define LUMIERA_NOBUG_INIT_CPP
#include "include/logging.h"



namespace lumiera {
  
  void 
  initialise_NoBug () 
  {
    NOBUG_INIT;
    
#if NOBUG_MODE_ALPHA
    static uint callCount = 0;
    ASSERT ( 0 == callCount++ );
#endif
  }
  
  
}
