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


#ifndef LIB_TIME_TIMEVALUE_H
#define LIB_TIME_TIMEVALUE_H

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
      boost::totally_ordered<TimeValue, gavl_time_t> >
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
      
    public:
      /** explicit limit of allowed time range */
      static gavl_time_t limited (gavl_time_t raw);
      
      
      explicit 
      TimeValue (gavl_time_t val=0)
        : t_(limited (val))
        { }
      
      /** copy initialisation allowed */
      TimeValue (TimeValue const& o)
        : t_(o.t_)
        { }
      
      /** @internal to pass Time values to C functions */
      friend gavl_time_t _raw (TimeValue const& time) { return time.t_; }
      static TimeValue buildRaw_(gavl_time_t);
      
      // Supporting totally_ordered
      friend bool operator<  (TimeValue const& t1, TimeValue const& t2)  { return t1.t_ <  t2.t_; }
      friend bool operator<  (TimeValue const& t1, gavl_time_t t2)       { return t1.t_ <  t2   ; }
      friend bool operator>  (TimeValue const& t1, gavl_time_t t2)       { return t1.t_ >  t2   ; }
      friend bool operator== (TimeValue const& t1, TimeValue const& t2)  { return t1.t_ == t2.t_; }
      friend bool operator== (TimeValue const& t1, gavl_time_t t2)       { return t1.t_ == t2   ; }
    };
  
  
  
  /** a mutable time value,
   *  behaving like a plain number,
   *  allowing copy and re-accessing
   * @note supports scaling by a factor,
   *       which \em deliberately is chosen 
   *       as int, not gavl_time_t, because the
   *       multiplying times is meaningless.
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
      
      // Allowing copy and assignment
      TimeVar (TimeVar const& o)
        : TimeValue(o)
        { }
      
      TimeVar&
      operator= (TimeValue const& o)
        {
          t_ = TimeVar(o);
          return *this;
        }
      
      /** @internal diagnostics */
      operator std::string ()  const;
      
      // Supporting mixing with plain long int arithmetics
      operator gavl_time_t ()  const { return t_; }
      
      // Supporting additive
      TimeVar& operator+= (TimeVar const& tx)  { t_ += tx.t_; return *this; }
      TimeVar& operator-= (TimeVar const& tx)  { t_ -= tx.t_; return *this; }
      
      // Supporting multiplication with integral factor
      TimeVar& operator*= (int fact)           { t_ *= fact;  return *this; }
      
      // Supporting flip sign
      TimeVar  operator-  ()         const     { return TimeVar(*this)*=-1; }
       
      // baseclass TimeValue is already totally_ordered 
    };
  
  
  
  /**
   * Offset measures a distance in time.
   * It may be used to relate two points in time,
   * or to create a modification for time-like entities.
   * Similar to (basic) time values, offsets can be compared,
   * but are otherwise opaque and immutable. Yet they allow
   * to build derived values, including
   * - the \em absolute (positive) distance for this offset
   * - a combined offset by chaining another offset
   */
  class Offset
    : public TimeValue
    {
        
    public:
      explicit 
      Offset (TimeValue const& distance)
        : TimeValue(distance)
        { }
      
      Offset (TimeValue const& origin, TimeValue const& target)
        : TimeValue(TimeVar(target) -= origin)
        { }
      
      TimeValue
      abs()  const
        {
          return TimeValue(std::llabs (t_));
        }
      
      Offset
      operator+ (Offset const& toChain)  const
        {
          TimeVar distance(*this);
          distance += toChain;
          return Offset(distance);
        }
    };
  
  
  
  
  /* ======= specific Time entities ==================== */
  
  /** rational representation of fractional seconds
   * @warning do not mix up gavl_time_t and FSecs */
  typedef boost::rational<long> FSecs;

  /**
   * Lumiera's internal time value datatype.
   * This is a TimeValue, but now more specifically denoting
   * a point in time, measured in reference to an internal
   * (opaque) time scale.
   * 
   * Lumiera Time provides some limited capabilities for
   * direct manipulation; Time values can be created directly
   * from \c (h,m,s,ms) specification and there is an string
   * representation intended for internal use (reporting and
   * debugging). Any real output, formatting and persistent
   * storage should be based on the (quantised) timecode
   * formats though, which can be generated from time values.
   * 
   * Non constant Time objects can receive an encapsulated
   * \em mutation message, which is also the basis for
   * changing time spans, durations and for re-aligning
   * quantised values to some other grid.
   * 
   * @todo define these mutations
   */
  class Time
    : public TimeValue
    {
    public:
      static const Time MAX ; 
      static const Time MIN ;
      
      explicit 
      Time (TimeValue const& val =TimeValue(0))
        : TimeValue(val)
        { }
      
      Time (TimeVar const& calcResult)
        : TimeValue(calcResult)
        { }
      
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
    };
  
  
  
  /**
   * Duration is the internal Lumiera time metric.
   * It is an absolute (positive) value, but can be
   * promoted from an offset. Similar to Time,
   * Duration can receive mutation messages.
   * 
   * @todo define these mutations
   */
  class Duration
    : public Offset
    {
    public:
      Duration (Offset const& distance)
        : Offset(distance.abs())
        { }
    };
  
  
  
  /**
   * A time interval anchored at a specific point in time.
   * The start point of this timespan is also its nominal
   * position, and the end point is normalised to happen
   * never before the start point. A TimeSpan is enough
   * to fully specify the temporal properties of an
   * object within the model.
   * 
   * Similar to Time and Duration, a TimeSpan may
   * also receive an (opaque) mutation message.
   */
  class TimeSpan
    : public Time
    {
      Duration dur_;
      
    public:
      TimeSpan(Time start, Duration length)
        : Time(start)
        , dur_(length)
        { }
                                                ///////////TODO creating timespans needs to be more convenient....
      
      operator Duration()  const 
        {
          return dur_;
        }
      
      Time
      getEnd()  const
        {
          TimeVar startPoint (*this);
          return (startPoint + dur_);
        }
    };
  
  
  
  
  /* == implementations == */
  
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
  
  
}} // lib::time
#endif
