/*
  DIAGNOSTICS.hpp  -  diagnostics and output helpers for time(code) values

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file time/diagnostics.hpp
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

#include <string>


namespace lib {
namespace time {
  
  
  
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
