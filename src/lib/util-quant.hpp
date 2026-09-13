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

#include "lib/integral.hpp"

#include <type_traits>
#include <concepts>
#include <cstdlib>
#include <climits>
#include <limits>
#include <cmath>

#include <nobug.h>



namespace util {
  
  template<typename N>
  inline constexpr bool
  isPow2 (N n)  noexcept
    {
      return n > 0 and !(n & (n-1));
    };    // at each power of 2, a new bit is set for the first time
  
  
  /** helper to treat int or long division uniformly */
  template<typename I>
  struct IDiv
    {
      I quot;
      I rem;
      
      constexpr
      IDiv (I num, I den)  noexcept
        : quot(num/den)
        , rem(num - quot*den)
        { }
    };
  
  template<>
  struct IDiv<int>
    : div_t
    {
      constexpr
      IDiv (int num, int den)  noexcept
        : div_t(div (num,den))
        { }
    };
  
  template<>
  struct IDiv<long>
    : ldiv_t
    {
      constexpr
      IDiv (long num, long den)  noexcept
        : ldiv_t(ldiv (num,den))
        { }
    };
  
  template<>
  struct IDiv<llong>
    : lldiv_t
    {
      constexpr
      IDiv (llong num, llong den)  noexcept
        : lldiv_t(lldiv (num,den))
        { }
    };
  
  template<typename I>
  inline constexpr IDiv<I>
  iDiv (I num, I den)  noexcept  ///< support type inference and auto typing...
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
  inline constexpr I
  floordiv (I num, I den)  noexcept
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
  inline constexpr IDiv<I>
  floorwrap (I num, I den)  noexcept
  {
    IDiv<I> res(num,den);
    if (0 > (num^den) and res.rem)
      {  // negative results
        //  wrapped similar to floor()
        --res.quot;
        res.rem = den - (-res.rem);
      }
    return res;
  }
  
  
  /** quantise towards ceiling.
   *  This function models to fit something into a tiled structure.
   */
  template<typename U>   requires std::unsigned_integral<U>
  inline constexpr U
  ceilDiv (U num, U den) noexcept
  {
    return (num + den - 1u) / den;
  }
  
  static_assert (0 == ceilDiv (0u,5u));
  static_assert (1 == ceilDiv (1u,5u));
  static_assert (1 == ceilDiv (4u,5u));
  static_assert (1 == ceilDiv (5u,5u));
  static_assert (2 == ceilDiv (6u,5u));
  
  
  
  /**
   * Transform into a cyclically wrapping domain.
   * @tparam N any integral type, signed or unsigned
   * @return mapping into the range [minVal...maxVal(
   * @remark This function exploits the equivalence between signed and corresponding unsigned type
   *   to perform the modulus computation with positive numbers, because the modulus operator of C++
   *   works flipped for negative numbers, and oriented towards zero, while we want here always
   *   the same cyclic wrap-around, irrespective of the position relative to zero.
   *   - mathematically we want: `minVal + (rawVal-minVal) modulus PERIOD`
   *   - the unsigned representation of a negative number wraps cleanly, and thus maxVal - minVal,
   *     when converted into unsigned, is off by multiples of the whole number domain length, which
   *     is again absorbed by wrapping. Thus surprisingly `PERIOD = U(U(maxVal) - U(minVal))` is exact
   *     and can always be represented in the unsigned type.
   *   - for rawVal < minVal a flipped definition is used that can also be represented as unsigned:
   *     ** define dist ≔ minVal - rawval, then dist ≥ 1 for rawVal < minVal
   *     ** mathematically we need offset ≡ (-dist) modulus PERIOD
   *     ** we could compute PERIOD - (dist % PERIOD) -- but that would flip for dist ≡ PERIOD
   *     ** thus both arguments are shifted by -1, which keeps them in-range and computable as unsigned
   * @note the repeated casts `U(signed)` prevent C++ integer promotion to kick in (esp. by short and char)
   * @remark Claude was used (9/26) to improve and then formally verify this solution.
   */
  template<std::integral N>
  inline constexpr N
  cyclicWrap (N rawVal, N minVal, N maxVal)
  {
     REQUIRE (minVal < maxVal);
     using U = std::make_unsigned_t<N>;
     const U PERIOD = U(U(maxVal) - U(minVal));    // always representable
     U offset;
     if (rawVal >= minVal)
       offset = U(U(rawVal) - U(minVal)) % PERIOD;
     else
       {
          const U dist = U(U(minVal) - U(rawVal)); // dist ≥ 1
          offset = PERIOD-1 - (dist-1) % PERIOD;   // ≡ (-dist) modulus PERIOD
       }
     return N(U(U(minVal) + offset));
   }
  
  
  /**
   * Transform into a cyclically wrapping domain: floating point implementation.
   * @return mapping into the range [minVal...maxVal(
   * @warning this implementation can not cover extreme numeric values beyond #limit_cyclicWrap()
   * @remark within the usable numeric range, the flipped modulus for values below minVal can simply
   *   be fixed by shifting once by PERIOD. But there is a caveat: extremely small negative offsets
   *   could be absorbed, so that the result lands on +PERIOD
   */
  template<std::floating_point F>
  inline constexpr F
  cyclicWrap (F rawVal, F minVal, F maxVal)
  {
    REQUIRE (minVal < maxVal);
    const F PERIOD = maxVal - minVal;
    F offset = std::fmod (rawVal - minVal, PERIOD);
    if (offset < 0)
      offset += PERIOD;
    if (not (offset < PERIOD)) // can happen due to ε or non-finite values
      offset = 0;
    F res = minVal + offset;   // could be rounded to maxVal
    return res < maxVal? res : minVal;
  }
  
  
  /**
   * Validation helper for the floating-point implementation of #cyclicWrap().
   * Values beyond the computed limit can not be handled by the implementation,
   * due to the limited internal resolution of floating-point computations.
   * @param period length of the cyclic wrap period (≔ maxVal - minVal)
   * @return a limit for each parameter values, taken absolutely
   * @remark at the core of the #cyclicWrap() computation sits a numeric difference,
   *   taken modulo the period length. Each floating point number has a minimum resolution ("ULP").
   *   When the ULP of the difference computation surpasses the periodic cycle length, the cyclicWrap
   *   becomes meaningless (albeit computed correct without glitch). And since that offset is then
   *   added on top of meanValue, the same argument holds, so `std::abs (minVal) <= limit` is required,
   *   and by transitive argument also for `maxVal` and `rawVal`. Furthermore, the difference and the
   *   period length must also be finite numbers by themselves.
   * @note for performance reasons, that limit should be computed once, checked against `minVal` and `maxVal`,
   *   and then before each computation of #cyclicWrap(), `std::abs (rawVal) <= limit` should be checked.
   *   It is recommended to fail with an hard error, since resolving theses settings typically requires
   *   an intervention by the user (parameters need to be adjusted).
   * @remark Claude was used (9/26) to analyse the computation and derive a boundary criterion.
   */
  template<std::floating_point F>
  inline constexpr F
  limit_cyclicWrap (F period)
  {
    REQUIRE (std::isfinite (period));
    F limit = period / std::numeric_limits<F>::epsilon();
    if (not std::isfinite (limit))
      limit = std::numeric_limits<F>::max();
    return limit / 2; // exact
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
  template<std::floating_point F>
  inline constexpr bool
  almostEqual (F d1, F d2, uint ulp =2)  noexcept
  {
    using std::fabs;
    return fabs (d1-d2) < std::numeric_limits<F>::epsilon() * fabs (d1+d2) * ulp
        or fabs (d1-d2) < std::numeric_limits<F>::min(); // special treatment for subnormal results
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
   * [stackoverflow]: https://stackoverflow.com/questions/994593/how-to-do-an-integer-log2-in-c/24748637#24748637 "How to do an integer log2()"
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
  
  static_assert (-1 == ilog2(-5));
  static_assert ( 0 == ilog2( 1));
  static_assert ( 5 == ilog2(42));
  
  
} // namespace util
#endif /*UTIL_QUANT_H*/
