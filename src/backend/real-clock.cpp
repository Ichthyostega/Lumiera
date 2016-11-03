/*
  Timings  -  timing specifications for a frame quantised data stream

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file real-clock.cpp
 ** TODO real-clock.cpp
 */


#include "backend/real-clock.hpp"

#include <ctime>


namespace backend {
  
#define MICRO_TICS_PER_NANOSECOND (1000*1000*1000 / GAVL_TIME_SCALE)
  
  
  
  TimeValue
  RealClock::_readSystemTime()
  {
    timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
                                      ////////////////////////////////////////////TODO shouldn't that be CLOCK_MONOTONIC ?
                                      ////////////////////////////////////////////TODO (what happens on ntp adjustments?)
                                      ////////////////////////////////////////////TICKET #886
    
    gavl_time_t ticksSince1970 = now.tv_sec * GAVL_TIME_SCALE
                               + now.tv_nsec / MICRO_TICS_PER_NANOSECOND; 
    
    ENSURE (ticksSince1970 == Time::limited (ticksSince1970));
    return TimeValue::buildRaw_(ticksSince1970);  // bypassing the limit check
  }
  
  
  
} // namespace backend
