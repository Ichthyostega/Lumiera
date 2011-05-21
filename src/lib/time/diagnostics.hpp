/*
  DIAGNOSTICS.hpp  -  diagnostics and output helpers for time(code) values

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file diagnostics.hpp
 ** Extension to the lib::time::Time wrapper, adding output and further
 ** diagnostic aids. This shouldn't be confused with formatting into
 ** distinctive \em Timecode formats. There is an elaborate framework
 ** for the latter: basically you'd need first to create a frame quantised
 ** time value (QuTime) -- from there you can build various timecode
 ** representations.
 ** 
 ** To the contrary, the purpose of this header is to help with debugging,
 ** writing unit tests and similar diagnostic activities.
 ** 
 ** @see timevalue.hpp
 ** @see lumitime-test.cpp
 **
 */





#ifndef LIB_TIME_DIAGNOSTICS_H
#define LIB_TIME_DIAGNOSTICS_H

#include "lib/time/timevalue.hpp"
#include "lib/time.h"

#include <iostream>
#include <string>


namespace lib {
namespace time {
  
  
  /** writes time value, formatted as HH:MM:SS:mmm
   *  @see lumiera_tmpbuf_print_time  */
  inline std::ostream&
  operator<< (std::ostream& os, TimeValue const& t)
  {
    return os << std::string(Time(t));
  }
  
  
  /* === H:M:S:mm component diagnostics === */
  
  inline int
  getHours (TimeValue const& t)
  {
    return lumiera_time_hours (_raw(t));
  }
  
  inline int
  getMins (TimeValue const& t)
  {
    return lumiera_time_minutes (_raw(t));
  }
  
  inline int
  getSecs (TimeValue const& t)
  {
    return lumiera_time_seconds (_raw(t));
  }
  
  inline int
  getMillis (TimeValue t)
  {
    return lumiera_time_millis (_raw(t));
  }
  
  
  
}} // lib::time
#endif
