/*
  BOOL-CHECKABLE.hpp  -  mixin template for defining a safe conversion to bool
 
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


/** @file bool-checkable.hpp
 ** When working with generic function objects and function pointers typed to
 ** arbitrary signatures, often there is the necessity to hold onto such a functor
 ** while hiding the actual signature behind an common interface ("type erasure").
 ** The usual solution based on subclassing has the downside of requiring separate
 ** storage for the concrete functor object, which might become problematic when
 ** dealing with lots of functor objects. 
 ** 
 ** Especially when dealing with tr1::function objects, all of the type differences
 ** are actually encoded into 3 internal pointers, thus yielding the same size for
 ** all various types of functors. Building on this observation, we can create an
 ** common container object to store the varying functors inline, while hiding the
 ** actual signature.
 ** 
 ** There remains the problem of re-accessing the concrete functor later on. As
 ** C++ has only rudimental introspection capabilities, we can only rely on the
 ** usage context to provide the correct function signature; only when using a
 ** virtual function for the re-access, we can perform at least a runtime-check.
 ** 
 ** Thus there are various flavours for actually implementing this idea, and the
 ** picking a suitable implementation depends largely on the context. Thus we
 ** provide a common and expect the client code to pick an implementation policy.
 ** 
 ** @see control::Mutation usage example
 ** @see function-erasure-test.cpp
 ** 
 */


#ifndef LIB_BOOL_CHECKABLE_H
#define LIB_BOOL_CHECKABLE_H



namespace lib {
  
  namespace imp {
    struct Dummy {};
  }
  
  
  /*
   * Provide an implicit conversion to "bool".
   * Inherit (mix-in) from this template by providing the
   * concrete subclass type ("CRTP"). Additionally, to implement
   * the specific logic of this bool check, the subclass using this
   * template must provide a public function \c isValid() 
   * 
   * \par safe bool conversion
   * Generally speaking, a direct automatic conversion to bool would be 
   * a dangerous feature, because bool is considered an "integral type" in C.
   * Thus, you'd not only get the ability to write very expressive checks
   * like \c if(object) -- you would also get automatic conversions to int
   * where you'd never expect them. In C++ there is a well-known idiom to
   * circumvent this problem. Unfortunately, this idiom isn't easy to 
   * grasp, involving the convoluted syntax of member pointers.
   * So we try to hide away these details into a mixin.
   * 
   * The idea this implementation is based on is the fact that a 
   * pointer-to-member is "somewhat a pointer" but not \em really a pointer.
   * Similar to a pointer, a member pointer can be \em unbound, and this
   * is the property we exploit here. But implementation-wise, a member pointer
   * is a type information plus an offset into this type, and thus can't be
   * converted to an integral type. Thus, depending on the result of the
   * \c isValid() function, the conversion operator returns either a
   * bound or unbound member pointer. 
   * 
   * @see bool-checkable-test.cpp
   * @see control::Mutation usage example 
   */
  template< class T                 ///< the target type implementing \c isValid()
          , class PAR = imp::Dummy ///<  optional parent for inheritance chain
          >
  struct BoolCheckable
    : PAR
    {
      typedef bool (T::*ValidityCheck)()  const;
      typedef ValidityCheck _unspecified_bool_type;
      
      /** implicit conversion to "bool" */ 
      operator _unspecified_bool_type()  const   ///< never throws
        {
          ValidityCheck isValid (&T::isValid);
          T const& obj = static_cast<T const&> (*this);
          
          return  (obj.*isValid)()? isValid : 0;
        }
      
      bool operator! ()  const ///< never throws
        {
          ValidityCheck isValid (&T::isValid);
          T const& obj = static_cast<T const&> (*this);
          
          return !(obj.*isValid)();
        }
      
    };
  
  
  
  
} // namespace lib
#endif
