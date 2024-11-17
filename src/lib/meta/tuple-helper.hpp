/*
  TUPLE-HELPER.hpp  -  metaprogramming utilities for type and data tuples

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file tuple-helper.hpp
 ** Metaprogramming with tuples-of-types and the `std::tuple` record.
 ** The metaprogramming part of this header complements typelist.hpp and allows
 ** some additional manipulations on type sequences, especially to integrate
 ** with the Tuples provided by the standard library.
 ** 
 ** @warning the metaprogramming part of Lumiera to deal with type sequences is in a
 **          state of transition, since C++11 now offers direct language support for
 **          processing of flexible template parameter sequences ("parameter packs").
 **          It is planned to regroup and simplify our homemade type sequence framework
 **          to rely on variadic templates and integrate better with std::tuple.
 **          It is clear that we will _retain some parts_ of our own framework,
 **          since programming with _Loki-style typelists_ is way more obvious
 **          and straight forward than handling of template parameter packs,
 **          since the latter can only be rebound through pattern matching.
 ** @todo transition lib::meta::Types to variadic parameters  /////////////////////////////////TICKET #987
 ** 
 ** @see control::CommandDef usage example
 ** @see TupleHelper_test
 ** @see typelist.hpp
 ** @see function.hpp
 ** @see generator.hpp
 ** 
 */


#ifndef LIB_META_TUPLE_HELPER_H
#define LIB_META_TUPLE_HELPER_H

#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-util.hpp"
#include "lib/meta/typeseq-util.hpp"
#include "lib/meta/variadic-helper.hpp"
#include "lib/meta/util.hpp"

#include <tuple>


namespace util { // forward declaration
  
  template<typename TY>
  std::string
  toString (TY const& val)  noexcept;
}


namespace lib {
namespace meta {
  
  /**
   * Helper: perform some arbitrary operation on each element of a tuple.
   * @note the given functor must be generic, since each position of the tuple
   *       may hold a data element of different type.
   * @remark credits to David Vandevoorde (member of C++ committee) for using
   *       std::apply to unpack the tuple's contents into an argument pack and
   *       then employ a fold expression with the comma operator.
   */
  template<class FUN, typename...ELMS>
  void forEach (std::tuple<ELMS...>&& tuple, FUN fun)
  {
      std::apply ([&fun](auto&... elms)
                       {
                         (fun(elms), ...);
                       }
                 ,tuple);
  }
  
  
  
  namespace { // rebinding helper to create std::tuple from a type sequence
    
    template<typename SEQ>
    struct BuildTupleType
      : std::false_type
      { };
    
    template<typename...TYPES>
    struct BuildTupleType<TySeq<TYPES...>>
      {
        using Type = std::tuple<TYPES...>;
      };
    
    /**
     * temporary workaround: strip trailing NullType entries
     * prior to rebinding to the `std::tuple` type.
     */
    template<typename...TYPES>
    struct BuildTupleType<Types<TYPES...>>
      {
        using VariadicSeq = typename StripNullType<Types<TYPES...>>::Seq;
        
        using Type = typename BuildTupleType<VariadicSeq>::Type;
      };
    
    template<class H, typename TAIL>
    struct BuildTupleType<Node<H, TAIL>>
      {
        using Seq  = typename Types< Node<H,TAIL>>::Seq;
        using Type = typename BuildTupleType<Seq>::Type;
      };
    
    template<>
    struct BuildTupleType<NullType>
      {
        using Type = typename BuildTupleType<Types<>>::Type;
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
  using Tuple = typename BuildTupleType<TYPES>::Type;
  
  
  using std::tuple_size;
  using std::tuple_element;
  
  
  
  /** temporary workaround: match and rebind the type sequence from a tuple */
  template<typename...TYPES>
  struct RebindTupleTypes
    {
      using Seq  = typename Types<TYPES...>::Seq;
      using List = typename Seq::List;
    };
  template<typename...TYPES>
  struct RebindTupleTypes<std::tuple<TYPES...>>
    {
      using Seq  = typename Types<TYPES...>::Seq;
      using List = typename Seq::List;
    };
  
  
  /** trait to detect tuple types */
  template<typename T>
  struct is_Tuple
    : std::false_type
    { };
  
  template<typename...TYPES>
  struct is_Tuple<std::tuple<TYPES...>>
    : std::true_type
    { };
  
  
  
  
  
  
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
   *    initialisation logic can be embodied in the `_ElmMapper_` template. The sole
   *    requirement is that the concrete instance is _assignable_ by the source type
   *    and _convertible_ to the individual member type of the target tuple it is
   *    invoked for. Moreover, it is possible to build a generic _element extractor_,
   *    which will be specialised on base of the source type accepted.
   * @see ExtractArg
   */
  template< typename TYPES
          , template<class,class, size_t> class _ElmMapper_
          >
  struct TupleConstructor
    : Tuple<TYPES>
    {
      /** meta-sequence to drive instantiation of the ElmMapper */
      using SequenceIterator = typename BuildIdxIter<TYPES>::Ascending;
      
    protected:
      template<class SRC, size_t...idx>
      TupleConstructor (SRC values, IndexSeq<idx...>)
        : Tuple<TYPES> (_ElmMapper_<SRC, Tuple<TYPES>, idx>{values}...)
        { }
      
      
    public:
      template<class SRC>
      TupleConstructor (SRC values)
        : TupleConstructor (values, SequenceIterator())
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
  using ExtractArg = typename ElementExtractor<SRC, TAR>::template Access<i>;
  
  
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
  buildTuple (SRC values)
  {
    return TupleConstructor<TYPES, ExtractArg> (values);
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
      using Head         = typename Split<TYPES>::Head;
      using Tail         = typename Split<TYPES>::Tail;
      using NextBuilder  = BuildTupleAccessor<_X_, Tail,TUP, i+1>;
      using NextAccessor = typename NextBuilder::Product;
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
      using Product = _X_<NullType, TUP, TUP, i>;   // Note: i == tuple size
      
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
  struct TupleElementDisplayer<NullType, TUP, TUP, n>
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
    using Displayer     = typename BuildAccessor::Product ;
    
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
#endif /*LIB_META_TUPLE_HELPER_H*/
