/*
  NoBugInit  -  pull up NoBug automagically in static initialisation 
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
* *****************************************************/


#include "lib/nobug-init.hpp"



namespace lumiera {
  
  void 
  initialise_NoBug () 
  {
    NOBUG_INIT;

////////////////////////////////////////////////////////////////////////TODO: a better way to detect Alpha/beta builds
#ifdef DEBUG
    static uint callCount = 0;
    ASSERT ( 0 == callCount++ );
#endif
  }

}

