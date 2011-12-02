/*
  TRAIT.hpp  -  type handling and type detection helpers

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


#ifndef LIB_META_TRAIT_H
#define LIB_META_TRAIT_H


#include "lib/meta/util.hpp"
#include "lib/meta/duck-detector.hpp"

#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/utility/enable_if.hpp>
#include <string>

//Forward declarations for the Unwrap helper....
namespace boost{
  template<class X> class reference_wrapper; 
}
namespace std {
namespace tr1 {
  template<class X> class reference_wrapper; 
  template<class X> class shared_ptr; 
}}
namespace lib{
  template<class X, class B>  class P; 
}
namespace mobject{
  template<class X, class B>  class Placement; 
}


namespace lib {
namespace meta {
  
  
  
  /** 
   * Helper for type analysis:
   * tries to extract a base type from various wrappers.
   * Additionally allows to extract/deref the wrapped element.
   * @warning strips away any const
   */
  template<typename X>
  struct Unwrap
    {
      typedef X Type;
      
      static  X&
      extract (X const& x)
        {
          return const_cast<X&> (x);
        }
    };
  
  template<typename X>
  struct Unwrap<X*>
    {
      typedef typename boost::remove_cv<X>::type Type;
      
      static Type&
      extract (const X* ptr)
        {
          ASSERT (ptr);
          return const_cast<Type&> (*ptr);
        }
    };
  
  template<typename X>
  struct Unwrap<boost::reference_wrapper<X> >
    {
      typedef X  Type;
      
      static X&
      extract (boost::reference_wrapper<X> wrapped)
        {
          return wrapped;
        }
    };
  
  template<typename X>
  struct Unwrap<std::tr1::reference_wrapper<X> >
    {
      typedef X  Type;
      
      static X&
      extract (std::tr1::reference_wrapper<X> wrapped)
        {
          return wrapped;
        }
    };
  
  template<typename X>
  struct Unwrap<std::tr1::shared_ptr<X> >
    {
      typedef X  Type;
      
      static X&
      extract (std::tr1::shared_ptr<X> ptr)
        {
          ASSERT (ptr);
          return *ptr;
        }
    };
  
  template<typename X, class B>
  struct Unwrap<P<X, B> >
    {
      typedef X  Type;
      
      static X&
      extract (P<X,B> ptr)
        {
          ASSERT (ptr);
          return *ptr;
        }
    };
  
  template<typename X, class B>
  struct Unwrap<mobject::Placement<X, B> >
    {
      typedef X  Type;
      
      static X&
      extract (mobject::Placement<X,B> placement)
        {
          ASSERT (placement.isValid());
          return *placement;
        }
    };
  
  
  /** convenience shortcut: unwrapping free function.
   *  @return reference to the bare element.
   *  @warning this function is dangerous: it strips away
   *           any managing smart-ptr and any const!
   *           You might even access and return a
   *           reference to an anonymous temporary.
   */
  template<typename X>
  typename Unwrap<X>::Type&
  unwrap (X const& wrapped)
  {
    return Unwrap<X>::extract(wrapped);
  }
  
  
  
  
  /** Helper for type analysis: tries to strip all kinds of type adornments */
  template<typename X>
  struct Strip
    {
      typedef typename boost::remove_cv<X>                 ::type TypeUnconst;
      typedef typename boost::remove_reference<TypeUnconst>::type TypeReferred;
      typedef typename boost::remove_pointer<TypeReferred> ::type TypePointee;
      typedef typename boost::remove_cv<TypePointee>       ::type TypePlain;
      
      typedef typename Unwrap<TypePlain>                   ::Type Type;
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
    };
  
  template<typename TY>
  struct RefTraits<TY *>
    {
      typedef TY* pointer;
      typedef TY& reference;
      typedef TY  value_type;
    };
  
  template<typename TY>
  struct RefTraits<TY &>
    {
      typedef TY* pointer;
      typedef TY& reference;
      typedef TY  value_type;
    };
  
  
  
  
  
  
  
  
  
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
      typedef typename Strip<T>::Type Type;
       
      struct is_iterable
        {
          META_DETECT_NESTED(iterator);
          META_DETECT_FUNCTION(typename X::iterator, begin,(void));
          META_DETECT_FUNCTION(typename X::iterator, end  ,(void));
          
          enum { value = HasNested_iterator<Type>::value
                      && HasFunSig_begin<Type>::value
                      && HasFunSig_end<Type>::value
           };
        };
      
      struct is_const_iterable
        {
          META_DETECT_NESTED(const_iterator);
          META_DETECT_FUNCTION(typename X::const_iterator, begin,(void) const);
          META_DETECT_FUNCTION(typename X::const_iterator, end  ,(void) const);
          
          enum { value = HasNested_const_iterator<Type>::value
                      && HasFunSig_begin<Type>::value
                      && HasFunSig_end<Type>::value
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
      typedef typename Strip<T>::Type Type;
       
      META_DETECT_NESTED(value_type);
      META_DETECT_OPERATOR_DEREF();
      META_DETECT_OPERATOR_INC();
      
    public:
      enum{ value = boost::is_convertible<Type, bool>::value
                 && HasNested_value_type<Type>::value
                 && HasOperator_deref<Type>::value
                 && HasOperator_inc<Type>::value
          };
    };
  
  
}} // namespace lib::meta
#endif
