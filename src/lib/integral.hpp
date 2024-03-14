/*
  INTEGRAL.hpp  -  there is nothing like one

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file integral.h
 ** Inclusion for common place integral types and constants.
 */



#ifndef LIB_INTEGRAL_H
#define LIB_INTEGRAL_H

#include <stdlib.h>
#include <cstddef>
#include <cstdint>

/* === minimal common place === */
using uchar = unsigned char;
using uint  = unsigned int;

using f128  = long double;
static_assert(10 <= sizeof(f128));

const uint LIFE_AND_UNIVERSE_4EVER = 42;

  
#endif /*LIB_INTEGRAL_H*/
