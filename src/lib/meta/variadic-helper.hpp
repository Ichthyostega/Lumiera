/*
  VARIADIC-HELPER.hpp  -  metaprogramming utilities for parameter- and type sequences

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


/** @file variadic-helper.hpp
 ** Metaprogramming with type sequences based on variadic template parameters.
 ** The type rebinding- and helper templates in this header allow to perform
 ** simple sequence manipulations on sequences of template parameters extracted
 ** from variadic parameter packs. The goal is to (pre)process flexible argument
 ** lists _at compile time,_ driven by template instantiation, allowing to specialise
 ** and react specifically on some concrete pattern of argument types.
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


#ifndef LIB_META_VARIADIC_HELPER_H
#define LIB_META_VARIADIC_HELPER_H

#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-util.hpp"
#include "lib/meta/typeseq-util.hpp"
#include "lib/meta/util.hpp"


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
   *    to variadic parameters, this type will be obsoleted.
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
      
      template<size_t d>
      using OffsetBy  = typename BuildIndexSeq<n-1>::template OffsetBy<d>::template AppendElm<n-1+d>;
      
      template<size_t i>
      using FilledWith = typename BuildIndexSeq<n-1>::template FilledWith<i>::template AppendElm<i>;
    };
  
  template<>
  struct BuildIndexSeq<0>
    {
      using Ascending = IndexSeq<>;
      
      template<size_t d>
      using OffsetBy  = IndexSeq<>;
      
      template<size_t>
      using FilledWith = IndexSeq<>;
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
  
  
  
}} // namespace lib::meta
#endif /*LIB_META_VARIADIC_HELPER_H*/
