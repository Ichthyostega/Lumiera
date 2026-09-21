/*
  TYPE-HANDLER.hpp  -  generic type conversion handling support

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file type-handler.hpp
 ** Generic building blocks to support parameter value conversions and adaptation.
 ** The implementation of parameters is built such as to conceal the actual type of
 ** the parameter values -- which requires a virtual dispatch at some point. Yet the
 ** underlying type conversion functionality can be defined in a generic way, based
 ** on the actual types involved. Simple value-like parameters are based on a fixed
 ** collection of number types, both integral and floating-point. Thus the actual
 ** conversion is handled by the promotion- and coercion rules of C++ in most cases.
 ** 
 ** @todo WIP-WIP-WIP can be considered an initial draft and placeholder, as of 2026
 */


#ifndef LIB_PAR_TYPE_HANDLER_H
#define LIB_PAR_TYPE_HANDLER_H


#include "lib/par/spec.hpp"
#include "lib/meta/trait.hpp"
#include "lib/util.hpp"

#include <concepts>
#include <limits>


namespace lib {
namespace par {
  
  using util::isNeg;
  using std::same_as;
  using std::integral;
  using std::signed_integral;
  using std::floating_point;
  using std::numeric_limits;
  using std::common_type_t;
  using std::is_signed_v;
  using std::is_unsigned_v;
  using std::is_integral_v;
  using std::is_floating_point_v;
  
  
  /* ===== type conversion implementation details ===== */
  
  /** Constraint: type \a SUB can not represent the full domain of type \a BAS
   * @todo implicitly also implements a limitation to »numeric« types,
   *       due to the use of std::numeric_limits
   */
  template<typename SUB, typename BAS>
  concept sub_domain = signed_integral<SUB> != signed_integral<BAS>
                    or numeric_limits<SUB>::max()    < numeric_limits<BAS>::max()
                    or numeric_limits<SUB>::lowest() > numeric_limits<BAS>::lowest()
                     ;
  
  
_Pragma("GCC diagnostic push") \
_Pragma("GCC diagnostic ignored \"-Wsign-compare\"")
// the following functions are specifically crafted to work around
// the ambiguities caused by automatic signed-to-unsigned promotion;
// furthermore, the automatic promotion allows to perform the comparison.
  
  template<typename X, typename Y>
  inline constexpr bool
  isLower_safe (X const& x, Y const& y)
  {
    return isNeg(x) == isNeg(y)? x < y
                               : isNeg(x);
  }

  template<typename X, typename Y>
  inline constexpr bool
  isLarger_safe (X const& x, Y const& y)
  {
    return isNeg(x) == isNeg(y)? x > y
                               : isNeg(y);
  }
_Pragma("GCC diagnostic pop")



  /** generic helper to conform into a common value domain
   * @tparam SUB the implied target value domain
   * @tparam BAS source data type
   * @return a value in source data type,
   *         yet conformed into the target data range
   */
  template<typename SUB, typename BAS>
  inline constexpr BAS
  preClamp (BAS const& value)
  {
    return value;
  }
  
  /** special case: clamp the source value into the target value domain.
   * @remark tricky due to automatic conversions between signed/unsigned */
  template<typename SUB, typename BAS>   requires (sub_domain<SUB,BAS>
                                                   and not same_as<SUB,bool>)
  inline constexpr BAS
  preClamp (BAS const& rawVal)
  {
    static_assert (std::is_constructible_v<BAS,SUB const&>
                  ,"Value domains must overlap at least partially "
                   "and values must be cross-constructible within overlap");
    
    auto upperBound = numeric_limits<SUB>::max();
    auto lowerBound = numeric_limits<SUB>::lowest();
    
    return isLarger_safe(rawVal, upperBound)? BAS(upperBound)
         : isLower_safe (rawVal, lowerBound)? BAS(lowerBound)
                                            : rawVal;
  }
  
  /** special case: trigger a bool at the 0.5 level */
  template<same_as<bool> B, floating_point F>
  inline constexpr F
  preClamp (F const& rawVal)
  {
    return 0.5 < rawVal? F{1} : F{0};
  }
  
  
  
  template<typename V, typename X>
  inline void
  assignConverted (V& targetVal, X const& srcVal)
  {
    if constexpr (std::is_assignable_v<V&, X const&>)
      {
        targetVal = preClamp<V> (srcVal);
      }
    else
    if constexpr (std::is_constructible_v<V, X const&>)
      {
        targetVal.~V();
        new(&targetVal) V (preClamp<V> (srcVal));
      }
    else
      static_assert (!sizeof(X), "this type conversion is not supported");
  }
  
  
  template<typename T>
  concept number = integral<T> or floating_point<T>;

  /**
   * the best-precision floating point type to carry out
   * a numeric operation to combine two values
   * @note the result type is always some floating point,
   *       since the goal is to use the exponential / logarithm family.
   * @todo 9/26 we _could_ go from integral to long double for 64bit,
   *       yet it is not clear if we ever need that tiny bit of extra precision...
   */
  template<number V, number W>
  using CommonComputeType = std::conditional_t<integral<V> and integral<W>, double
                                                                          , common_type_t<V, W>>;

  namespace {
    /** @internal decision logic for combining numeric parameters */
    template<typename V, typename W>
    auto
    selectNumberJoinType()
    {
      if constexpr (is_floating_point_v<V> or is_floating_point_v<W>)
          return std::type_identity<common_type_t<V,W>>{};
      else
      if constexpr (is_signed_v<V> == is_signed_v<W>)
          return std::type_identity<common_type_t<V,W>>{};
      else
        { // mixed signed/unsigned integral: need a type wide enough to accommodate.
          using U = std::conditional_t<is_signed_v<V>, W,V>;
          if constexpr (8 > sizeof(U))
              return std::type_identity<int64_t>{};
          else
              return std::type_identity<lflp>{};
              // no standard integral type can hold uint64_t's full range signed.
              // Expand to long double, which can hold 64bit without precision loss
        }
    }
  }
  
  /**
   * @return A common type to perform simple numeric operations safely.
   *  - when one of the type parameters is floating-point, computations switch over there
   *  - on integral types with same signedness, std::common_type handles a sane common ground
   *  - when mixed computations can be managed within signed 64bit, switch up to that
   *  - in the remaining problematic case(s), long double is used, as it can accommodate
   *    the full 64bit value range without loss on platforms relevant for Lumiera.
   */
  template<typename A, typename B>
  using NumberJoinType = typename decltype(selectNumberJoinType<A,B>())::type;
  
  
  
}} // namespace lib::par
#endif /*LIB_PAR_TYPE_HANDLER_H*/
