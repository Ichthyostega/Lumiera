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

extern "C" {
#include <gavl/gavltime.h>
}

namespace lumiera {


  /**
   * C++ convenience wrapper representing a time value, which could denote
   * a temporal position (time point) relative to an (implicit) timeline zero
   * point, or it could represent a time interval.
   * 
   * @note this is currently (10/08) an experimental implementation to ease
   *       the time handling within C++ code. It is advisable not to use it
   *       on external interfaces (use gavl_time_t there please).
   */
  class Time 
    : boost::additive<Time,
      boost::multipliable<Time, int64_t,
      boost::totally_ordered<Time> > >
    {
      gavl_time_t t_;
      
    public:
      static const Time MAX ; 
      static const Time MIN ;
      
      Time(gavl_time_t val=0) : t_(val) {}
      operator gavl_time_t () { return t_; }
      
      
      // Supporting additive
      Time& operator+= (Time const& tx)  { t_ += tx.t_; return *this; }
      Time& operator-= (Time const& tx)  { t_ -= tx.t_; return *this; }
      
      // Supporting multiplication with integral factor
      Time& operator*= (int64_t fact)    { t_ *= fact;  return *this; }
       
      // Supporting totally_ordered
      friend bool operator<  (Time const& t1, Time const& t2)  { return t1.t_ <  t2.t_; }
      friend bool operator== (Time const& t1, Time const& t2)  { return t1.t_ == t2.t_; }
    };
  
  
  
  
} // namespace lumiera
#endif
