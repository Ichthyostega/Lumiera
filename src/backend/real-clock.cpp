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


#include "backend/real-clock.hpp"
#include "lib/time/timequant.hpp"



namespace backend {
  
  
//  using lib::time::PQuant;
//  using lib::time::Time;
  
  namespace { // hidden local details of the service implementation....
    
    
  } // (End) hidden service impl details
  
  
  /** storage for the singleton accessor, holding
   *  an instance of the RealClock service */
  lib::Singleton<RealClock> RealClock::_clock;
  
  
  /** Initialise a service to retrieve system time with sufficient precision 
   */
  RealClock::RealClock ()
    { 
      UNIMPLEMENTED ("system clock service");
    }
  
  RealClock::~RealClock ()
    { 
      UNIMPLEMENTED ("disconnect the system clock service");
    }
  
  
     
  TimeValue
  RealClock::readSystemTime()
  {
    UNIMPLEMENTED ("access the system clock");
    gavl_time_t ticksSince1970 = 1261440000000000L;
    
    ENSURE (ticksSince1970 == Time::limited (ticksSince1970));
    return TimeValue::buildRaw_(ticksSince1970);  // bypassing the limit check
  }
  
  
  
} // namespace backend
