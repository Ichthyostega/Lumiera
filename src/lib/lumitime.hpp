/*
  LUMITIME.hpp  -  convenience wrapper for working with gavl_time in C++
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/


#ifndef LUMIERA_LUMITIME_H
#define LUMIERA_LUMITIME_H

#include <boost/operators.hpp>
#include <string>

extern "C" {
#include <gavl/gavltime.h>
}

namespace lumiera {


  /**
   * C++ convenience wrapper representing a time value, which could denote
   * a temporal position (time point) relative to an (implicit) timeline zero
   * point, or it could represent a time interval.
   * 
   * This wrapper is deliberately kept rather limited as not to be completely
   * interchangeable with and integral type. The rationale is that time values
   * should be kept separate and tagged as time values. The following is supported:
   * - conversions from / to gavl_time_t (which is effectively a int64_t)
   * - additions and subtractions of time values
   * - multiplication with an integral factor
   * - comparisons between time values and gavl_time_t values
   * 
   * @todo consider the possible extensions
   *       - parsing and pretty printing
   *       - quantising of floating point values
   *       - conversion to boost::rational
   *       - define a Framerate type
   * 
   * @note this is currently (10/08) an experimental implementation to ease
   *       the time handling within C++ code. It is advisable not to use it
   *       on external interfaces (use gavl_time_t there please).
   */
  class Time 
    : boost::additive<Time,
      boost::totally_ordered<Time,
      boost::totally_ordered<Time, gavl_time_t> > >
    {
      gavl_time_t t_;
      
    public:
      static const Time MAX ; 
      static const Time MIN ;
      
      explicit 
      Time (gavl_time_t val=0)
        : t_(val)
        { }
      
      Time ( long millis
           , uint secs 
           , uint mins =0
           , uint hours=0
           );
      
      // component access
      int getMillis () const; 
      int getSecs   () const; 
      int getMins   () const; 
      int getHours  () const; 
      
      
      operator std::string ()  const;
      operator gavl_time_t ()  const { return t_; }
      
      // Supporting additive
      Time& operator+= (Time const& tx)  { t_ += tx.t_; return *this; }
      Time& operator-= (Time const& tx)  { t_ -= tx.t_; return *this; }
      
      // Supporting multiplication with integral factor
      Time& operator*= (int64_t fact)    { t_ *= fact;  return *this; }
       
      // Supporting totally_ordered
      friend bool operator<  (Time const& t1, Time const& t2)  { return t1.t_ <  t2.t_; }
      friend bool operator<  (Time const& t1, gavl_time_t t2)  { return t1.t_ <  t2   ; }
      friend bool operator>  (Time const& t1, gavl_time_t t2)  { return t1.t_ >  t2   ; }
      friend bool operator== (Time const& t1, Time const& t2)  { return t1.t_ == t2.t_; }
      friend bool operator== (Time const& t1, gavl_time_t t2)  { return t1.t_ == t2   ; }
    };
  
  
  
  
} // namespace lumiera
#endif
