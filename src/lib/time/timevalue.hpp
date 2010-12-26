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
#include <string>

extern "C" {
#include <stdint.h>
#include <gavl/gavltime.h>
}


namespace lib {
namespace time {
  
  using lumiera::Time;
  
  
  /**
   * fixed format time specification.
   * 
   * @todo WIP-WIP-WIP
   */
  class TimeValue
    : boost::totally_ordered<Time,
      boost::totally_ordered<Time, gavl_time_t> >
    {
    protected:
      gavl_time_t t_;
      
    public:
      static const Time MAX ; 
      static const Time MIN ;
      
      explicit 
      TimeValue (gavl_time_t val=0)
        : t_(val)
        { }
      
      // using standard copy operations
      
      operator gavl_time_t ()  const { return t_; }
      
      // Supporting totally_ordered
      friend bool operator<  (TimeValue const& t1, TimeValue const& t2)  { return t1.t_ <  t2.t_; }
      friend bool operator<  (TimeValue const& t1, gavl_time_t t2)       { return t1.t_ <  t2   ; }
      friend bool operator>  (TimeValue const& t1, gavl_time_t t2)       { return t1.t_ >  t2   ; }
      friend bool operator== (TimeValue const& t1, TimeValue const& t2)  { return t1.t_ == t2.t_; }
      friend bool operator== (TimeValue const& t1, gavl_time_t t2)       { return t1.t_ == t2   ; }
    };
  
  
  class TimeVar
    : public TimeValue
    , boost::additive<TimeVar>
    {
        
    public:
      TimeVar (TimeValue time)
        : TimeValue(time)
        { }
      
      
      // Supporting additive
      TimeVar& operator+= (TimeVar const& tx)  { t_ += tx.t_; return *this; }
      TimeVar& operator-= (TimeVar const& tx)  { t_ -= tx.t_; return *this; }
      
      // Supporting multiplication with integral factor
      TimeVar& operator*= (int64_t fact)       { t_ *= fact;  return *this; }
       
      // baseclass TimeValue is already totally_ordered 
    };
  
  
  class Offset;  
    
  /**
   * Lumiera's internal time value datatype
   */
//class Time
//  : public TimeValue
//  {
//  public:
//    explicit 
//    Time (TimeValue val=0)
//      : TimeValue(val)
//      { }
//    
//    Time ( long millis
//         , uint secs 
//         , uint mins =0
//         , uint hours=0
//         );
//  };
  
  
  class Offset
    : public TimeValue
    {
        
    public:
      explicit 
      Offset (TimeValue distance)
        : TimeValue(distance)
        { }
    };
      
  inline Offset
  operator- (Time const& end, Time const& start)
  {
//  TimeVar distance(end);
//  distance -= start;
//  return Offset(distance);
  }
    
  typedef const Offset TimeDistance;
  
  
  class Duration
    : public TimeValue
    {
      // always positive
    };
  
  
  class TimeSpan
    : public Time
    {
      Duration dur_;
      
    public:
      TimeSpan(Time start, Duration length)
        : Time(start)
        , dur_(length)
        { }
    };
  
  
}} // lib::time
#endif
