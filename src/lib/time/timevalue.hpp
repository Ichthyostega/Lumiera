/*
  TIMEVALUE.hpp  -  basic definitions for time values and time intervals

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file timevalue.hpp
 ** a family of time value like entities and their relationships.
 ** This is the foundation for the Lumiera time handling framework. On the implementation
 ** level, time values are represented as 64bit integer values `gavl_time_t`. But for the
 ** actual use, we create several kinds of time "values", based on their logical properties.
 ** These time values are considered to be fixed (immutable) values, which may only be
 ** created through some limited construction paths, and any time based calculation
 ** is forced to go through our time calculation library. This is prerequisite for
 ** the definition of _frame aligned_ time values and time code representation
 ** implemented as display format based on these frame quantised time values.
 ** 
 ** # Time entities
 ** 
 ** The value types defined in this header represent time points and time intervals
 ** based on an internal time scale (µs ticks) and not related to any known fixed time
 ** zone or time base; rather they are interpreted in usage context, and the intended
 ** way to retrieve such a value is by formatting it into a time code format.
 ** 
 ** The lib::time::TimeValue serves as foundation for all further time calculations;
 ** in fact it is implemented as a single 64bit µ-tick value (`gavl_time_t`). The
 ** further time entities are implemented as value objects (without virtual functions):
 ** - lib::time::Time represents a time instant and is the reference for any usage
 ** - lib::time::TimeVar is a mutable time variable and can be used for calculations
 ** - lib::time::Offset can be used to express a positive or negative shift on time scale
 ** - lib::time::Duration represents the extension or an amount of time
 ** - lib::time::TimeSpan represents a distinct interval, with start time and duration
 ** - lib::time::FrameRate can be used to mark a number to denote a frames-per-second spec
 ** - lib::time::FSecs is a rational number to represent seconds or fractions thereof
 ** 
 ** # Manipulating time values
 ** 
 ** Time values are conceived as fixed, immutable entities, similar to numbers; you can't
 ** just change the number two, and likewise, two seconds are two seconds. However, for
 ** many use cases we have to combine time values to perform calculations
 ** - Time entities can be combined with operators, to form new time entities
 ** - the TimeVar can be used as accumulator or variable for ongoing calculations
 ** - since TimeSpan, Duration (and the grid-aligned, "quantised" flavours) will often
 **   represent some time-like property or entity, e.g. the temporal specification of
 **   a media Clip with start and duration, there is the concept of an explicit *mutation*,
 **   which is _accepted_ by these entities. Notably the lib::time::Control can be attached
 **   to these entities, and can then receive manipulations (nudging, offset); moreover it
 **   is possible to attach as listener to such a "controller" and be notified by any
 **   manipulation; this setup is the base for running time display, playback cursors etc.
 ** 
 ** # Quantised time
 ** 
 ** While these _internal time values_ can be considered _sufficiently precise,_ in practice
 ** any time specifications in the context of media handling will be aligned to some grid,
 ** and expressed in a _time code format._ Typically, we want to know the number of frames
 ** since playback started at the beginning of the timeline, and such a specification also
 ** relies on some implicitly known _frame rate_ (24fps for film in US, 25fps for film and
 ** TV in Europe, ...). By _deliberate choice,_ in Lumiera we *do not incorporate* such
 ** implicit assumptions into the actual time values. Rather, they need to be made explicitly
 ** in the relevant usage context. This is also the reason why the time entities defined in
 ** this header _do not offer an API_ to get the "real" time (whatever this means). Rather,
 ** the user of these time entities should get used to the concept that these abstract
 ** opaque values are the real thing, and a concrete, human readable time code is only
 ** a derivation, and any such derivation also incurs information loss. To reiterate that,
 ** _any time quantisation is a lossy information;_ grid aligned values are not "cleaner",
 ** they are just easier to handle for humans.
 ** 
 ** \par how can I extract a time value?
 ** Taking the aforementioned into account, it depends on the context what to expect and to get
 ** - the standard path is to create a lib::time::QuTime by associating the internal time value
 **   with a pre-defined _time grid._ From there you can call QuTime::formatAs() to build an
 **   actual timecode instance, which can then be investigated or just printed.
 ** - for debugging purpose, lib::time::Time defines an `operator string()`, which breaks down
 **   the internal values into the format `-hh:mm:ss.mss`
 ** - advanced calculations with the need to access the implementation data in full precision
 **   should go through lib::time::TimeVar, which offers conversions to raw `int64_t` and the
 **   even more fine grained `FSec`, which is a rational (fraction) `boost::rational<int64_t>`
 ** 
 ** @see time.h basic time calculation library functions
 ** @see timequant.hpp
 ** @see TimeValue_test
 ** 
 */


#ifndef LIB_TIME_TIMEVALUE_H
#define LIB_TIME_TIMEVALUE_H

#include "lib/error.hpp"

#include <boost/operators.hpp>
#include <boost/rational.hpp>
#include <cstdlib>
#include <string>

extern "C" {
#include <stdint.h>
#include <gavl/gavltime.h>
}


namespace lib {
namespace time {
  
  namespace error = lumiera::error;
  
  // forwards...
  class FrameRate;
  class TimeSpan;
  class Mutation;
  
  
  /**
   * basic constant internal time value.
   * These time values provide the implementation base
   * for all further time types. They can be created by
   * wrapping up a raw micro tick value (gavl_time_t),
   * are totally ordered, but besides that,
   * they are opaque and non-mutable.
   * @note clients should prefer to use Time instances,
   *       which explicitly denote an Lumiera internal
   *       time value and are easier to use.
   * @see TimeVar when full arithmetics are required
   */
  class TimeValue
    : boost::totally_ordered<TimeValue,
      boost::totally_ordered<TimeValue, gavl_time_t>>
    {
    protected:
      /** the raw (internal) time value
       *  used to implement the time types */
      gavl_time_t t_;
      
      
      /** Assigning of time values is not allowed,
       *  but derived classed might allow that */
      TimeValue&
      operator= (TimeValue const& o)
        {
          t_ = o.t_;
          return *this;
        }
      
      /** some subclasses may receive modification messages */
      friend class Mutation;
      
    public:
      /** Number of micro ticks (µs) per second as basic time scale */
      static const gavl_time_t SCALE;
      
      /** explicit limit of allowed time range */
      static gavl_time_t limited (gavl_time_t raw);
      
      
      explicit
      TimeValue (gavl_time_t val=0)       ///< time given in µ ticks here
        : t_(limited (val))
        { }
      
      /** copy initialisation allowed */
      TimeValue (TimeValue const& o)
        : t_(o.t_)
        { }
      
      /** @internal to pass Time values to C functions */
      friend gavl_time_t _raw (TimeValue const& time) { return time.t_; }
      static TimeValue buildRaw_(gavl_time_t);
      
      /** @internal diagnostics */
      operator std::string ()  const;
      
      // Supporting totally_ordered
      friend bool operator<  (TimeValue const& t1, TimeValue const& t2)  { return t1.t_ <  t2.t_; }
      friend bool operator<  (TimeValue const& t1, gavl_time_t t2)       { return t1.t_ <  t2   ; }
      friend bool operator>  (TimeValue const& t1, gavl_time_t t2)       { return t1.t_ >  t2   ; }
      friend bool operator== (TimeValue const& t1, TimeValue const& t2)  { return t1.t_ == t2.t_; }
      friend bool operator== (TimeValue const& t1, gavl_time_t t2)       { return t1.t_ == t2   ; }
    };
  
  
  
  
  
  
  
  /* ======= specific Time entities ==================== */
  
  /** relative framecount or frame number.
   *  Used within the engine at places where the underlying
   *  grid and origin is obvious from the call context.
   * @warning do not mix up gavl_time_t and FrameCnt.
   * @warning use 64bit consistently.
   *          beware: `long` is 32bit on i386
   * @note any conversion to frame numbers should go through
   *       time quantisation followed by conversion to FrameNr
   */
  using FrameCnt = int64_t;
  
  /** rational representation of fractional seconds
   * @warning do not mix up gavl_time_t and FSecs */
  using FSecs = boost::rational<int64_t>;
  
  
  
  /** a mutable time value,
   *  behaving like a plain number,
   *  allowing copy and re-accessing
   * @note supports scaling by a factor,
   *       which _deliberately_ is chosen
   *       as int, not gavl_time_t, because the
   *       multiplying of times is meaningless.
   */
  class TimeVar
    : public TimeValue
    , boost::additive<TimeVar,
      boost::additive<TimeVar, TimeValue,
      boost::multipliable<TimeVar, int>
      > >
    {
    public:
      TimeVar (TimeValue const& time = TimeValue())
        : TimeValue(time)
        { }
      
      /** Allow to pick up precise fractional seconds
       * @warning truncating fractional µ-ticks  */
      TimeVar (FSecs const&);
      
      /// Allowing copy and assignment
      TimeVar (TimeVar const& o)
        : TimeValue(o)
        { }
      
      TimeVar&
      operator= (TimeValue const& o)
        {
          t_ = TimeVar(o);
          return *this;
        }
      
      /// Support mixing with plain long int arithmetics
      operator gavl_time_t()  const { return t_; }
      /// Support for micro-tick precise time arithmetics
      operator FSecs()  const { return FSecs{t_, TimeValue::SCALE}; }
      
      /// Supporting additive
      TimeVar& operator+= (TimeVar const& tx)  { t_ += tx.t_; return *this; }
      TimeVar& operator-= (TimeVar const& tx)  { t_ -= tx.t_; return *this; }
      
      /// Supporting multiplication with integral factor
      TimeVar& operator*= (int64_t fact)       { t_ *= fact;  return *this; }
      
      /// Supporting sign flip
      TimeVar  operator-  ()         const     { return TimeVar(*this)*=-1; }
       
      // baseclass TimeValue is already totally_ordered
    };
  
  
  
  
  /**********************************************************//**
   * Lumiera's internal time value datatype.
   * This is a TimeValue, but now more specifically denoting
   * a point in time, measured in reference to an internal
   * (opaque) time scale.
   * 
   * Lumiera Time provides some limited capabilities for
   * direct manipulation; Time values can be created directly
   * from `(ms,sec,min,hour)` specification and there is an
   * string representation intended for internal use (reporting
   * and debugging). Any real output, formatting and persistent
   * storage should be based on the (quantised) timecode
   * formats though, which can be generated from time values.
   * 
   * Similar to TimeValue, also Time objects are considered
   * immutable values. As convenience shortcut, some operators
   * are provided, creating a TimVar for further calculations.
   */
  class Time
    : public TimeValue
    {
      /// direct assignment prohibited
      Time& operator= (Time const);
      
      /// suppress possible direct conversions
      Time(int);
      
    public:
      static const Time MAX ;
      static const Time MIN ;
      static const Time ZERO;
      
      static const Time ANYTIME;  ///< border condition marker value. #ANYTIME <= any time value
      static const Time NEVER;   ///<  border condition marker value. #NEVER >= any time value
      
      explicit
      Time (TimeValue const& val =TimeValue(0))
        : TimeValue(val)
        { }
      
      Time (TimeVar const& calcResult)
        : TimeValue(calcResult)
        { }
      
      explicit
      Time (FSecs const& fractionalSeconds);
      
      Time ( long millis
           , uint secs
           , uint mins =0
           , uint hours=0
           );
      
      /** @internal diagnostics */
      operator std::string ()  const;
      
      /** convenience start for time calculations */
      TimeVar operator+ (TimeValue const& tval)  const { return TimeVar(*this) + tval; }
      TimeVar operator- (TimeValue const& tval)  const { return TimeVar(*this) - tval; }
      TimeVar operator- ()                       const { return -TimeVar(*this); }
    };
  
  
  
  
  /**
   * Offset measures a distance in time.
   * It may be used to relate two points in time,
   * or to create a modification for time-like entities.
   * Similar to (basic) time values, offsets can be compared,
   * but are otherwise opaque and immutable. Yet they allow
   * to build derived values, including
   * - the _absolute (positive) distance_ for this offset: #abs
   * - a combined offset by chaining another offset
   */
  class Offset
    : public TimeValue
    {
    protected:
      /** generally immutable,
       *  but derived classes allow some limited mutation
       *  through special API calls */
      Offset&
      operator= (Offset const& o)
        {
          TimeValue::operator= (o);
          return *this;
        }
      
    public:
      explicit
      Offset (TimeValue const& distance =Time::ZERO)
        : TimeValue(distance)
        { }
      
      Offset (TimeValue const& origin, TimeValue const& target)
        : TimeValue(TimeVar(target) -= origin)
        { }
      
      Offset (FrameCnt count, FrameRate const& fps);
      
      static const Offset ZERO;
      
      
      TimeValue
      abs()  const
        {
          return TimeValue(std::llabs (t_));
        }
      
      /** @internal stretch offset by a possibly fractional factor,
       *            and quantise into raw (micro tick) grid */
      Offset stretchedByRationalFactor (boost::rational<int64_t>)  const;
      
      /** @internal diagnostics, indicating ∆ */
      operator std::string ()  const;
      
      // Supporting sign flip
      Offset operator- ()  const;
    };
  
  //-- support linear offset chaining ---------------
  
  inline Offset
  operator+ (Offset const& start, Offset const& toChain)
  {
    TimeVar distance(start);
    distance += toChain;
    return Offset(distance);
  }
  
  template<typename INT>
  inline Offset
  operator* (Offset const& distance, INT factor)
  {
    return factor*distance;
  }
  
  template<typename INT>
  inline Offset
  operator* (INT factor, Offset const& o)
  {
    TimeVar distance(o);
    distance *= factor;
    return Offset(distance);
  }
  
  template<typename INTX>
  inline Offset
  operator* (boost::rational<INTX> factor, Offset const& offset)
  {
    return offset.stretchedByRationalFactor (boost::rational<int64_t>(factor.numerator(), factor.denominator()));
  }
  
  
  /** flip offset direction */
  inline Offset
  Offset::operator- ()  const
  {
    return -1 * (*this);
  }
  
  
  
  
  /**
   * Duration is the internal Lumiera time metric.
   * It is an absolute (positive) value, but can be
   * promoted from an offset. While Duration generally
   * is treated as immutable value, there is the
   * possibility to send a _Mutation message_.
   */
  class Duration
    : public TimeValue
    {
      /// direct assignment prohibited
      Duration& operator= (Duration const&);
      
    public:
      Duration()
        : Duration(Time::ZERO)
        { }
      
      Duration (Offset const& distance)
        : TimeValue(distance.abs())
        { }
      
      explicit
      Duration (TimeValue const& timeSpec)
        : TimeValue(Offset(timeSpec).abs())
        { }
      
      explicit
      Duration (FSecs const& timeSpan_in_secs)
        : TimeValue(Offset(Time(timeSpan_in_secs)).abs())
        { }
      
      Duration (TimeSpan const& interval);
      Duration (FrameCnt count, FrameRate const& fps);
      
      Duration (Duration const& o)
        : Duration{Offset(o)}
        { }
      
      static const Duration NIL;
      
      void accept (Mutation const&);
      
      
      /** @internal diagnostics */
      operator std::string ()  const;

      /// Supporting backwards use as offset
      Offset operator- ()  const;
      
    };
    
  //-- support using a Duration to build offsets ---------------
  
  inline Duration
  operator+ (Duration const& base, Duration const& toAdd)
  {
    return Offset(base) + Offset(toAdd);
  }
  
  template<typename INT>
  inline Offset
  operator* (INT factor, Duration const& dur)
  {
    return factor * Offset(dur);
  }
  
  template<typename INT>
  inline Offset
  operator* (Duration const& dur, INT factor)
  {
    return factor*dur;
  }
  
  inline Offset
  Duration::operator- ()  const
  {
    return -1 * (*this);
  }
  
  
  
  /**
   * A time interval anchored at a specific point in time.
   * The start point of this timespan is also its nominal
   * position, and the end point is normalised to happen
   * never before the start point. A TimeSpan is enough
   * to fully specify the temporal properties of an
   * object within the model.
   * 
   * As an exception to the generally immutable Time
   * entities, a non constant TimeSpan may receive
   * _mutation messages_, both for the start point
   * and the duration. This allows for changing
   * position and length of objects in the timeline.
   * 
   * @todo define these mutations
   */
  class TimeSpan
    : public Time
    , boost::totally_ordered<TimeSpan>
    {
      Duration dur_;
      
    public:
      TimeSpan(TimeValue const& start, Duration const& length)
        : Time(start)
        , dur_(length)
        { }
      
      TimeSpan(TimeValue const& start, Offset const& reference_distance)
        : Time(start)
        , dur_(reference_distance)
        { }
      
      TimeSpan(TimeValue const& start, TimeValue const& end)
        : Time(start)
        , dur_(Offset(start,end))
        { }
      
      TimeSpan(TimeValue const& start, FSecs(duration_in_secs))
        : Time(start)
        , dur_(duration_in_secs)
        { }
      
      TimeSpan()
        : TimeSpan(Time::ZERO, Time::ZERO)
        { }
      
      
      Duration&
      duration()
        {
          return dur_;
        }
      
      Duration
      duration()  const
        {
          return dur_;
        }
      
      Time
      start()  const
        {
          return *this;
        }
      
      Time
      end()  const
        {
          return TimeVar(*this) += dur_;
        }
      
      bool
      contains (TimeValue const& tp)  const
        {
          return *this <= tp
              && tp < end();
        }
      
      /** may change start / duration */
      void accept (Mutation const&);
      
      /** @internal diagnostics */
      operator std::string ()  const;
      
      /// Supporting extended total order, based on start and interval length
      friend bool operator== (TimeSpan const& t1, TimeSpan const& t2)  { return t1.t_==t2.t_ && t1.dur_==t2.dur_; }
      friend bool operator<  (TimeSpan const& t1, TimeSpan const& t2)  { return t1.t_< t2.t_ ||
                                                                               (t1.t_==t2.t_ && t1.dur_< t2.dur_);}
    };
  
  
  /**
   * Framerate specified as frames per second.
   * Implemented as rational number.
   */
  class FrameRate
    : public boost::rational<uint>
    {
    public:
      FrameRate (uint fps) ;
      FrameRate (uint num, uint denom);
      FrameRate (boost::rational<uint> const& fractionalRate);
      
      // standard copy acceptable;
      
      double asDouble()  const;
      
      static const FrameRate PAL;
      static const FrameRate NTSC;
      
      static const FrameRate HALTED;
      
      /** duration of one frame */
      Duration duration() const;
      
      operator std::string() const;
    };
  
  /** convenient conversion to duration in fractional seconds */
  inline FSecs
  operator/ (int n, FrameRate rate)
  {
    return FSecs{ n*rate.denominator(), rate.numerator()};
  }
  
  
  
  
  /* == implementations == */
    
  namespace { // implementation helpers...
    
    template<typename NUM>
    inline NUM
    __ensure_nonzero (NUM n)
    {
      if (n == NUM{0})
        throw error::Logic ("Degenerated frame grid not allowed"
                           , error::LERR_(BOTTOM_VALUE));
      return n;
    }
  }//(End) implementation helpers
  
  
  
  /** @internal applies a limiter on the provided
   * raw time value to keep it within the arbitrary
   * boundaries defined by (Time::MAX, Time::MIN).
   * While Time entities are \c not a "safeInt"
   * implementation, we limit new values and
   * establish this safety margin to prevent
   * wrap-around during time quantisation */
  inline gavl_time_t
  TimeValue::limited (gavl_time_t raw)
  {
    return raw > Time::MAX? Time::MAX.t_
         : raw < Time::MIN? Time::MIN.t_
         :                  raw;
  }
  
  inline
  TimeVar::TimeVar (FSecs const& fractionalSeconds)
    : TimeVar{Time(fractionalSeconds)}
    { }
  
  inline
  Duration::Duration (TimeSpan const& interval)
    : TimeValue(interval.duration())
    { }
  
  inline
  FrameRate::FrameRate (uint fps)
    : boost::rational<uint> (__ensure_nonzero(fps))
    { }
  
  inline
  FrameRate::FrameRate (uint num, uint denom)
    : boost::rational<uint> (__ensure_nonzero(num), denom)
    { }
  
  inline
  FrameRate::FrameRate (boost::rational<uint> const& fractionalRate)
    : boost::rational<uint> (__ensure_nonzero(fractionalRate))
    { }
  
  inline double
  FrameRate::asDouble()  const
  {
    return boost::rational_cast<double> (*this);
  }
  
  
  
  
}} // lib::time


namespace util {
  
  inline bool
  isnil (lib::time::Duration const& dur)
  {
    return 0 == dur;
  }

  // repeated or forward declaration, see meta/util.hpp
  template<typename X, typename COND>
  struct StringConv;
  
  /** specialisation: render fractional seconds (for diagnostics) */
  template<>
  struct StringConv<lib::time::FSecs, void>
    {
      static std::string
      invoke (lib::time::FSecs) noexcept;
    };
}
#endif /*LIB_TIME_TIMEVALUE_H*/
