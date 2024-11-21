/*
  INTEGRAL.hpp  -  there is nothing like one

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
using uint  = unsigned int;
using uchar = unsigned char;
using ulong = unsigned long int;
using llong = long long int;
using ullong = unsigned long long int;
using ushort = unsigned short int;

using f128  = long double;
static_assert(10 <= sizeof(f128));

const uint LIFE_AND_UNIVERSE_4EVER = 42;

  
#endif /*LIB_INTEGRAL_H*/
