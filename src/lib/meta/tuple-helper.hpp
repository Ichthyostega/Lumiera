/*
  TUPLE-HELPER.hpp  -  metaprogramming utilities for type and data tuples

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file tuple.hpp
 ** Metaprogramming with tuples-of-types and the `std::tuple` record.
 ** The metaprogramming part of this header complements typelist.hpp and allows
 ** some additional manipulations on type sequences, especially to integrate
 ** with the Tuples provided by the standard library.
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
   * temporary workaround:
   * alternative definition of "type sequence",
   * already using variadic template parameters.
   * @remarks the problem with our existing type sequence type
   *    is that it fills the end of each sequence with NullType,
   *    which was the only way to get a flexible type sequence
   *    prior to C++11. Unfortunately these trailing NullType
   *    entries do not play well with other variadic defs.
   * @deprecated when we switch our primary type sequence type
   *    to variadic parameters, this type will be superfluous.
   */
  template<typename...TYPES>
  struct TySeq
    {
      using Seq = TySeq;
      using List = typename Types<TYPES...>::List;
    };
  
  
  /**
   * temporary workaround: additional specialisation for the template
   * `Prepend` to work also with the (alternative) variadic TySeq.
   * @see typeseq-util.hpp
   */
  template<typename T, typename...TYPES>
  struct Prepend<T, TySeq<TYPES...>>
  {
    using Seq  = TySeq<T, TYPES...>;
    using List = typename Types<T, TYPES...>::List;
  };
  
  
  /**
   * temporary workaround: strip trailing NullType entries from a
   * type sequence, to make it compatible with new-style variadic
   * template definitions.
   * @note the result type is a TySec, to keep it apart from our
   *    legacy (non-variadic) lib::meta::Types
   * @deprecated necessary for the transition to variadic sequences
   */
  template<typename SEQ>
  struct StripNullType;
  
  template<typename T, typename...TYPES>
  struct StripNullType<Types<T,TYPES...>>
    {
      using TailSeq = typename StripNullType<Types<TYPES...>>::Seq;
      
      using Seq = typename Prepend<T, TailSeq>::Seq;
    };
  
  template<typename...TYPES>
  struct StripNullType<Types<NullType, TYPES...>>
    {
      using Seq = TySeq<>;  // NOTE: this causes the result to be a TySeq
    };
  
  
  
  
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
  
  
  
  /** match and rebind the type sequence from a tuple */
  template<typename...TYPES>
  struct Types<std::tuple<TYPES...>>
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
  
  
  
  
  
  /** Hold a sequence of index numbers as template parameters */
  template<size_t...idx>
  struct IndexSeq
    {
      template<size_t i>
      using AppendElm = IndexSeq<idx..., i>;
    };
  
  /** build an `IndexSeq<0, 1, 2, ..., n-1>` */
  template<size_t n>
  struct BuildIndexSeq
    {
      using Ascending = typename BuildIndexSeq<n-1>::Ascending::template AppendElm<n-1>;
      
      template<size_t i>
      using FilledWith = typename BuildIndexSeq<n-1>::template FilledWith<i>::template AppendElm<i>;
    };
  
  template<>
  struct BuildIndexSeq<0>
    {
      using Ascending = IndexSeq<>;
      
      template<size_t>
      using FilledWith = IndexSeq<>;;
    };
  
  
  
  /** build an index number sequence from a structured reference type */
  template<class REF>
  struct IndexIter;
  
  /** build an index number sequence from a type sequence */
  template<typename...TYPES>
  struct IndexIter<Types<TYPES...>>
    {
      /////TODO as long as Types is not variadic (#987), we need to strip NullType here (instead of just using sizeof...(TYPES)
      enum {SIZ = lib::meta::count<typename Types<TYPES...>::List>::value };
      
      using Seq = typename BuildIndexSeq<SIZ>::Ascending;
    };
  
  

  
  
  
  
  
  
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
   * just to invoke a recursive call chain through this layers
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
