/*
  LUMITIME.hpp  -  convenience wrapper for working with gavl_time in C++

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

*/


#ifndef LUMIERA_LUMITIME_H
#define LUMIERA_LUMITIME_H

#include <boost/operators.hpp>
#include <string>

extern "C" {
#include <stdint.h>
#include <gavl/gavltime.h>
}

namespace lumiera {


  /**
   * Lumiera's internal time data.
   * Time denotes a time point, specified as opaque value on an 
   * quasi continuous ("sufficiently precise") internal time scale, relative
   * to an (implicit) timeline zero point. The actual implementation relies
   * on gavl_time_t (long) values.
   * 
   * @see lib::time::TimeVar for an number-like time value usable for calculations
   * 
   * @todo it is not clear to which degree Time is mutable...
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
      int getFrames () const;

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
