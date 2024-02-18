/*
  BINARY-SEARCH.hpp  -  generic search over continuous domain with a probe predicate

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

/** @file binary-search.hpp
 ** Textbook implementation of the classical binary search over continuous domain.
 ** The domain is given by its lower and upper end points. Within this domain,
 ** a _breaking point_ is located, where the result of a _probe predicate_
 ** flips from `false` to `true`. For the core search, the _invariant_
 ** is assumed, implying that the `predicate(lower) ≡ false` and
 ** `predicate(upper) ≡ true`.
 ** 
 ** For good convergence, it is advisable to enter the search with rather tight
 ** bounds. For the case that it's not clear if the invariant holds for both ends,
 ** two alternative entrance points are provided, which check the condition on the
 ** interval ends and possibly shift and expand the search domain in case the
 ** assumption is broken.
 ** 
 ** @see stress-test-rig.hpp
 ** @see SchedulerStress_test
 */


#ifndef LIB_BINARY_SEARCH_H
#define LIB_BINARY_SEARCH_H


#include "lib/meta/function.hpp"

#include <utility>


namespace lib {
  
  using std::forward;
  
  /** binary search: actual search loop
   * - search until (upper-lower) < epsilon
   * - the \a FUN performs the actual test
   * - the goal is to narrow down the breaking point
   * @param fun `bool(PAR)` perform probe and decide criterion.
   * @note `fun(lower)` must be `false` and
   *       `fun(upper)` must be `true`
   */
  template<class FUN, typename PAR>
  inline auto
  binarySearch_inner (FUN&& fun, PAR lower, PAR upper, PAR epsilon)
  {
    ASSERT_VALID_SIGNATURE (FUN, bool(PAR) );
    REQUIRE (lower <= upper);
    while ((upper-lower) >= epsilon)
      {
        PAR div = (lower+upper) / 2;
        bool hit = fun(div);
        if (hit)
          upper = div;
        else
          lower = div;
      }
    return (lower+upper)/2;
  }
  
  
  /** entrance point to binary search to ensure the upper point
   *  indeed fulfils the test. If this is not the case, the search domain
   *  is shifted up, but also expanded so that the given upper point is
   *  still located within, but close to the lower end.
   * @note `fun(lower)` must be `false`
   */
  template<class FUN, typename PAR>
  inline auto
  binarySearch_upper (FUN&& fun, PAR lower, PAR upper, PAR epsilon)
  {
    REQUIRE (lower <= upper);
    bool hit = fun(upper);
    if (not hit)
      {// the upper end breaks contract => search above
        PAR len = (upper-lower);
        lower = upper - len/10;
        upper = lower + 14*len/10;
      }
    return binarySearch_inner (forward<FUN> (fun), lower,upper,epsilon);
  }
  
  
  template<class FUN, typename PAR>
  inline auto
  binarySearch (FUN&& fun, PAR lower, PAR upper, PAR epsilon)
  {
    REQUIRE (lower <= upper);
    bool hit = fun(lower);
    if (hit)
      {// the lower end breaks contract => search below
        PAR len = (upper-lower);
        upper = lower + len/10;
        lower = upper - 14*len/10;
      }
    return binarySearch_upper (forward<FUN> (fun), lower,upper,epsilon);
  }
  
  
} // namespace lib
#endif /*LIB_BINARY_SEARCH_H*/
