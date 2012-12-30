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


#include "lib/time/timevalue.hpp"
#include "lib/time.h"
#include "lib/util-quant.hpp"

#include <limits>
#include <string>
#include <boost/rational.hpp>

using std::string;
using util::floordiv;


namespace lib {
namespace time {
  
  
  
  /** @note the allowed time range is explicitly limited to help overflow protection */
  const Time Time::MAX ( TimeValue::buildRaw_(+std::numeric_limits<gavl_time_t>::max() / 30) );
  const Time Time::MIN ( TimeValue::buildRaw_(-_raw(Time::MAX)                             ) );
  const Time Time::ZERO;
  
  const Time Time::ANYTIME(Time::MAX);
  const Time Time::NEVER  (Time::MIN);
  
  const Offset Offset::ZERO (Time::ZERO);
  
  
  
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
    : TimeValue(lumiera_rational_to_time (fractionalSeconds))
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
  
  
  /** @internal backdoor to sneak in a raw time value
   *   bypassing any normalisation and limiting */
  TimeValue
  TimeValue::buildRaw_ (gavl_time_t raw)
  {
    return reinterpret_cast<TimeValue const&> (raw);
  }
  
  
  
  /** predefined constant for PAL framerate */
  const FrameRate FrameRate::PAL  (25);
  const FrameRate FrameRate::NTSC (30000,1001);
  
  
  /** @return time span of one frame of this rate,
   *   cast into internal Lumiera time scale */
  Duration
  FrameRate::duration() const
  {
    if (0 == *this)
      throw error::Logic ("Impossible to quantise to an zero spaced frame grid"
                         , error::LUMIERA_ERROR_BOTTOM_VALUE);
    
    return Duration (1, *this);
  }
  
  
  Offset
  operator* (boost::rational<int64_t> factor, Offset const& o)
  {
    boost::rational<int64_t> distance (_raw(o));
    distance *= factor;
    gavl_time_t microTicks = floordiv (distance.numerator(), distance.denominator());
    return Offset(TimeValue(microTicks));
  }
  
  
  /** offset by the given number of frames. */
  Offset::Offset (int64_t count, FrameRate const& fps)
    : TimeValue (count?  (count<0? -1:+1) * lumiera_framecount_to_time (::abs(count), fps)
                      : _raw(Duration::NIL))
    { }
  
  /** duration of the given number of frames.
   * @note always positive; count used absolute */
  Duration::Duration (int64_t count, FrameRate const& fps)
    : TimeValue (count? lumiera_framecount_to_time (abs(count), fps) : _raw(Duration::NIL))
    { }
  
  
  /** constant to indicate "no duration" */
  const Duration Duration::NIL (Time::ZERO);
  
  
  
  
  
  
}} // namespace lib::Time
