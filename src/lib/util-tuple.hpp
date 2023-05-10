/*
  UTIL-TUPLE.hpp  -  helpers and convenience shortcuts for working with tuples

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file util-tuples.hpp
 ** Some small helpers and convenience shortcuts to simplify working with
 ** tuples and sequences (given by iterator). While tuples and sequences
 ** are fundamentally different insofar a tuple has a fixed structure (and
 ** may hold elements of different type), sometimes it can be convenient to
 ** treat a tuple like a sequence (especially a tuple holding elements of a
 ** single type. Notably, an iterator can be unloaded into a fixed-size tuple,
 ** which in turn can than be used in a structural binding to unpack references
 ** to the elements into scoped variables. Obviously, the meaning of the elements
 ** in the sequence must be fixed and predetermined -- which is often the case
 ** when dealing with tests or communication protocols.
 ** 
 ** @see util-tuple-test.cpp
 ** @see util-coll.hpp
 ** 
 */


#ifndef UTIL_TUPLE_H
#define UTIL_TUPLE_H


#include <tuple>
#include <utility>



namespace util {
  
  namespace { // recursive builder helper to unpack a sequence...
    
    template<size_t N>
    using cnt_ = std::integral_constant<size_t, N>;
    
    template<class SEQ>
    inline auto
    _buildSeqTuple (cnt_<0>, SEQ&&)
    {
      return std::tuple<>{};
    }
    
    template<size_t N, class SEQ>
    inline auto
    _buildSeqTuple (cnt_<N>, SEQ&& iter)
    {
      auto prefixTuple = std::tie (*iter);
      ++iter;
      return std::tuple_cat (prefixTuple, _buildSeqTuple (cnt_<N-1>{}, std::forward<SEQ> (iter)));
    }
  }//(End) unpacking helper
  
  
  /**
   * Unpack an iterator to build a fixed-size std::tuple of references
   * @tparam N (mandatory) defines the number of elements to unpack; can be zero
   * @param iter anything compliant to the Lumiera Forward Iterator protocol
   * @warning since the implementation uses `std::tie (*iter)`, a _reference_ is
   *     stored, which may lead to strange and dangerous behaviour if the given
   *     iterator exposes a reference to mutable internal state (e.g. "state core").
   *     Moreover, it is assumed the iterator yields enough values to fill the new
   *     tuple, and this is not checked; an empty or exhausted iterator might throw,
   *     or yield otherwise undefined behaviour.
   */
  template<size_t N, class SEQ>
  auto
  seqTuple (SEQ&& iter)
  {
    return _buildSeqTuple (cnt_<N>{}, std::forward<SEQ> (iter));
  }
  
  
} // namespace util
#endif /*UTIL_TUPLE_H*/
