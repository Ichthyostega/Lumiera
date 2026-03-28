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
 ** Helper templates to access parts of a tuple.
 ** 
 ** @todo 2026 this is old metaprogramming code, largely obsoleted by newer features of the C++ language.
 **       However, since this code is still used in some basic support facilities, it was separated out,
 **       so that it can be refactored as time permits.
 ** 
 ** @see TupleHelper_test
 ** 
 */


#ifndef LIB_META_TUPLE_ACCESSOR_H
#define LIB_META_TUPLE_ACCESSOR_H

#include "lib/meta/tuple-helper.hpp"
#include "lib/format-obj.hpp"

#include <string>

namespace lib {
namespace meta {
  
  
  
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
