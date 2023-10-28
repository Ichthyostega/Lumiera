/*
  REAL-CLOCK.hpp  -  convenience front-end to access the system clock

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

*/

/** @file real-clock.hpp
 ** Front-end for simplified access to the current wall clock time.
 ** The implementation relies on Lumiera vault functions to access the
 ** system clock with a sufficient level of precision. The result is
 ** delivered in lumiera's [internal time format](\ref lib::time::Time)
 ** 
 ** @todo this might be a good candidate also to provide some kind of
 **       translation service, i.e. a grid to anchor a logical time value
 **       with actual running wall clock time.
 ** 
 ** @see lib/time/timevalue.hpp
 */


#ifndef VAULT_REAL_CLOCK_H
#define VAULT_REAL_CLOCK_H


#include "lib/error.hpp"
#include "lib/time/timevalue.hpp"

namespace vault {

  using lib::time::Time;
  using lib::time::TimeValue;
  using lib::time::Offset;
  
  
  /**
   * Convenience frontend to access the current raw system time
   */
  class RealClock
    {
      
      
    public:
      
      static Time
      now()
        {
          return Time{_readSystemTime()};
        }
      
      static bool
      wasRecently (Time event)
        {
          Offset past{event, now()};
          return Time::ZERO <= past
             and past < CONSIDERED_RECENT;
        }
      
      
    private:
      static TimeValue _readSystemTime();
      static const Offset CONSIDERED_RECENT;
    };
  
  
  
} // namespace vault
#endif
