/*
  TUPLE-ACCESSOR.hpp  -  metaprogramming utilities for type and data tuples

   Copyright (C)
     2016             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file tuple-accessor.hpp
 ** Metaprogramming with tuples-of-types and the `std::tuple` record.
 ** This header complements typelist.hpp and provides a bridge from type sequences
 ** to the tuple type provided by the standard library, including traits and
 ** helpers to build tuple types from metaprogramming and to pretty-print tuples.
 ** 
 ** Notably the handling of tuples is based on a **Concept** `tuple_like`,
 ** that is satisfied for any type in compliance with the »tuple protocol«.
 ** Together with a [generic accessor][\ref lib::meta::getElm], this allows
 ** to handle all _tuple-like_ types uniformly.
 ** This solution reaches beyond the scope of the C++ standard, which declines
 ** to define such a concept and rather provides an "exposition-only" concept
 ** that is limited to a hard-wired selection of library types, yet does not
 ** cover custom user provided types able to participate in _structured bindings_.
 ** @note Due to that unfortunate limitation of the standard, we're forced to
 **       provide our own alternative implementation of `std::apply`.
 ** 
 ** Furthermore, a generic iteration construct is provided, to instantiate
 ** a generic Lambda for each element of a given tuple, which allows to write
 ** generic code »for each tuple element«.
 ** 
 ** @see control::CommandDef usage example
 ** @see TupleHelper_test
 ** @see typelist.hpp
 ** @see function.hpp
 ** @see generator.hpp
 ** 
 */


#ifndef LIB_META_TUPLE_ACCESSOR_H
#define LIB_META_TUPLE_ACCESSOR_H

#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-util.hpp"
#include "lib/meta/typeseq-util.hpp"
#include "lib/meta/variadic-helper.hpp"
#include "lib/meta/util.hpp"

#include <tuple>
#include <utility>
#include <functional>


namespace util { // forward declaration
  
  template<typename TY>
  std::string
  toString (TY const& val)  noexcept;
}


namespace lib {
namespace meta {
  
  /** trait to detect tuple types */
  template<typename T>
  struct is_Tuple
    : std::false_type
    { };
  
  template<typename...TYPES>
  struct is_Tuple<std::tuple<TYPES...>>
    : std::true_type
    { };
  
  template<typename...TYPES>
  struct is_Tuple<const std::tuple<TYPES...>>
    : std::true_type
    { };
  
  using std::remove_cv_t;
  using std::is_reference_v;
  using std::remove_reference_t;
  
  
  /** @internal building-block: a type supporting the `tuple_size` metafunction */
  template<class TUP>
  concept tuple_sized = requires
    {
      { std::tuple_size<TUP>::value } -> std::convertible_to<size_t>;
    };
  
  
  /** @internal building-block: a type where elements can be accessed through a `get` friend function */
  template<class TUP, std::size_t idx>
  concept tuple_adl_accessible = requires(TUP tup)
    {
      typename std::tuple_element_t<idx, TUP>;
      { get<idx>(tup) } -> std::convertible_to<std::tuple_element_t<idx, TUP>&>;
    };
  
  /** @internal building-block: a type where elements can be accessed through a `get` member function */
  template<class TUP, std::size_t idx>
  concept tuple_mem_accessible = requires(TUP tup)
    {
      typename std::tuple_element_t<idx, TUP>;
      { tup.template get<idx>() } -> std::convertible_to<std::tuple_element_t<idx, TUP>&>;
    };
  
  template<class TUP, std::size_t idx>
  concept tuple_element_accessible = tuple_mem_accessible<TUP,idx> or tuple_adl_accessible<TUP,idx>;
  
  template<class TUP>
  concept tuple_accessible =
    tuple_sized<TUP> and
    WithIdxSeq<std::tuple_size_v<TUP>>::andAll([](auto idx)
                                                {
                                                 return tuple_element_accessible<TUP,idx>;
                                                });
  
  
  /**
   * Concept to mark any type compliant to the »tuple protocol«
   */
  template<class TUP>
  concept tuple_like = not is_reference_v<TUP>
                   and tuple_sized<remove_cv_t<TUP>>
                   and tuple_accessible<remove_cv_t<TUP>>;
  
  
  /**
   * Helper for abstracted / unified access to member elements of any _tuple-like_
   * @remark preferably uses a `get<i>` member function, falling back to a
   *         free function `get`, which is found by ADL.
   */
  template<std::size_t idx, class TUP>
                            requires(tuple_like<std::remove_reference_t<TUP>>)
  decltype(auto)
  getElm (TUP&& tup)
  {
    using Tup = std::remove_reference_t<TUP>;
    static_assert (0 < std::tuple_size_v<Tup>);
    if constexpr (tuple_mem_accessible<Tup,0>)
      {
        if constexpr (std::is_reference_v<TUP>)
          return tup.template get<idx>();
        else
          { // return value copy when tuple given as RValue
            using Elm = std::tuple_element_t<idx, TUP>;
            Elm elm(tup.template get<idx>());
            return elm;
          }
      }
    else
      {     // ▽▽▽ ADL
        using std::get;
        return get<idx> (std::forward<TUP> (tup));
      }
  }
  
  
  
  
  namespace { // apply to tuple-like : helpers...
    
    /** @internal invocation helper similar to C++17 — but preferring a custom `get` impl */
    template<typename FUN, typename TUP, size_t...Idx>
    constexpr decltype(auto)
    __unpack_and_apply (FUN&& f, TUP&& tup, std::index_sequence<Idx...>)
    {
      return std::invoke (std::forward<FUN> (f)
                         ,getElm<Idx> (std::forward<TUP>(tup))...
                         );
    }
    
    
    /** @internal invoke a metafunction with \a FUN and all element types from the _tuple-like_ \a TUP */
    template<template<typename...> class META, class FUN, class TUP>
    struct _InvokeMetafunTup
      {
        using Tupl = std::decay_t<TUP>;
        using Elms = ElmTypes<Tupl>::Seq;
        using Args = Prepend<FUN, Elms>::Seq;
        using Type = RebindVariadic<META, Args>::Type;
      };
    template<template<typename...> class META, class FUN, class TUP>
    struct _InvokeMetafunTup<META, FUN, TUP&>
      {
        using Tupl = std::decay_t<TUP>;
        using Elms = typename ElmTypes<Tupl>::template Apply<std::add_lvalue_reference_t>;
        using Args = Prepend<FUN, Elms>::Seq;
        using Type = RebindVariadic<META, Args>::Type;
      };
    
    template<class FUN, class TUP>
    inline constexpr bool can_nothrow_invoke_tup = _InvokeMetafunTup<std::is_nothrow_invocable,FUN,TUP>::Type::value;
  }
  
  /**
   * Replacement for `std::apply` — yet applicable to _tuple-like custom types_.
   * For unclear reasons, the standard chooses to reject such custom types, and
   * only allows a fixed set of explicitly defined facilities from the Stdlib
   * (tuple, pair, array, and some ranges stuff).
   * @todo 6/2025 as a first step, this replicates the implementation from C++17;
   *       the second step would be to constrain this to a concept `tuple_like`
   */
  template<class FUN, class TUP>   requires(tuple_like<remove_reference_t<TUP>>)
  constexpr decltype(auto)
  apply (FUN&& f, TUP&& tup)  noexcept (can_nothrow_invoke_tup<FUN,TUP> )
  {
    using Indices = std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<TUP>>>;
    
    return __unpack_and_apply (std::forward<FUN> (f)
                              ,std::forward<TUP> (tup)
                              ,Indices{}
                              );
  }
  
  
  /**
   * Tuple iteration: perform some arbitrary operation on each element of a tuple.
   * @note the given functor must be generic, since each position of the tuple
   *       may hold a data element of different type.
   * @remark credits to David Vandevoorde (member of C++ committee) for using
   *       std::apply to unpack the tuple's contents into an argument pack and
   *       then employ a fold expression with the comma operator.
   */
  template<class TUP, class FUN>   requires(tuple_like<remove_reference_t<TUP>>)
  constexpr void
  forEach (TUP&& tuple, FUN fun)
  {
           lib::meta::apply ([&fun]<typename...ELMS>(ELMS&&... elms)
                                  {
                                    (fun (std::forward<ELMS>(elms)), ...);
                                  }
                            ,std::forward<TUP> (tuple));
  }
  
  /**
   * Apply some arbitrary function onto all elements of a tuple.
   * @return a new tuple constructed from the results of this function
   * @note the functor must be generic and able to work with all element types
   * @warning pay attention to references; the function can take arguments by-ref
   *          and manipulate them (side-effect), and it may return references,
   *          which will be placed as such into the result tuple; furthermore,
   *          the argument tuple can also be taken as reference...
   * @remark The tuple constructor invocation is preceded by a regular argument evaluation,
   *          which has unspecified evaluation order (even in C++17); _no assumptions_ can be
   *          made regarding the order the functor will see the source tuple elements.
   *          Notably this differs from #forEach, where a fold-expression with comma-operator
   *          is used, which is guaranteed to evaluate from left to right.
   */
  template<class TUP, class FUN>   requires(tuple_like<remove_reference_t<TUP>>)
  constexpr auto
  mapEach (TUP&& tuple, FUN fun)
  {
    return lib::meta::apply ([&fun]<typename...ELMS>(ELMS&&... elms)
                                  {  //..construct the type explicitly (make_tuple would decay fun result types)
                                    using Tuple = std::tuple<decltype(fun (std::forward<ELMS>(elms))) ...>;
                                    return Tuple (fun (std::forward<ELMS>(elms)) ...);
                                  }
                            ,std::forward<TUP> (tuple));
  }
  
  
  
  /**
   * Specialisation of variadic access for any tuple-like
   * @see variadic-helper.hpp
   */
  template<tuple_like TUP>
  struct ElmTypes<TUP>
    {
      template<typename>
      struct Extract;
      template<size_t...idx>
      struct Extract<std::index_sequence<idx...>>
        {
          using ElmTypes = Types<std::tuple_element_t<idx,TUP> ...>;
        };
      
      static constexpr size_t SIZ = std::tuple_size_v<TUP>;
      
      using Idx = std::make_index_sequence<SIZ>;
      using Seq = Extract<Idx>::ElmTypes;
      using Tup = RebindVariadic<std::tuple, Seq>::Type;
      
      template<template<class> class META>
      using Apply = ElmTypes<Seq>::template Apply<META>;
      
      template<template<typename...> class O>
      using Rebind = RebindVariadic<O, Seq>::Type;
      
      template<template<class> class PRED>
      using AndAll = ElmTypes<Apply<PRED>>::template Rebind<std::__and_>;
      
      template<template<class> class PRED>
      using OrAll  = ElmTypes<Apply<PRED>>::template Rebind<std::__or_>;
    };
  
  
  
  
  
  namespace { // rebinding helper to create std::tuple from a type sequence
    
    template<typename SEQ>
    struct BuildTupleType
      : std::false_type
      { };
    
    template<typename...TYPES>
    struct BuildTupleType<Types<TYPES...>>
      {
        using Type = std::tuple<TYPES...>;
      };
    
    template<class H, typename TAIL>
    struct BuildTupleType<Node<H, TAIL>>
      {
        using Seq  = Types<Node<H,TAIL>>::Seq;
        using Type = BuildTupleType<Seq>::Type;
      };
    
    template<>
    struct BuildTupleType<Nil>
      {
        using Type = std::tuple<>;
      };
  }
  
  
  /** Build a `std::tuple` from types given as type sequence
   * @remarks for Lumiera, we deliberately use a dedicated template `Types`
   *    to mark a type sequence of types as such. This allows to pass such a
   *    sequence as first-class citizen. The standard library often (ab)uses
   *    the std::tuple for this purpose, which is an understandable, yet
   *    inferior design choice. We should always favour dedicated types
   *    over clever re-use of existing types.
   */
  template<typename TYPES>
  using Tuple = BuildTupleType<TYPES>::Type;
  
  
  using std::tuple_size;
  using std::tuple_element;
  
  
  
  /** match and rebind the type sequence from a tuple */
  template<typename...TYPES>
  struct RebindTupleTypes
    {
      using Seq  = Types<TYPES...>::Seq;
      using List = Seq::List;
    };
  template<typename...TYPES>
  struct RebindTupleTypes<std::tuple<TYPES...>>
    {
      using Seq  = Types<TYPES...>::Seq;
      using List = Seq::List;
    };
  
  
  
  
  
  
  
  /**
   * Extensible Adapter to construct a distinct tuple from some arbitrary source type.
   * This includes the possibility to re-map elements or element positions.
   * @tparam TYPES sequence of types to use for the tuple
   * @tparam _ElmMapper_ a _template_ to extract each
   *         constructor argument from the source value.
   *         On invocation, we'll pick up the source type from the actual ctor argument,
   *         and then invoke this helper template iteratively for each component of the
   *         tuple, passing as template arguments
   *         - the source type, as picked up from the constructor
   *         - the target tuple type, i.e. `Tuple<TYPES>`
   *         - the actual index position of the tuple element
   *           to be initialised through this concrete instantiation.
   * @remarks this design has several extension points. Pretty much any conceivable
   *    initialisation logic can be embodied in the `_ElmMapper_` template.
   *    Required is that the concrete instance is _constructible_ from the source type
   *    and _convertible_ to the individual member type of the target tuple it is
   *    invoked for. Source data _must_ be taken by-value, from the ctor argument.
   * @note based on this mechanics, a generic _element extractor_ may be built,
   *    selecting a (partial) specialisation based on the source type given.
   * @see ExtractArg
   */
  template< typename TYPES
          , template<class,class, size_t> class _ElmMapper_
          >
  struct TupleConstructor
    : Tuple<TYPES>
    {
      /** meta-sequence to drive instantiation of the ElmMapper */
      using SequenceIterator = BuildIdxIter<TYPES>::Ascending;
      
      template<size_t idx, class SRC>
      static auto
      mapElm (SRC&& init)   ///< initialise an instance of the element-mapper
        {
          return _ElmMapper_<std::decay_t<SRC>
                            ,Tuple<TYPES>
                            , idx
                            >{std::forward<SRC> (init)};
        }
      
    protected:
      template<class SRC, size_t...idx>
      TupleConstructor (SRC&& initVals, IndexSeq<idx...>)
        : Tuple<TYPES> {mapElm<idx> (std::forward<SRC>(initVals)) ...}
        { }
      
      
    public:
      template<class SRC>
      TupleConstructor (SRC&& values)
        : TupleConstructor (std::forward<SRC>(values), SequenceIterator())
        { }
    };
  
  
  /**
   * Generic converter to somehow extract values from the "source"
   * type to fill and initialise a tuple of given target type.
   * @note to be specialised. The concrete specialisation is
   *       assumed to provide a _member template_ `Access<size_t>`,
   *       which in turn picks and converts the value for the n-th
   *       tuple element.
   */
  template<class SRC, class TAR>
  struct ElementExtractor;
  

  template<class SRC, class TAR, size_t i>
  using ExtractArg = ElementExtractor<SRC, TAR>::template Access<i>;
  
  
  /**
   * convenience shortcut to build a tuple from some suitable source data.
   * For this to work, there needs to be a partial specialisation for
   * (\ref ElementExtractor) to deal with the concrete source type given.
   * @note we provide such a specialisation for `Record<GenNode>`, which
   *       allows us to fill an (argument) tuple from a sequence of generic
   *       data values, with run-time type compatibility check.
   * @see tuple-record-init.hpp
   */
  template<typename TYPES, class SRC>
  Tuple<TYPES>
  buildTuple (SRC&& values)
  {
    return TupleConstructor<TYPES, ExtractArg>{std::forward<SRC> (values)};
  }


  
  
  
  
  
  
  /**
   * Decorating a tuple type with auxiliary data access operations.
   * This helper template builds up a subclass of the given TUP (base) type
   * (which is assumed to be a Tuple or at least need to be copy constructible
   * from `Tuple<TYPES>` ). The purpose is to use the Tuple as storage record, but
   * to add a layer of access functions, which in turn might rely on the exact
   * type of the individual elements within the Tuple. To achieve this, for each
   * type within the Tuple, the TUP type is decorated with an instance of the
   * template passed in as template template parameter _X_. Each of these
   * decorating instances is provided with an index number, allowing to
   * access "his" specific element within the underlying tuple.
   * 
   * The decorating template _X_ need to take its own base class as template
   * parameter. Typically, operations on _X_ will be defined in a recursive fashion,
   * calling down into this templated base class. To support this, an instantiation
   * of _X_ with the empty type sequence is generated for detecting recursion end
   * (built as innermost decorator, i.e. the immediate subclass of TUP)
   */
  template
    < template<class,class,class, uint> class _X_   ///< user provided template<Type, Base, TupleType, arg-idx>
    , typename TYPES                                ///< Sequence of types to use within the Accessor
    , class TUP =Tuple<TYPES>                       ///< the tuple type to build on
    , uint i = 0                                    ///< tuple element index counter
    >
  class BuildTupleAccessor
    {
      // prepare recursion...
      using Head         = Split<TYPES>::Head;
      using Tail         = Split<TYPES>::Tail;
      using NextBuilder  = BuildTupleAccessor<_X_, Tail,TUP, i+1>;
      using NextAccessor = NextBuilder::Product;
    public:
      
      /** type of the product created by this template.
       *  Will be a subclass of TUP */
      using Product = _X_< Head            // the type to use for this accessor
                         , NextAccessor    // the base type to inherit from
                         , TUP             // the tuple type we build upon
                         , i               // current element index
                         >;
    };
  
  
  template
    < template<class,class,class, uint> class _X_
    , class TUP
    , uint i
    >
  class BuildTupleAccessor< _X_, Types<>, TUP, i>
    {
    public:
      using Product = _X_<Nil, TUP, TUP, i>;   // Note: i == tuple size
    };
  
  
  
  /**
   * Helper to dump tuple contents.
   * Defined to act as "Accessor" for BuildTupleAccessor, this helper template
   * allows to create a recursive operation to invoke string conversion on
   * all elements within any given tuple.
   */
  template
    < typename TY
    , class BASE
    , class TUP
    , uint idx
    >
  struct TupleElementDisplayer
    : BASE
    {
      using BASE::BASE;
      
      std::string
      dump (std::string const& prefix ="(")  const
        {
          return BASE::dump (prefix + util::toString(std::get<idx>(*this))+",");
        }
    };
  
  template<class TUP, uint n>
  struct TupleElementDisplayer<Nil, TUP, TUP, n>
    : TUP
    {
      TupleElementDisplayer (TUP const& tup)
        : TUP(tup)
        { }
      
      std::string
      dump (std::string const& prefix ="(")  const
        {
          if (1 < prefix.length())
            // remove the trailing comma
            return prefix.substr (0, prefix.length()-1) +")";
          else
            return prefix+")";
        }
    };
  
  
  /**
   * convenience function to dump a given tuple's contents.
   * Using the BuildTupleAccessor, we layer a stack of Instantiations of
   * the TupleElementDisplayer temporarily on top of the given tuple,
   * just to invoke a recursive call chain through these layers
   * and get a string representation of each element in the
   * tuple.
   */
  template<typename...TYPES>
  inline std::string
  dump (std::tuple<TYPES...> const& tuple)
  {
    using BuildAccessor = BuildTupleAccessor<TupleElementDisplayer, Types<TYPES...>>;
    using Displayer     = BuildAccessor::Product ;
    
    return static_cast<Displayer const&> (tuple)
          .dump();
  }
  
  
  
}} // namespace lib::meta


// add a specialisation to enable tuple string conversion
namespace util {
  
  template<typename...TYPES>
  struct StringConv<std::tuple<TYPES...>>
    {
      static std::string
      invoke (std::tuple<TYPES...> const& tuple) noexcept
        try {
          return "«"+typeStr(tuple)
               + "»──" + lib::meta::dump (tuple);
        }
        catch(...) { return FAILURE_INDICATOR; }
    };
  
  
} // namespace util
#endif /*LIB_META_TUPLE_ACCESSOR_H*/
