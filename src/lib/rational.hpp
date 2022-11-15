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
 ** # Perils of fractional arithmetics
 ** 
 ** While the always precise results of integral numbers might seem compelling, the
 ** danger of _numeric overflow_ is significantly increased by fractional computations.
 ** Most notably, this danger is *not limited to large numbers*. Adding two fractional
 ** number requires multiplications with both denominators, which can overflow easily.
 ** Thus, for every given fractional number, there is a class of »dangerous counterparts«
 ** which can not be added without derailing the computation, leading to arbitrary wrong
 ** results without detectable failure. And these problematic counterparts are distributed
 ** _over the whole valid numeric range._ To give an extreme example, any numbers of the
 ** form `n / INT_MAX` can not be added or subtracted with any other rational number > 1,
 ** while being themselves perfectly valid and representable.
 ** \par rule of thumb
 ** Use fractional arithmetics only where it is possible to control the denominators involved.
 ** Never use them for computations drawing from arbitrary (external) input.
 ** 
 ** @see Rational_test
 ** @see zoom-window.hpp
 ** @see timevalue.hpp
 */


#ifndef LIB_RATIONAL_H
#define LIB_RATIONAL_H


#include <cmath>
#include <limits>
#include <stdint.h>
#include <boost/rational.hpp>



namespace util {
  
  using Rat = boost::rational<int64_t>;
  using boost::rational_cast;
  using std::abs;
  
  // these are neither standard C++ nor POSIX, yet pretty much common place...
  using uchar = unsigned char;
  using uint  = unsigned int;
  
  
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
  inline int
  ilog2 (I num)
  {
    if (num <= 0)
      return -1;
    const I MAX_POW = sizeof(I)*CHAR_BIT - 1;
    int logB{0};
    auto remove_power = [&](I pow)
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
  
  
  inline bool
  can_represent_Product (int64_t a, int64_t b)
  {
    return ilog2(abs(a))+1
         + ilog2(abs(b))+1
         < 63;
  }
  
  inline bool
  can_represent_Sum (Rat a, Rat b)
  {
    return can_represent_Product(a.numerator(), b.denominator())
       and can_represent_Product(b.numerator(), a.denominator());
  }
  
  
  /**
   * re-Quantise a rational number to a (typically smaller) denominator.
   * @param u the new denominator to use
   * @warning this is a lossy operation and possibly introduces an error
   *          of up to 1/u
   * @remark  Rational numbers with large numerators can be »poisonous«,
   *          causing numeric overflow when used, even just additively.
   *          This function can thus be used to _"sanitise"_ a number,
   *          and thus accept a small error while preventing overflow.
   * @note using extended-precision floating point to get close to the
   *          quantiser even for large denominator. Some platforms
   *          fall back to double, leading to extended error bounds
   */
  inline Rat
  reQuant (Rat src, int64_t u)
  {
    int64_t d = rational_cast<int64_t> (src);
    int64_t r = src.numerator() % src.denominator();
    using f128 = long double;
    
    // construct approximation quantised to 1/u
    f128 frac = rational_cast<f128> (Rat{r, src.denominator()});
    Rat res = d + int64_t(frac*u) / Rat(u);
    ENSURE (abs (rational_cast<f128>(src) - rational_cast<f128>(res)) <= 1.0/abs(u));
    return res;
  }
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
