/*
  Time  -  Lumiera time handling foundation

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>
    2010,               Stefan Kangas <skangas@skangas.se>
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

* *****************************************************/


/** @file time.cpp
 ** Lumiera time handling core implementation unit.
 ** This translation unit generates code for the Lumiera internal time wrapper,
 ** based on gavl_time_t, associated constants, marker classes for the derived
 ** time entities (TimeVar, Offset, Duration, TimeSpan, FrameRate) and for the
 ** basic time and frame rate conversion functions.
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

extern "C" {
#include "lib/tmpbuf.h"
}

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
  
  
  const gavl_time_t TimeValue::SCALE = GAVL_TIME_SCALE;
  
  
  /** @note the allowed time range is explicitly limited to help overflow protection */
  const Time Time::MAX ( TimeValue::buildRaw_(+std::numeric_limits<gavl_time_t>::max() / 30) );
  const Time Time::MIN ( TimeValue::buildRaw_(-_raw(Time::MAX)                             ) );
  const Time Time::ZERO;
  
  const Time Time::ANYTIME(Time::MIN);
  const Time Time::NEVER  (Time::MAX);
  
  const Offset Offset::ZERO (Time::ZERO);
  
  const FSecs FSEC_MAX{std::numeric_limits<int64_t>::max() / lib::time::TimeValue::SCALE};
  
  Literal DIAGNOSTIC_FORMAT{"%s%01d:%02d:%02d.%03d"};

  
/** scale factor _used locally within this implementation header_.
 *  GAVL_TIME_SCALE rsp. TimeValue::SCALE is the correct factor or dividend when using
 *  gavl_time_t for display on a scale with seconds. Since we want to use milliseconds,
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
    gavl_time_t time = t_;
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
    gavl_time_t time = t_;
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
  TimeValue::buildRaw_ (gavl_time_t raw)
  {
    return reinterpret_cast<TimeValue const&> (raw);
  }
  
  
  
  /** predefined constant for PAL framerate */
  const FrameRate FrameRate::PAL  (25);
  const FrameRate FrameRate::NTSC (30000,1001);
  
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
    gavl_time_t microTicks = floordiv (distance.numerator(), distance.denominator());
    return Offset{buildRaw_(microTicks)};
  }
  
  
  Offset
  Offset::stretchedByFloatFactor (double factor)  const
  {
    double distance(this->t_);
    distance *= factor;
    gavl_time_t microTicks = floor (distance);
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


char*
lumiera_tmpbuf_print_time (gavl_time_t time)
{
  int milliseconds, seconds, minutes, hours;
  bool negative = (time < 0);
  
  if (negative)
      time = -time;
  
  time /= TIME_SCALE_MS;
  milliseconds = time % 1000;
  time /= 1000;
  seconds = time % 60;
  time /= 60;
  minutes = time % 60;
  time /= 60;
  hours = time;
  
  char *buffer = lumiera_tmpbuf_snprintf(64, lib::time::DIAGNOSTIC_FORMAT,
    negative ? "-" : "", hours, minutes, seconds, milliseconds);
  
  ENSURE(buffer != NULL);
  return buffer;
}


/// @todo this utility function could be factored out into a `FSecs` or `RSec` class  ///////////////////////TICKET #1262
gavl_time_t
lumiera_rational_to_time (FSecs const& fractionalSeconds)
{
  // avoid numeric wrap from values not representable as 64bit µ-ticks
  if (abs(fractionalSeconds) > lib::time::FSEC_MAX)
    return (fractionalSeconds < 0? -1:+1)
         * std::numeric_limits<int64_t>::max();
  
  return gavl_time_t(util::reQuant (fractionalSeconds.numerator()
                                   ,fractionalSeconds.denominator()
                                   ,lib::time::TimeValue::SCALE
                                   ));
}

gavl_time_t
lumiera_framecount_to_time (uint64_t frameCount, FrameRate const& fps)
{
  // convert to 64bit
  boost::rational<uint64_t> framerate (fps.numerator(), fps.denominator());
  
  return rational_cast<gavl_time_t> (lib::time::TimeValue::SCALE * frameCount / framerate);
}

gavl_time_t
lumiera_frame_duration (FrameRate const& fps)
{
  if (!fps)
    throw error::Logic ("Impossible to quantise to an zero spaced frame grid"
                       , error::LUMIERA_ERROR_BOTTOM_VALUE);
  
  FSecs duration = 1 / fps;
  return lumiera_rational_to_time (duration);
}


namespace { // implementation: basic frame quantisation....
  
  inline int64_t
  calculate_quantisation (gavl_time_t time, gavl_time_t origin, gavl_time_t grid)
  {
    time -= origin;
    return floordiv (time,grid);
  }
  
  inline int64_t
  calculate_quantisation (gavl_time_t time, gavl_time_t origin, uint framerate, uint framerate_divisor=1)
  {
    REQUIRE (framerate);
    REQUIRE (framerate_divisor);
    
    const int64_t limit_num = std::numeric_limits<gavl_time_t>::max() / framerate;
    const int64_t limit_den = std::numeric_limits<gavl_time_t>::max() / framerate_divisor;
    const int64_t microScale {lib::time::TimeValue::SCALE};
    
    // protect against numeric overflow
    if (abs(time) < limit_num and microScale < limit_den)
      {
        // safe to calculate "time * framerate"
        time -= origin;
        return floordiv (time*framerate, microScale*framerate_divisor);
      }
    else
      {
        // direct calculation will overflow.
        // use the less precise method instead...
        gavl_time_t frameDuration = microScale / framerate; // truncated to µs
        return calculate_quantisation (time,origin, frameDuration);
      }
  }
}


int64_t
lumiera_quantise_frames (gavl_time_t time, gavl_time_t origin, gavl_time_t grid)
{
  return calculate_quantisation (time, origin, grid);
}

int64_t
lumiera_quantise_frames_fps (gavl_time_t time, gavl_time_t origin, uint framerate)
{
  return calculate_quantisation (time, origin, framerate);
}

gavl_time_t
lumiera_quantise_time (gavl_time_t time, gavl_time_t origin, gavl_time_t grid)
{
  int64_t count = calculate_quantisation (time, origin, grid);
  gavl_time_t alignedTime = count * grid;
  return alignedTime;
}

gavl_time_t
lumiera_time_of_gridpoint (int64_t nr, gavl_time_t origin, gavl_time_t grid)
{
  gavl_time_t offset = nr * grid;
  return origin + offset;
}


gavl_time_t
lumiera_build_time(long millis, uint secs, uint mins, uint hours)
{
  gavl_time_t time = millis
                   + 1000 * secs
                   + 1000 * 60 * mins
                   + 1000 * 60 * 60 * hours;
  time *= TIME_SCALE_MS;
  return time;
}

gavl_time_t
lumiera_build_time_fps (uint fps, uint frames, uint secs, uint mins, uint hours)
{
  gavl_time_t time = 1000LL * frames/fps
                   + 1000 * secs
                   + 1000 * 60 * mins
                   + 1000 * 60 * 60 * hours;
  time *= TIME_SCALE_MS;
  return time;
}

int
lumiera_time_hours (gavl_time_t time)
{
  return time / TIME_SCALE_MS / 1000 / 60 / 60;
}

int
lumiera_time_minutes (gavl_time_t time)
{
  return (time / TIME_SCALE_MS / 1000 / 60) % 60;
}

int
lumiera_time_seconds (gavl_time_t time)
{
  return (time / TIME_SCALE_MS / 1000) % 60;
}

int
lumiera_time_millis (gavl_time_t time)
{
  return (time / TIME_SCALE_MS) % 1000;
}

int
lumiera_time_frames (gavl_time_t time, uint fps)
{
  REQUIRE (fps < uint(std::numeric_limits<int>::max()));
  return floordiv<int> (lumiera_time_millis(time) * int(fps), TIME_SCALE_MS);
}




/* ===== NTSC drop-frame conversions ===== */


namespace { // implementation helper
  
  const uint FRAMES_PER_10min = 10*60 * 30000/1001;
  const uint FRAMES_PER_1min  =  1*60 * 30000/1001;
  const uint DISCREPANCY      = (1*60 * 30) - FRAMES_PER_1min;
  
  
  /** reverse the drop-frame calculation
   * @param  time absolute time value in micro ticks
   * @return the absolute frame number using NTSC drop-frame encoding
   * @todo I doubt this works correct for negative times!!
   */
  inline int64_t
  calculate_drop_frame_number (gavl_time_t time)
  {
    int64_t frameNr = calculate_quantisation (time, 0, 30000, 1001);
    
    // partition into 10 minute segments
    lldiv_t tenMinFrames = lldiv (frameNr, FRAMES_PER_10min);
    
    // ensure the drop-frame incidents happen at full minutes;
    // at start of each 10-minute segment *no* drop incident happens,
    // thus we need to correct discrepancy between nominal/real framerate once:
    int64_t remainingMinutes = (tenMinFrames.rem - DISCREPANCY) / FRAMES_PER_1min;
    
    int64_t dropIncidents = (10-1) * tenMinFrames.quot + remainingMinutes;
    return frameNr + 2*dropIncidents;
  }
}

int
lumiera_time_ntsc_drop_frames (gavl_time_t time)
{
  return calculate_drop_frame_number(time) % 30;
}

int
lumiera_time_ntsc_drop_seconds (gavl_time_t time)
{
  return calculate_drop_frame_number(time) / 30 % 60;
}

int
lumiera_time_ntsc_drop_minutes (gavl_time_t time)
{
  return calculate_drop_frame_number(time) / 30 / 60 % 60;
}

int
lumiera_time_ntsc_drop_hours (gavl_time_t time)
{
  return calculate_drop_frame_number(time) / 30 / 60 / 60 % 24;
}

gavl_time_t
lumiera_build_time_ntsc_drop (uint frames, uint secs, uint mins, uint hours)
{
  uint64_t total_mins = 60 * hours + mins;
  uint64_t total_frames  = 30*60*60 * hours
                         + 30*60 * mins
                         + 30 * secs
                         + frames
                         - 2 * (total_mins - total_mins / 10);
  gavl_time_t result = lumiera_framecount_to_time (total_frames, FrameRate::NTSC);
  
  if (0 != result) // compensate for truncating down on conversion
    result += 1;  //  without this adjustment the frame number
  return result; //   would turn out off by -1 on back conversion
}
