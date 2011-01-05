/*
  Time  -  convenience wrapper for working with gavl_time in C++

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


#include "lib/lumitime.hpp"
#include "lib/time/timevalue.hpp"

extern "C" {
#include "lib/time.h"
}

#include <limits>
#include <string>
#include <boost/rational.hpp>

using std::string;


namespace lib {
namespace time {
  
  namespace {
    /** implementation detail: convert a rational number denoting fractionalSeconds
     *  into the internal time scale used by GAVL and Lumiera internal time values.
     */
    inline gavl_time_t
    rational2time (FSecs const& fractionalSeconds)
    {
      return boost::rational_cast<gavl_time_t> (GAVL_TIME_SCALE * fractionalSeconds);
    }
  }
  
  
  const Time Time::MAX ( TimeValue (+std::numeric_limits<gavl_time_t>::max()) );
  const Time Time::MIN ( TimeValue (-std::numeric_limits<gavl_time_t>::max()) );
  
  
  /** convenience constructor to build an
   *  internal Lumiera Time value from the usual parts
   *  of an sexagesimal time specification. Arbitrary integral
   *  values are acceptable and will be summed up accordingly.
   *  The minute and hour part can be omitted.
   * @warning internal Lumiera time values refer to an
   *         implementation dependent time origin/scale.
   *         The given value will be used as-is, without
   *         any further adjustments.
   */
  Time::Time ( long millis
             , uint secs 
             , uint mins
             , uint hours
             )
    : TimeValue(lumiera_build_time (millis,secs,mins,hours))
  { }
  
  
  /** convenience constructor to build an Time value
   *  from a fraction of seconds, given as rational number.
   *  An example would be to the time unit of a framerate.
   */
  Time::Time (FSecs const& fractionalSeconds)
    : TimeValue(rational2time (fractionalSeconds))
    { }
  
  
  /** displaying an internal Lumiera Time value
   *  for diagnostic purposes or internal reporting.
   * @warning internal Lumiera time values refer to an
   *         implementation dependent time origin/scale.
   * @return string rendering of the actual, underlying
   *         implementation value, as \c h:m:s:ms
   */
  Time::operator string()  const
  {
    return string (lumiera_tmpbuf_print_time (t_));
  }
  
  TimeVar::operator string()  const
  {
    return string (lumiera_tmpbuf_print_time (t_));
  }
  
  
  
}} // namespace lib::Time

///////////////////////////////////////////////////////////////////////////TODO leftover of the existing/initial lumitime-Implementation
namespace lumiera {
  
  
  const Time Time::MAX ( +std::numeric_limits<int64_t>::max() );
  const Time Time::MIN ( -std::numeric_limits<int64_t>::max() );
  
  
  
  Time::Time ( long millis
             , uint secs 
             , uint mins
             , uint hours
             )
    : t_(lumiera_build_time (millis,secs,mins,hours))
  { }
  
  int
  Time::getMillis() const
  {
    return lumiera_time_millis(t_);
  }
  
  
  int
  Time::getSecs()   const
  {
    return lumiera_time_seconds(t_);
  }
  
  
  int
  Time::getMins()   const
  {
    return lumiera_time_minutes(t_);
  }
  
  
  int
  Time::getHours()  const
  {
    return lumiera_time_hours(t_);
  }
  
  int
  Time::getFrames()  const
  {
    // TODO
    return 0;
  }
  
  
  Time::operator string()  const
  {
    return string (lumiera_tmpbuf_print_time (t_));
  }
  
} // namespace lumiera
