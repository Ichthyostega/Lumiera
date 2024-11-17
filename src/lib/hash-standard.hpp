/*
  HASH-STANDARD.hpp  -  allow use both of std and boost hasher implementations

   Copyright (C)
     2014,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
 ** can be found -- which defeats a meta programming solution based on SFINAE.
 ** 
 ** In the Lumiera code base, the habit is to define a free function \c hash_value alongside with
 ** custom data types. This helper uses metaprogramming to generate a bridge from the standard hasher
 ** to use this boost-style custom hash function if applicable. To allow for such an automatic bridge,
 ** we have to work around aforementioned problem with the static assertion. Recent discussion threads
 ** indicate that the GCC and Clang developers are aware of those likely unintended side effects of a
 ** well meant hint for people to implement their own hash functions. AFAIK, the standard lib shipping
 ** with some GCC 4.8.x doesn't contain the assertion anymore; and there are plans to adopt the boost style
 ** extension mechanism and provide such a bridge in the standard library at some point in the future.
 ** 
 ** In the light of this situation, it feels justified to play a dirty trick in order to defeat that
 ** static assertion: in this header, we blatantly hijack the standard library definition of std::hash,
 ** push it aside and plant our own definition instead.
 ** 
 ** @note this trick was proposed by user "enobayram" on Stackoverflow at Oct 5, 2012
 ** http://stackoverflow.com/questions/12753997/check-if-type-is-hashable
 ** 
 ** @warning this header <b>includes and manipulates</b> the standard header `<functional>`.
 **       Please ensure it is always included _before_ the latter. Failing to do so will result
 **       in mysterious failures.
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
#ifdef _FUNCTIONAL_HASH_H
#error "unable to hijack std::hash, since <bits/functional_hash.hpp> has already been included. \
        Please ensure that lib/hash_standard.hpp is included first, before including <string> or <functional>"
#endif


#include "lib/hash-value.h"

#include <cstddef>
#include <utility>


namespace lib {
namespace meta {
  
  struct NoUsableHashDefinition { size_t more_than_one[2]; };
  typedef size_t HasUsableHashDefinition;
  
  NoUsableHashDefinition hash_value(...);  ///< declared for metaprogramming only, never defined
  
  
  /**
   * trait template to detect if some custom type TY
   * provides a boost compliant hash function through ADL
   */
  template<typename TY>
  class provides_BoostHashFunction
    {
      TY const& unusedDummy = *(TY*)nullptr;
      
    public:
      enum{ value = (sizeof(HasUsableHashDefinition) == sizeof(hash_value(unusedDummy))) };
    };

}}



namespace std {
  
  template<typename Result, typename Arg>
  struct __hash_base;
  
  
  template<typename TY, typename TOGGLE = void>
  struct _HashImplementationSelector
    : public __hash_base<size_t, TY>
    {
      static_assert (sizeof(TY) < 0, "No hash implementation found. "
                                     "Either specialise std::hash or provide a boost-style hash_value via ADL.");
      
      // the default provides *no* hash implementation
      // and adds a marker type for metaprogramming
      typedef int NotHashable;
    };
  
  /**
   * Specialisation: Bridge from std::hash to boost::hash
   */
  template<typename TY>
  struct _HashImplementationSelector<TY,   std::enable_if_t< lib::meta::provides_BoostHashFunction<TY>::value >>
    : public __hash_base<size_t, TY>
    {
      size_t
      operator() (TY const& elm) const noexcept
        {
          return hash_value(elm);
        }
    };
  
  
  /**
   * Primary class template for std::hash.
   * We provide no default implementation, but a marker type
   * to allow detection of custom implementation through metaprogramming
   * 
   * @note this definition has been \em hijacked by Lumiera
   *       to add an automatic bridge to use boost::hash functions
   */
  template<typename TY>
  struct hash
    : public _HashImplementationSelector<TY>
    { };
  
}




/* ==== Evil Evil ==== */

#define hash hash_HIDDEN
#define _Hash_impl _Hash_impl_HIDDEN
#include <bits/c++config.h>
#include <bits/functional_hash.h>
#undef hash
#undef _Hash_impl


namespace std {
  
  /* after the manipulation is performed now,
   * we have to do some clean-up: it might happen that
   * other parts of the standard library call directly into the
   * standard Hash implementation (in fact, the hashers for strings,
   * floats and doubles do this). Thus we have to amend this standard
   * implementation, and re-wire it to the original definition.
   */
  
  struct _Hash_impl
    : public std::_Hash_impl_HIDDEN
    {
      template<typename ... ARGS>
      static auto
      hash (ARGS&&... args) -> decltype(hash_HIDDEN (std::forward<ARGS>(args)...))
        {
          return hash_HIDDEN (std::forward<ARGS>(args)...);
        }
    };

  /* and likewise, we have to re-wire all the
   * default hash implementations to the original implementation.
   * This is just a defensive approach; we change a function name,
   * yet we avoid changing any actual library code.
   */
#define STD_HASH_IMPL(_TY_) \
  template<> struct hash<_TY_> : public hash_HIDDEN<_TY_> { };

  STD_HASH_IMPL (bool)
  STD_HASH_IMPL (char)
  STD_HASH_IMPL (signed char)
  STD_HASH_IMPL (unsigned char)
  STD_HASH_IMPL (wchar_t)
  STD_HASH_IMPL (char16_t)
  STD_HASH_IMPL (char32_t)
  STD_HASH_IMPL (short)
  STD_HASH_IMPL (int)
  STD_HASH_IMPL (long)
  STD_HASH_IMPL (long long)
  STD_HASH_IMPL (unsigned short)
  STD_HASH_IMPL (unsigned int)
  STD_HASH_IMPL (unsigned long)
  STD_HASH_IMPL (unsigned long long)
  STD_HASH_IMPL (float)
  STD_HASH_IMPL (double)
  STD_HASH_IMPL (long double)
  
#undef STD_HASH_IMPL
}

#endif /*LIB_HASH_STANDARD_H*/
