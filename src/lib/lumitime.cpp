/*
  Time  -  convenience wrapper for working with gavl_time in C++
 
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
 
* *****************************************************/


#include "lib/lumitime.hpp"
extern "C" {
#include "lib/time.h"
}

#include <limits>
#include <string>

using std::string;


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
