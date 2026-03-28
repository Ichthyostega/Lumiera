/*
  TUPLE-HELPER.hpp  -  metaprogramming utilities for type and data tuples

   Copyright (C)
     2016,2025        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file tuple-helper.hpp
 ** Metaprogramming with tuples-of-types and the `std::tuple` record.
 ** This header complements typelist.hpp and provides a bridge from type sequences
 ** to the tuple type provided by the standard library, including traits and
 ** helpers to build tuple types from metaprogramming and to pretty-print tuples.
 ** 
 ** Notably the handling of tuples is based on a **Concept** `tuple_like`,
 ** that is satisfied for any type in compliance with the »[tuple protocol]«.
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
 ** [tuple protocol]: https://en.cppreference.com/w/cpp/language/structured_binding.html#Case_2:_binding_a_type_implementing_the_tuple_operations
 ** @see control::CommandDef usage example
 ** @see TupleHelper_test
 ** @see typelist.hpp
 ** @see function.hpp
 ** @see generator.hpp
 ** 
 */


#ifndef LIB_META_TUPLE_HELPER_H
#define LIB_META_TUPLE_HELPER_H

#include "lib/integral.hpp"
#include "lib/meta/tuple-concept.hpp"
#include "lib/meta/variadic-helper.hpp"
#include "lib/meta/typeseq-util.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-util.hpp"
#include "lib/meta/util.hpp"

#include <tuple>
#include <utility>
#include <functional>


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
  template<typename TYPES
          ,template<class, class, unsigned> class _ElmMapper_
          >
  struct TupleConstructor
    : Tuple<TYPES>
    {
      /** meta-sequence to drive instantiation of the ElmMapper */
      using SequenceIterator = BuildIdxIter<TYPES>::Ascending;
      
      template<unsigned idx, class SRC>
      using ElmMapperFor = _ElmMapper_<std::decay_t<SRC>, Tuple<TYPES>, idx>;
      
      template<unsigned idx, class SRC>
      static auto
      mapElm (SRC&& init)   ///< initialise an instance of the element-mapper
        {
          return ElmMapperFor<idx,SRC>{std::forward<SRC> (init)};
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
  
  
}} // namespace lib::meta
#endif /*LIB_META_TUPLE_HELPER_H*/
