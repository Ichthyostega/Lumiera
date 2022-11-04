/*
  RATIONAL.hpp  -  support for precise rational arithmetics

  Copyright (C)         Lumiera.org
    2022,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file rational.hpp
 ** Rational number support, based on `boost::rational`.
 ** As an extension to integral arithmetics, rational numbers can be defined
 ** as a pair (numerator, denominator); since most calculations imply multiplication
 ** by common factors, each calculation will be followed by normalisation to greatest
 ** common denominator, to keep numbers within value range. Obviously, this incurs
 ** a significant performance penalty — while on the other hand allowing for lossless
 ** computations on fractional scales, which can be notoriously difficult to handle
 ** with floating point numbers. The primary motivation for using this number format
 ** is for handling fractional time values properly, e.g 1/30 sec or 1/44100 sec.
 ** 
 ** The underlying implementation from boost::rational can be parametrised with various
 ** integral data types; since our time handling is based on 64bit integers, we mainly
 ** use the specialisation `boost::rational<int64_t>`.
 ** 
 ** @note all compatible integral types can be automatically converted to rational
 **       numbers, which is a lossless conversion. The opposite is not true: to get
 **       a "ordinary" number — be it integral or floating point — an explicit
 **       conversion using `rational_cast<NUM> (fraction)` is necessary, which
 **       performs the division of `numerator/denominator` in the target value domain.
 ** 
 ** @see zoom-window.hpp
 ** @see timevalue.hpp
 */


#ifndef LIB_RATIONAL_H
#define LIB_RATIONAL_H


#include <stdint.h>
#include <boost/rational.hpp>



namespace util {
  
  using Rat = boost::rational<int64_t>;
  using boost::rational_cast;
  
} // namespace util


/** 
 * user defined literal for constant rational numbers.
 * \code
 * Rat twoThirds = 2_r/3;
 * \endcode 
 */
inline util::Rat
operator""_r (unsigned long long num)
{
  return util::Rat{num};
}


#endif /*LIB_RATIONAL_H*/
