/*
  UTIL-QUANT.hpp  -  helper functions to deal with quantisation and comparison

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file util-quant.hpp
 ** Utilities for quantisation (grid alignment) and comparisons.
 */


#ifndef LIB_UTIL_QUANT_H
#define LIB_UTIL_QUANT_H

#include <cstdlib>
#include <cfloat>
#include <cmath>



namespace util {
  
  
  /** helper to treat int or long division uniformly */
  template<typename I>
  struct IDiv
    {
      I quot;
      I rem;
      
      IDiv (I num, I den)
        : quot(num/den)
        , rem(num - quot*den)
        { }
    };
  
  template<>
  struct IDiv<int>
    : div_t
    {
      IDiv<int> (int num, int den)
        : div_t(div (num,den))
        { }
    };
  
  template<>
  struct IDiv<long>
    : ldiv_t
    {
      IDiv<long> (long num, long den)
        : ldiv_t(ldiv (num,den))
        { }
    };
  
  template<>
  struct IDiv<long long>
    : lldiv_t
    {
      IDiv<long long> (long long num, long long den)
        : lldiv_t(lldiv (num,den))
        { }
    };
  
  
  /** floor function for integer arithmetics.
   *  Unlike the built-in integer division, this function
   *  always rounds towards the _next smaller integer,_
   *  even for negative numbers.
   * @warning floor on doubles performs way better
   * @see UtilFloordiv_test
   */
  template<typename I>
  inline I
  floordiv (I num, I den)
  {
    if (0 < (num^den))
      return num/den;
    else
      { // truncate similar to floor()
        IDiv<I> res(num,den);
        return (res.rem)? res.quot-1   // negative results truncated towards next smaller int
                        : res.quot;   //..unless the division result not truncated at all
      }
  }
  
  /** scale wrapping operation.
   *  Quantises the numerator value into the scale given by the denominator.
   *  Unlike the built-in integer division, this function always rounds towards
   *  the _next smaller integer_ and also relates the remainder (=modulo) to
   *  this next lower scale grid point.
   * @return quotient and remainder packed into a struct
   * @see UtilFloorwarp_test
   */
  template<typename I>
  inline IDiv<I>
  floorwrap (I num, I den)
  {
    IDiv<I> res(num,den);
    if (0 > (num^den) && res.rem)
      {  // negative results
        //  wrapped similar to floor()
        --res.quot;
        res.rem = den - (-res.rem);
      }
    return res;
  }
  
  
  
  /**
   * epsilon comparison of doubles.
   * @remarks Floating point calculations are only accurate up to a certain degree,
   *          and we need to adjust for the magnitude of the involved numbers, since
   *          floating point numbers are scaled by the exponent. Moreover, we need
   *          to be careful with very small numbers (close to zero), where calculating
   *          the difference could yield coarse grained 'subnormal' values.
   * @param ulp number of grid steps to allow for difference (default = 2).
   *          Here, a 'grid step' is the smallest difference to 1.0 which can be
   *          represented in floating point ('units in the last place')
   * @warning don't use this for comparison against zero, rather use an absolute epsilon then.
   * @see https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
   * @see http://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
   * @see https://en.wikipedia.org/wiki/Unit_in_the_last_place
   */
  inline bool
  almostEqual (double d1, double d2, unsigned int ulp =2)
  {
    using std::fabs;
    return fabs (d1-d2) < DBL_EPSILON * fabs (d1+d2) * ulp
        || fabs (d1-d2) < DBL_MIN; // special treatment for subnormal results
  }
  
  
} // namespace util
#endif /*UTIL_QUANT_H*/
