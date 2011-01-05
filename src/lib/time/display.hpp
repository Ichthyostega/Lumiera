/*
  DISPLAY.hpp  -  formatting of time values for display

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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


#ifndef LIB_TIME_DISPLAY_H
#define LIB_TIME_DISPLAY_H

#include "lib/time/timecode.hpp"

#include <iostream>
#include <string>



namespace lib {
namespace time {
  
  
   /* === shortcuts for diagnostic output === */
    
  /** writes time value, formatted as HH:MM:SS:mmm */
  inline std::ostream& operator<< (std::ostream& os, Time const& t)  { return os << string(t); }
  inline std::ostream& operator<< (std::ostream& os, TCode const& t) { return os << string(t); }
  
  
  
}} // lib::time
#endif
