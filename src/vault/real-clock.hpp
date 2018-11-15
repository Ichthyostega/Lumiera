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
 ** delivered in lumiera's internal \link lib::time::Time time format \endlink
 **   
 ** @todo WIP-WIP-WIP 3/2012
 ** @todo this might be a good candidate also to provide some kind of
 **       translation service, i.e. a grid to anchor a logical time value
 **       with actual running wall clock time.
 ** @todo not clear if this becomes some kind of central service (singleton)
 **       or just a bunch of library routines
 ** 
 ** @see lib/time/timevalue.hpp
 */


#ifndef BACKEND_REAL_CLOCK_H
#define BACKEND_REAL_CLOCK_H


#include "lib/error.hpp"
//#include "lib/handle.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/depend.hpp"
//#include "steam/engine/buffer-provider.hpp"
//#include "lib/iter-source.hpp"
//#include "lib/sync.hpp"

//#include <string>
//#include <vector>
//#include <memory>

//namespace lib {
//namespace time{
//  class Quantiser;
//  typedef std::shared_ptr<const Quantiser> PQuant;
//}}

namespace backend {

  using lib::time::Time;
  using lib::time::TimeValue;
//using std::string;

//using std::vector;
//using std::shared_ptr;
  
  
  /**
   * Convenience frontend to access the current wall clock time
   */
  class RealClock
    {
      
      
    public:
      
      static Time
      now()
        {
          return Time(_readSystemTime());
        }
      
      
    private:
      static TimeValue _readSystemTime();
    };
  
  
  
} // namespace backend
#endif
