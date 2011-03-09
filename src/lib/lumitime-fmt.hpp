/*
  LUMITIME-FMT.hpp  -  output and formatting of time(code) values

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


/** @file lumitime-fmt.hpp
 ** Extension to the lumiera::Time wrapper, adding output and specific
 ** time formats. For now (7/09) this header serves the purpose to add
 ** the (not so lightweight) includes necessary for formatting. For later
 ** the intention is to have "format aware" time values (as a subclass),
 ** carrying the additional information about current framerate and
 ** other time-format preferences
 ** 
 ** @todo this should be a mixed C/C++ header and really add the handling
 **       of multiple timecode-formats :-P
 **  
 ** @see time.h
 ** @see lumitime.hpp
 **
 */





#ifndef LUMIERA_LUMITIME_FMT_H
#define LUMIERA_LUMITIME_FMT_H

#include "lib/lumitime.hpp"
#include "lib/time.h"

#include <iostream>


namespace lumiera {


  /** writes time value, formatted as HH:MM:SS:mmm
   *  @see lumiera_tmpbuf_print_time  */
  inline std::ostream&
  operator<< (std::ostream& os, Time const& t)
    {
      return os << lumiera_tmpbuf_print_time(t);
    }
  
  
  
  
} // namespace lumiera
#endif
