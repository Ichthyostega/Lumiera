/*
  HASH-STANDARD.hpp  -  allow use both of std and boost hasher implementations

  Copyright (C)         Lumiera.org
    2014,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file hash-standard.hpp
 ** Helper to use a single extension point for specialised hash functions.
 ** With the switch to C++11, there are now two competing quasi standard frameworks
 ** for defining individual hasher functions: the \c std::hash functor and \c <boost/functional/hash.hpp>.
 ** The standard library hasher is used by the (new) STL hashtables and requires an explicit
 ** specialisation of \c std::hash<TYPE>, while the boost hash automatically picks up an
 ** free function `hash_value(MyType const&)`. Additionally, Boost ships with a large collection
 ** of predefined hash functions for various combinations of standard containers and custom types.
 ** To add to this venerable mess, the standard hash defines a default specialisation which
 ** triggers a static assertion and halts compilation, in case no custom specialisation
 ** can be found.
 ** 
 ** In the Lumiera code base, the habit is to define a free function \c hash_value alongside with
 ** custom data types. This helper uses metaprogramming to generate a bridge from the standard hasher
 ** to use this boost-style custom hash function if applicable.
 ** 
 ** @todo 4/2014 doesn't work as expected. My suspicion is that in the actual use case (PlacementIndex),
 **       the type providing the hasher is mixed in through inheritance, and the template specialisation
 **       for this base type is not considered on lookup.  ///////TICKET #722 
 ** 
 ** @see HashIndexed
 ** @see LUID
 **
 */



#ifndef LIB_HASH_STANDARD_H
#define LIB_HASH_STANDARD_H

#include "lib/hash-value.h"
#include "lib/meta/util.hpp"

#include <boost/utility/enable_if.hpp>
//#include <boost/functional/hash.hpp>
#include <functional>

namespace lib {
namespace meta {
  
  
  template <typename T>
struct has_boost_hash_subst { typedef void* Type; };



  template<typename TY>
  struct provides_BoostHasher
    {
     template<class X>
        static Yes_t check(typename has_boost_hash_subst<decltype(hash_value(X()))>::Type);
        template<class>
        static No_t  check(...);
        
        enum{ value = (sizeof(Yes_t)==sizeof(check<TY>(nullptr))) }; 
    };

}}

namespace std {

  /////////////////////////////////////////////////////////////////////////TICKET #722 : attempt to provide a generic bridge to use hash_value
  //
  // this was my first attempt 4/2014, but doesn't work, probably because of the actual ID being inherited
  // see also the attempt at the bottom of hash-indexed.hpp
  
  
  template<typename TY>
  struct hash<           boost::enable_if<lib::meta::provides_BoostHasher<TY>,
              TY > >
  {
    size_t
    operator() (TY const& val)  const noexcept
      {
        return hash_value(val);
      }
  };
  
  
}
#endif
