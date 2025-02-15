/*
  VARIADIC-REBIND.hpp  -  metaprogramming utilities for parameter- and type sequences

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file variadic-rebind.hpp
 ** Metaprogramming support to rebuild and rebind variadic templates.
 ** The sequence of variadic arguments itself is often difficult to manipulate,
 ** because a _parameter pack_ is not a type proper in C++, but rather some meta
 ** mapping expanded immediately by the compiler. For more elaborate processing,
 ** this sequence must thus be re-mapped into a format that allows to pass partial
 ** results from recursive evaluations (since all of this »processing« is actually
 ** a form of functional programming).
 ** 
 ** For many _simple cases_ though it is sufficient just to re-bind a template to
 ** another template's variadic sequence, possibly with some basic manipulation.
 ** Such a meta-processing can be coded very succinctly, by passing the target
 ** template to receive the altered sequence as a template-template parameter.
 ** @warning this kind of recursive remoulding typically imposes an O(n) effort
 **          during compilation, sometimes even O(n²) (notably reversing a sequence).
 **          Thus be sure to apply to short type sequences only
 **          endcode
 ** \par Usage example
 ** \code{.cpp}
 ** template<typename...CASES>
 ** struct MyModel
 **   {
 **     using SubSeq = typename _Vari<MyModel, CASES...>::Prefix;
 **
 **     // adapt a predecessor sequence
 **     MyModel (SubSeq&& subModel);
 **
 **
 **     using Tuple = typename RebindVariadic<std::tuple, CASES...>::Type;
 **   }
 ** \endcode
 ** @see param-weaving-pattern.hpp "usage example"
 ** @see util::parse::AltModel "usage example"
 ** @see typelist.hpp
 ** @see function.hpp
 */


#ifndef LIB_META_VARIADIC_REBIND_H
#define LIB_META_VARIADIC_REBIND_H


namespace lib {
namespace meta {
  
  
  
  
  /* ==== Rebinding Variadic Arguments ==== **/
  
  /**
   * Metaprogramming helper to transfer variadic arguments.
   * - builds a new type instantiation from the Template \a X
   * - possibly picks up the variadic argument pack from a given
   *   source template `U<ARGS....>`
   * @tparam X a variadic template
   */
  template<template<typename...> class X, typename...ARGS>
  struct RebindVariadic
    {
      using Type = X<ARGS...>;
    };
  
  template<template<typename...> class X
          ,template<typename...> class U
          ,typename...ARGS>
  struct RebindVariadic<X, U<ARGS...>>
    {
      using Type = X<ARGS...>;
    };
  
  
  
  
  
  /* ==== Rebuild with remoulded variadic sequence ==== **/
  
  /**
   * Metaprogramming helper to remould the type sequence
   * in the template arguments of a variadic template.
   * The key idea is to construct a new instance of the
   * target template with an altered sequence; the target
   * template itself is passed as template-template param.
   * - `Penult` get the penultimate element of the sequence
   * - `Ultima` get the last element of the sequence
   * - `Prefix` rebind to the prefix sequence, leaving out the last
   * - `Remain` rebind to the sequence starting with the second
   * - `Revers` rebind to a sequence with reversed order
   * A secondary helper template variant is provided for rebinding
   * while prepending or appending a single type parameter.
   * @note does not work with empty sequences; also the penultimate
   *       of a one-element sequence is mapped to NullType
   */
  template<template<class...> class L, typename...XS>
  struct _Vari;
  
  template<template<class...> class L, typename X, typename...XS>
  struct _Vari<L, X, L<XS...>>
    {
      using Prepend = L<X,XS...>;
      using Append  = L<XS...,X>;
    };
  
  template<template<class...> class L, typename X1, typename X2>
  struct _Vari<L, X1,X2>
    {
      using Penult = X1;
      using Ultima = X2;
      using Prefix = L<X1>;
      using Remain = L<X2>;
      using Revers = L<X2,X1>;
    };
  
  template<template<class...> class L, typename X>
  struct _Vari<L, X>
    {
      using Ultima = X;
      using Penult = NullType;  ///< marker for undefined
      using Remain = L<X>;
      using Revers = L<X>;
      using Prefix = L<>;
    };
  
  template<template<class...> class L, typename X, typename...XS>
  struct _Vari<L, X,XS...>
    {
      using Penult = typename _Vari<L,XS...>::Penult;
      using Ultima = typename _Vari<L,XS...>::Ultima;
      
      using _Tail_Pre_ = typename _Vari<L,XS...>::Prefix;
      using _Tail_Rev_ = typename _Vari<L,XS...>::Revers;
      
      using Remain = L<XS...>;
      using Prefix = typename _Vari<L, X,      _Tail_Pre_>::Prepend;
      using Revers = typename _Vari<L, Ultima, _Tail_Rev_>::Prepend;
    };
  
}} // namespace lib::meta
#endif /*LIB_META_VARIADIC_REBIND_H*/
