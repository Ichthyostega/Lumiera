/*
  UTIL-QUANT.hpp  -  helper functions to deal with quantisation and comparison

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file util-quant.hpp
 ** Utilities for quantisation (grid alignment) and comparisons.
 */


#ifndef LIB_UTIL_QUANT_H
#define LIB_UTIL_QUANT_H

#include <cstdlib>
#include <climits>
#include <cfloat>
#include <cmath>



namespace util {
  
  template<typename N>
  inline bool constexpr
  isPow2 (N n)
    {
      return n > 0 and !(n & (n-1));
    };    // at each power of 2, a new bit is set for the first time
  
  
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
  
  template<typename I>
  inline IDiv<I>
  iDiv (I num, I den)  ///< support type inference and auto typing...
  {
    return IDiv<I>{num,den};
  }
  
  
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
   * @todo 3/2024 seems we have solved this problem several times meanwhile /////////////////////////////////TICKET #1360 sort out floating-point rounding and precision
   */
  inline bool
  almostEqual (double d1, double d2, unsigned int ulp =2)
  {
    using std::fabs;
    return fabs (d1-d2) < DBL_EPSILON * fabs (d1+d2) * ulp
        || fabs (d1-d2) < DBL_MIN; // special treatment for subnormal results
  }
  
  
  
  /**
   * Integral binary logarithm (disregarding fractional part)
   * @return index of the largest bit set in `num`; -1 for `num==0`
   * @todo C++20 will provide `std::bit_width(i)` — run a microbenchmark!
   * @remark The implementation uses an unrolled loop to break down the given number
   *         in a logarithmic search, subtracting away the larger powers of 2 first.
   *         Explained 10/2021 by user «[ToddLehman]» in this [stackoverflow].
   * @note Microbenchmarks indicate that this function and `std::ilogb(double)` perform
   *         in the same order of magnitude (which is surprising). This function gets
   *         slightly faster for smaller data types. The naive bitshift-count implementation
   *         is always significantly slower (8 times for int64_t, 1.6 times for int8_t)
   * @see Rational_test::verify_intLog2()
   * @see ZoomWindow_test
   * 
   * [ToddLehman]: https://stackoverflow.com/users/267551/todd-lehman
   * [stackoverflow]: https://stackoverflow.com/a/24748637 "How to do an integer log2()"
   */
  template<typename I>
  inline constexpr int
  ilog2 (I num)
  {
    if (num <= 0)
      return -1;
    const I MAX_POW = sizeof(I)*CHAR_BIT - 1;
    int logB{0};
    auto remove_power = [&](I pow) constexpr
                          {
                            if (pow > MAX_POW) return;
                            if (num >= I{1} << pow)
                              {
                                logB += pow;
                                num >>= pow;
                              }
                          };
    remove_power(32);
    remove_power(16);
    remove_power (8);
    remove_power (4);
    remove_power (2);
    remove_power (1);
    
    return logB;
  }
  
  
} // namespace util
#endif /*UTIL_QUANT_H*/
