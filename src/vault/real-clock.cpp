/*
  RealClock  -  convenience front-end to access the system clock

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file real-clock.cpp
 ** Implementation of simplified access to current system time.
 ** Actually, a _steady clock_ is employed, with an unspecified base time,
 ** typically starting anew at each system boot. The micro-tick value will
 ** increase monotonously, without gaps at NTP corrections, but also without
 ** any relation to an external world time.
 */


#include "vault/real-clock.hpp"

#include <chrono>


using lib::time::FSecs;
using std::chrono::steady_clock;
using std::chrono::microseconds;
using std::chrono::floor;

namespace vault {
  
  
  
  /** events during the last ms are considered "recent" for the purpose of testing */
  const Offset RealClock::CONSIDERED_RECENT{FSecs {1,1000}};

  
  TimeValue
  RealClock::_readSystemTime()
  {
    auto now = steady_clock::now();
    auto microTicks = floor<microseconds> (now.time_since_epoch())
                        .count();
    
    ENSURE (microTicks == _raw(TimeValue{microTicks}));
    return TimeValue::buildRaw_(microTicks);        // bypassing the limit check
  }
  
  
  
} // namespace vault
