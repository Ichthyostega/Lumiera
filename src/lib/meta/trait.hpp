/*
  TRAIT.hpp  -  type handling and type detection helpers
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/


#ifndef LIB_META_TRAIT_H
#define LIB_META_TRAIT_H


#include "lib/meta/util.hpp"
#include "lib/meta/duck-detector.hpp"
#include "lib/wrapper.hpp"     ////////////////////////TODO only because of AssignableRefWrapper -- can we get rid of this import?

#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/utility/enable_if.hpp>
#include <string>


namespace lib {
namespace meta {
  
  
  
  /** Trait template for detecting if a type can be converted to string.
   *  For example, this allows to write specialisations with the help of
   *  boost::enable_if
   */
  template <typename TY>
  struct can_ToString
    {
      enum { value = boost::is_convertible<TY, std::string>::value
           };
    };
  
  
  /** Trait template for guarding \c lexical_cast<..> expressions.
   *  Such an expression won't even compile for some types, because of
   *  missing or ambiguous output operator(s).
   *  Ideally, there would be some automatic detection (relying on the
   *  existence of an operator<< for the given type. But I couldn't make
   *  this work, so I fell back on just declaring types which are known
   *  to work with lexical_cast to string
   *  @note this compile-time trait can't predict if such an conversion
   *        to string will be successful at runtime; indeed it may throw,
   *        so you should additionally guard the invocation with try-catch!
   */
  template<typename X>
  struct can_lexical2string
    {
      enum { value = boost::is_arithmetic<X>::value
           };
    };
  
  
  
  
  /** Trait template to detect a type usable with the STL for-each loop.
   *  Basically we're looking for the functions to get the begin/end iterator
   */
  template<typename T>
  class can_STL_ForEach
    {
      struct is_iterable
        {
          META_DETECT_NESTED(iterator);
          META_DETECT_FUNCTION(typename X::iterator, begin,(void));
          META_DETECT_FUNCTION(typename X::iterator, end  ,(void));
          
          enum { value = HasNested_iterator<T>::value
                      && HasFunSig_begin<T>::value
                      && HasFunSig_end<T>::value
           };
        };
      
      struct is_const_iterable
        {
          META_DETECT_NESTED(const_iterator);
          META_DETECT_FUNCTION(typename X::const_iterator, begin,(void) const);
          META_DETECT_FUNCTION(typename X::const_iterator, end  ,(void) const);
          
          enum { value = HasNested_const_iterator<T>::value
                      && HasFunSig_begin<T>::value
                      && HasFunSig_end<T>::value
           };
        };
      
      
    public:
      enum { value = is_iterable::value
                  || is_const_iterable::value
           };
    };
  
  
  /** Trait template to detect a type usable immediately as
   *  "Lumiera Forward Iterator" in a specialised for-each loop
   *  This is just a heuristic, based on some common properties
   *  of such iterators; it is enough to distinguish it from an 
   *  STL container, but can certainly be refined.
   */
  template<typename T>
  class can_IterForEach
    {
       
      META_DETECT_NESTED(value_type);
      META_DETECT_OPERATOR_DEREF();
      META_DETECT_OPERATOR_INC();
      
    public:
      enum{ value = boost::is_convertible<T, bool>::value
                 && HasNested_value_type<T>::value
                 && HasOperator_deref<T>::value
                 && HasOperator_inc<T>::value
          };
    };
  
  
  
  /** Type definition helper for pointer and reference types.
   *  Allows to create a member field and to get the basic type
   *  irrespective if the given type is plain, pointer or reference
   */
  template<typename TY>
  struct RefTraits
    {
      typedef TY* pointer;
      typedef TY& reference;
      typedef TY  value_type;
      typedef value_type member_type;
    };
  
  template<typename TY>
  struct RefTraits<TY *>
    {
      typedef TY* pointer;
      typedef TY& reference;
      typedef TY  value_type;
      typedef pointer member_type;
    };
  
  template<typename TY>
  struct RefTraits<TY &>
    {
      typedef TY* pointer;
      typedef TY& reference;
      typedef TY  value_type;
      typedef lib::wrapper::AssignableRefWrapper<TY> member_type;
    };
  //////////////////////////////////////////TODO: member_type not needed anymore 12/09 -- obsolete? useful? keep it?
  
  
}} // namespace lib::meta
#endif
