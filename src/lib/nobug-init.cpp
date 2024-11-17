/*
  NoBugInit  -  NoBug static initialisation and definition of logging vars 

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file nobug-init.cpp
 ** Implementation function for automatic trigger of NoBug initialisation
 ** @note `#include` nobug-init.hpp to use this initialisation automatically
 */


#include <nobug.h>

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
