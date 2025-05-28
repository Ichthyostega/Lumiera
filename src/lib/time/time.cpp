/*
  Time  -  Lumiera time handling foundation

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>
     2010,            Stefan Kangas <skangas@skangas.se>
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file time.cpp
 ** Lumiera time handling core implementation unit.
 ** This translation unit generates code for the Lumiera internal time wrapper,
 ** based on 64bit integral µ-tick values, associated constants, marker classes
 ** for the derived time entities (TimeVar, Offset, Duration, TimeSpan, FrameRate)
 ** and for the basic time and frame rate conversion functions.
 ** 
 ** Client code includes either time.h (for basics and conversion functions)
 ** or timevalue.hpp (for the time entities), timequant.hpp for grid aligned
 ** time values or timecode.hpp
 ** 
 ** @see Time
 ** @see TimeValue
 ** @see Grid
 ** @see TimeValue_test
 ** @see QuantiserBasics_test
 **
 */


#include "lib/error.hpp"
#include "lib/time.h"
#include "lib/time/timevalue.hpp"
#include "lib/rational.hpp"
#include "lib/util-quant.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"

#include <math.h>
#include <limits>
#include <string>
#include <sstream>
#include <boost/rational.hpp>
#include <boost/lexical_cast.hpp>

using std::string;
using util::limited;
using util::floordiv;
using lib::time::FSecs;
using lib::time::FrameRate;
using boost::rational_cast;
using boost::lexical_cast;

#undef MAX
#undef MIN


namespace error = lumiera::error;


namespace lib {
namespace meta {
  extern const std::string FAILURE_INDICATOR;
}
namespace time {
  
  
  const raw_time_64 TimeValue::SCALE = 1'000'000;
  
  
  /** @note the allowed time range is explicitly limited to help overflow protection */
  const Time Time::MAX ( TimeValue::buildRaw_(+std::numeric_limits<raw_time_64>::max() / 30) );
  const Time Time::MIN ( TimeValue::buildRaw_(-_raw(Time::MAX)                             ) );
  const Time Time::ZERO;
  
  const Time Time::ANYTIME(Time::MIN);
  const Time Time::NEVER  (Time::MAX);
  
  const Offset Offset::ZERO (Time::ZERO);
  
  const FSecs FSEC_MAX{std::numeric_limits<int64_t>::max() / lib::time::TimeValue::SCALE};
  
  Literal DIAGNOSTIC_FORMAT{"%s%01d:%02d:%02d.%03d"};

  
/** scale factor _used locally within this implementation header_.
 *  TimeValue::SCALE (µ-ticks, i.e. 1e6) is the correct factor or dividend when using
 *  raw_time_64 for display on a scale with seconds. Since we want to use milliseconds,
 *  we need to multiply or divide by 1000 to get correct results. */
#define TIME_SCALE_MS (lib::time::TimeValue::SCALE / 1000)
  
  
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
  
  Offset::Offset (FSecs const& delta_in_secs)
    : TimeValue{buildRaw_(symmetricLimit (lumiera_rational_to_time (delta_in_secs)
                                         ,Duration::MAX))}
    { }
  
  
  /** @note recommendation is to use TCode for external representation
   *  @remarks this is the most prevalent internal diagnostics display
   *        of any "time-like" value, it is meant to be compact. */
  TimeValue::operator string()  const
  {
    raw_time_64 time = t_;
    int64_t millis, seconds;
    bool negative = (time < 0);
    
    if (negative) time = -time;
    time /= TIME_SCALE_MS;
    millis = time % 1000;
    seconds = time / 1000;
    
    return string (negative ? "-" : "")
         + (seconds>0 or time==0? lexical_cast<string> (seconds)+"s" : "")
         + (millis>0? lexical_cast<string> (millis)+"ms" : "")
         ;
  }
  
  /** display an internal Lumiera Time value
   *  for diagnostic purposes or internal reporting.
   *  Format is `-hh:mm:ss.mss`
   * @warning internal Lumiera time values refer to an
   *         implementation dependent time origin/scale.
   * @return string rendering of the actual, underlying
   *         implementation value, as `h:m:s:ms`
   */
  Time::operator string()  const
  {
    raw_time_64 time = t_;
    int millis, seconds, minutes, hours;
    bool negative = (time < 0);
    
    if (negative)
        time = -time;
    
    time /= TIME_SCALE_MS;
    millis = time % 1000;
    time /= 1000;
    seconds = time % 60;
    time /= 60;
    minutes = time % 60;
    time /= 60;
    hours = time;
    
    return util::_Fmt{string(DIAGNOSTIC_FORMAT)}
                     % (negative? "-":"")
                     % hours
                     % minutes
                     % seconds
                     % millis;
  }
  
  
  Offset::operator string()  const
  {
    return (t_< 0? "" : "∆")
         + TimeValue::operator string();
  }
  
  Duration::operator string()  const
  {
    return "≺"+TimeValue::operator string()+"≻";
  }
  
  TimeSpan::operator string()  const
  {
    return string (start())
         + string (duration());
  }
  
  
  namespace {
    template<typename RAT>
    string
    renderFraction (RAT const& frac, Literal postfx) noexcept
    try {
      std::ostringstream buffer;
      if (1 == frac.denominator() or 0 == frac.numerator())
        buffer << frac.numerator() << postfx;
      else
        buffer << frac <<postfx;
      return buffer.str();
    }
    catch(...)
    { return meta::FAILURE_INDICATOR; }
  }

  /** visual framerate representation (for diagnostics) */
  FrameRate::operator string() const
  {
    return renderFraction (*this, "FPS");
  }
  
  
  
  /** @internal backdoor to sneak in a raw time value
   *      bypassing any normalisation and limiting */
  TimeValue
  TimeValue::buildRaw_ (raw_time_64 raw)
  {
    return reinterpret_cast<TimeValue const&> (raw);
  }
  
  
  
  /** predefined constant for PAL framerate */
  const FrameRate FrameRate::PAL  (25);
  const FrameRate FrameRate::NTSC (30000,1001);
  const FrameRate FrameRate::STEP (1);
  
  const FrameRate FrameRate::HALTED (1,std::numeric_limits<int>::max());
  
  
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
  
  
  /** a rather arbitrary safety limit imposed on internal numbers used to represent a frame rate.
   * @remark rational numbers bear the danger to overflow for quite ordinary computations;
   *         we stay away from the absolute maximum by an additional safety margin of 1/1000. 
   */
  const uint RATE_LIMIT{std::numeric_limits<uint>::max() / 1024};
  
  /**
   * @internal helper to work around the limitations of `uint`.
   * @return a fractional number approximating the floating-point spec.
   * @todo imposing a quite coarse limitation. If this turns out to be
   *       a problem: we can do better, use lib::reQuant (rational.hpp)
   */
  boost::rational<uint>
  __framerate_approximation (double fps)
  {
    const double UPPER_LIMIT = int64_t(RATE_LIMIT*1024) << 31;
    const int64_t HAZARD = util::ilog2(RATE_LIMIT);
    
    double doo = limited (1.0, fabs(fps) * RATE_LIMIT + 0.5, UPPER_LIMIT);
    int64_t boo(doo);
    util::Rat quantised{boo
                       ,int64_t(RATE_LIMIT)
                       };
    
    int64_t num = quantised.numerator();
    int64_t toxic = util::ilog2(abs(num));
    toxic = util::max (0, toxic - HAZARD);
    
    int64_t base = quantised.denominator();
    if (toxic)
      {
        base = util::max (base >> toxic, 1);
        num = util::reQuant (num, quantised.denominator(), base);
      }
    return {limited (1u, num,  RATE_LIMIT)
           ,limited (1u, base, RATE_LIMIT)
           };
  }
  
  /**
   * @internal helper calculate the _count per time span_ approximately,
   *           to the precision possible to represent as fractional `uint`.
   */
  boost::rational<uint>
  __framerate_approximation (size_t cnt, Duration timeReference)
  {
    boost::rational<uint64_t> quot{cnt, _raw(timeReference)};
    if (quot.denominator() < RATE_LIMIT
        and quot.numerator() < RATE_LIMIT*1024/1e6)
      return {uint(quot.numerator()) * uint(Time::SCALE)
             ,uint(quot.denominator())
             };
    // precise computation can not be handled numerically...
    return __framerate_approximation (rational_cast<double>(quot) * Time::SCALE);
  }
  

  
  
  /** @internal stretch offset by a possibly fractional factor, and quantise into raw (micro tick) grid */
  Offset
  Offset::stretchedByRationalFactor (boost::rational<int64_t> factor)  const
  {
    boost::rational<int64_t> distance (this->t_);
    distance *= factor;
    raw_time_64 microTicks = floordiv (distance.numerator(), distance.denominator());
    return Offset{buildRaw_(microTicks)};
  }
  
  
  /** @warning loss of precision on large time values beyond double mantissa length `2^52 ≈ 4.5e15` */
  Offset
  Offset::stretchedByFloatFactor (double factor)  const
  {
    double distance(this->t_);
    distance *= factor;
    raw_time_64 microTicks = floor (distance);
    return Offset{buildRaw_(microTicks)};
  }
  
  
  /** offset by the given number of frames. */
  Offset::Offset (FrameCnt count, FrameRate const& fps)
    : TimeValue{buildRaw_(
        count? (count<0? -1:+1) * lumiera_framecount_to_time (::abs(count), fps)
             :_raw(Duration::NIL))}
    { }
  
  
  
  /** constant to indicate "no duration" */
  const Duration Duration::NIL {Time::ZERO};
  
  /** maximum possible temporal extension */
  const Duration Duration::MAX = []{
                                     auto maxDelta {Time::MAX - Time::MIN};
                                     // bypass limit check, which requires Duration::MAX
                                     return reinterpret_cast<Duration const&> (maxDelta);
                                   }();
  
  const TimeSpan TimeSpan::ALL {Time::MIN, Duration::MAX};
  
}} // namespace lib::Time

namespace util {
  string
  StringConv<lib::time::FSecs, void>::invoke (lib::time::FSecs val) noexcept
  {
    return lib::time::renderFraction (val, "sec");
  }
} // namespace util







/* ===== implementation of the C API functions ===== */


/// @todo this utility function could be factored out into a `FSecs` or `RSec` class  ///////////////////////TICKET #1262
raw_time_64
lumiera_rational_to_time (FSecs const& fractionalSeconds)
{
  // avoid numeric wrap from values not representable as 64bit µ-ticks
  if (abs(fractionalSeconds) > lib::time::FSEC_MAX)
    return (fractionalSeconds < 0? -1:+1)
         * std::numeric_limits<int64_t>::max();
  
  return raw_time_64(util::reQuant (fractionalSeconds.numerator()
                                   ,fractionalSeconds.denominator()
                                   ,lib::time::TimeValue::SCALE
                                   ));
}

raw_time_64
lumiera_framecount_to_time (uint64_t frameCount, FrameRate const& fps)
{
  // convert to 64bit
  boost::rational<uint64_t> framerate (fps.numerator(), fps.denominator());
  
  return rational_cast<raw_time_64> (lib::time::TimeValue::SCALE * frameCount / framerate);
}





raw_time_64
lumiera_build_time(long millis, uint secs, uint mins, uint hours)
{
  raw_time_64 time = millis
                   + 1000 * secs
                   + 1000 * 60 * mins
                   + 1000 * 60 * 60 * hours;
  time *= TIME_SCALE_MS;
  return time;
}

raw_time_64
lumiera_build_time_fps (uint fps, uint frames, uint secs, uint mins, uint hours)
{
  raw_time_64 time = 1000LL * frames/fps
                   + 1000 * secs
                   + 1000 * 60 * mins
                   + 1000 * 60 * 60 * hours;
  time *= TIME_SCALE_MS;
  return time;
}

int
lumiera_time_hours (raw_time_64 time)
{
  return time / TIME_SCALE_MS / 1000 / 60 / 60;
}

int
lumiera_time_minutes (raw_time_64 time)
{
  return (time / TIME_SCALE_MS / 1000 / 60) % 60;
}

int
lumiera_time_seconds (raw_time_64 time)
{
  return (time / TIME_SCALE_MS / 1000) % 60;
}

int
lumiera_time_millis (raw_time_64 time)
{
  return (time / TIME_SCALE_MS) % 1000;
}

int
lumiera_time_frames (raw_time_64 time, uint fps)
{
  REQUIRE (fps < uint(std::numeric_limits<int>::max()));
  return floordiv<int> (lumiera_time_millis(time) * int(fps), TIME_SCALE_MS);
}



namespace lib {
namespace time { ////////////////////////////////////////////////////////////////////////////////////////////TICKET #1259 : move all calculation functions into a C++ namespace
  
  
}} // lib::time
