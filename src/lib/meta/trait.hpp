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


#ifndef LUMIERA_META_TRAIT_H
#define LUMIERA_META_TRAIT_H


#include "lib/meta/util.hpp"

#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/utility/enable_if.hpp>
#include <string>

  
namespace lumiera {
namespace typelist {

  using boost::enable_if;
  using boost::is_arithmetic;
  
  
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
      enum { value = is_arithmetic<X>::value
           };
    };
  
  
}} // namespace lumiera::typelist
#endif
