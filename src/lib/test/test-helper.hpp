/*
  TEST-HELPER.hpp  -  collection of functions supporting unit testing
 
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


#ifndef LIB_TEST_TESTHELPER_H
#define LIB_TEST_TESTHELPER_H


#include "include/symbol.hpp"

#include <typeinfo>
#include <string>



namespace lib {
namespace test{
  
  using lumiera::Symbol;
  using std::string;
  
  
  
  /** get a sensible display for a type or object 
   *  @param obj object of the type in question
   *  @param name optional name to be used literally
   *  @return either the literal name without any further magic,
   *          or the result of compile-time or run time 
   *          type identification as implemented by the compiler.
   */
  template<typename T>
  const char*
  showType (T const& obj, Symbol name=0)
  {
    return name? name : typeid(obj).name();
  }
  
  /** get a sensible display for a type 
   *  @param name optional name to be used literally
   *  @return either the literal name without any further magic,
   *          or the result of compile-time or run time 
   *          type identification as implemented by the compiler.
   */
  template<typename T>
  const char*
  showType (Symbol name=0)
  {
    return name? name : typeid(T).name();
  }
  
  
  /** for printing sizeof().
   *  prints the given size and name litterally, without any further magic */
  string
  showSizeof (size_t siz, Symbol name);
  
  /** for printing sizeof(), trying to figure out the type name automatically */
  template<typename T>
  string
  showSizeof(Symbol name=0)
  {
    return showSizeof (sizeof (T), showType<T> (name));
  }
  
  template<typename T>
  string
  showSizeof(T const& obj, Symbol name=0)
  {
    return showSizeof (sizeof (obj), showType (obj,name));
  }
  
  template<typename T>
  string
  showSizeof(T *obj, Symbol name=0)
  {
    return obj? showSizeof (*obj, name)
              : showSizeof<T> (name);
  }
  
  
}} // namespace lib::test
#endif
