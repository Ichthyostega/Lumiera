/*
  Time  -  unified representation of a time point, including conversion functions
 
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


#include "common/lumitime.hpp"
#include <limits>


namespace lumiera
  {

   // TODO: dummy values; should be adjusted when switching to the real time implementation provided by the backend
  
   const Time Time::MAX = +std::numeric_limits<long>::max();
   const Time Time::MIN = -std::numeric_limits<long>::max();


} // namespace lumiera
