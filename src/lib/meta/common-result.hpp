/*
  COMMON-RESULT.hpp  -  reconciled return types

   Copyright (C)
     2024,2026        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file common-result.hpp
 ** Determine a common result type for combined functors.
 ** This header defines a metafunction to determine if, and how the return types
 ** of several functors or accessors can be combined. This task becomes relevant
 ** when building exploration strategies defined over a functional datastructure.
 ** A notable example happens when performing a _tree expansion_ from within a
 ** [iterator pipeline](\ref iter-explorer.hpp) or when mapping an aggregating
 ** functor onto a [tuple of values](\ref tuple-concept.hpp).
 ** 
 ** The basic technique to find such a _reconciled type_ is to rely on the
 ** _ternary operator_ (`test? yes_expr : no_expr;`), which is specified to
 ** great detail in the C++ standard. However, the STDLIB provides only two
 ** tools tailored for specific cases:
 ** - std::common_type is what you can use to _store_ such a reconciled result,
 **   and includes _decaying_ the type.
 ** - std::common_reference is oriented towards passing of references,
 **   and includes elaborate mapping logic for mixture of RValues and LValues.
 ** 
 ** So there is a need to expose the foundational logic for type reconciliation,
 ** without any special adaptation rules. Furthermore, direct support for passing
 ** the selection of types in the form of a [Type Sequence](\ref typelist.hpp)
 ** is included.
 ** 
 ** @see CommonResult_test
 ** @see lib::iter_explorer::Expander
 ** @see tuple-idx-adaptor.hpp
 */


#ifndef LIB_META_COMMON_RESULT_H
#define LIB_META_COMMON_RESULT_H


#include "lib/meta/trait.hpp"
#include "lib/meta/typelist.hpp"



namespace lib {
namespace meta {
  
  namespace detail {// Implementation: reconcile N return types...
    
    /** @internal helper to find a reconciled result type.
     * This is the technical part, and relies on a SFINAE technique:
     * The test succeeds, if a ternary operator with both type results
     * can be instantiated in the signature of a `check` template.
     * 
     * Furthermore, the reconciliation for a pair of types can be
     * generalised to N types by gradually reconciling the following
     * types with the already established result.
     * 
     * @remark 2026 the GNU Libstdc++ relies on precisely the same
     *   technique, which implies that this approach works reliably.
     */
    template<typename X, typename U, typename...TYPES>
    class _ProbeCommonType
      {
        /******************************************************************************//**
         * Core of the type reconciliation logic: determine the _common type_.
         * The implementation relies on the **ternary operator** — the C++ language
         * specifies a detailed and elaborate algorithm for this operator to reconcile
         * the results of the "true" and "false" parts of `test? true_part : false_part`;
         * such an expression compiles if and only if these criteria are met and a common
         * type can be determined. The results of this procedure are quite solid, and
         * satisfactory in most usage situations. A mixture of references, values const
         * and volatile is handled properly. Only some rare corner cases are not covered,
         * like upcasting two subclasses to a common interface (actually in some cases,
         * involving multiple inheritance, a single authoritative solution for this
         * special problem does not exist, and thus these cases were left out
         * from the language spec).
         */
        template<typename T1, typename T2>
        using Reconciled = decltype(true ? std::declval<T1>() : std::declval<T2>());
        
        
        /** Marker type: successful test */
        template<typename RES>
        struct FoundResult
          : std::true_type
          {
            using Type = RES;
          };
        struct NoResultFound
          : std::false_type
          { };
        
        
        /** SFINAE setup to probe for a reconciled type */
        template<class T1
                ,class T2
                ,typename = Reconciled<T1,T2>>
        static FoundResult<Reconciled<T1,T2>>
        check(int);
        
        template<class,class>
        static NoResultFound
        check(...);
        
        /** perform the reconciliation test */
        using FirstResult = decltype(check<X,U>(42));
        
        
        /** Helper: Join results of pairwise reconciliation
         *  to extend the result to a sequence of types */
        template<bool success, typename...TS>
        struct Join
          {
            using Result = FirstResult;
          };
        
        template<typename TX, typename...TS>
        struct Join<true, TX,TS...>      ///<@note matches when FirstResult was successful
          {
            using CT = FirstResult::Type;
            using Result = _ProbeCommonType<CT,TX,TS...>::Result;
          };
        
        
      public:
        /** Retrieve the overall result of this metafunction */
        using Result = Join<FirstResult::value, TYPES...>::Result;
      };
    
  }//(End)implementation: type reconciliation
  
  
  
  /**
   * Metafunction to find a _common result type_.
   * @tparam TYPES a variadic sequence of types to be reconciled.
   * Finding such a common result type can be relevant when results
   * from several functors shall be combined, e.g. when mapping and
   * aggregating results of a function application to a tuple.
   * It is important to note that such a function may return arbitrary
   * types, either by value or by reference, and possibly marked as const.
   * 
   * \par Interface
   * The `struct` CommonResult itself is a std::bool_constant that
   * indicates if the given types can be reconciled. When successful,
   * a nested typedef `Type` is present and aliased to a type that
   * can represent all given result types as a function return.
   * 
   * @remarks
   *  - the C++ standard offers similar functionality as
   *    + std::common_type<TYPES...>
   *    + std::common_reference<TYPES...>
   *  - however, the standard solution, while based on the same logic
   *    as this implementation, adds additional transformations, like
   *    decaying the type (for std::common_type) or handling various
   *    corner cases with RValue references. The specification of
   *    expected behaviour is quite elaborate for those cases
   *  - the implementation presented here is focused on _returning_
   *    results from a function and thus relies _solely_ on the
   *    logic of the ternary operator (`? : `)
   *  - when several LValue-reference types are given, the result
   *    is only a reference when all results can be presented safely
   *    through such a reference; in most cases the result falls back
   *    to returning by-value.
   * @note Concepts are provided to require that a selection of types
   *       is reconcilable, or even reconcilable into a reference type.
   * @see CommonResult_test
   */
  template<typename...TYPES>
  struct CommonResult
    : detail::_ProbeCommonType<TYPES...>::Result
    { };
  
  template<typename T1>
  struct CommonResult<T1>
    : CommonResult<T1,T1>
    { };
  
  template<>
  struct CommonResult<>
    : std::false_type
    { };
  
  /** Specialisation to work transparently also on a type sequence,
   *  passed-in as a single template parameter. */
  template<typename...TYPES>
  struct CommonResult<Types<TYPES...>>
    : CommonResult<TYPES...>
    { };

  

  template<typename...TYPES>
  using CommonResult_t = CommonResult<TYPES...>::Type;
  
  template<typename...TYPES>
  static constexpr bool has_CommonResult_v = CommonResult<TYPES...>::value;
  
  
  
  template<typename...TYPES>
  concept reconcilable = requires
    {
      typename CommonResult<TYPES...>::Type;
    };
  
  template<typename...TYPES>
  concept reconcilable_ref = reconcilable<TYPES...>
                         and isLRef_v<CommonResult_t<TYPES...>>;
  
  
  
  template<typename...TYPES>
  struct CommonResultTraits
    {
      static_assert (reconcilable<TYPES...>, "unable to reconcile these types");
      
      using CommonVal = std::common_type_t<TYPES...>;
      using ResType   = CommonResult_t<TYPES...>;
      //NOTE: std::common_type decays (strips CV and ref)
      static constexpr bool makeConst = isConst_v<ResType>;
      static constexpr bool exposeRef = isLRef_v<ResType>;
    };
  
  
  
}} // namespace lib::meta
#endif /*LIB_META_COMMON_RESULT_H*/
