/*
  TypeDemangling(Test)  -  ensure a helper for C++ demangling works as expected

   Copyright (C)
     2014,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file type-demangling-test.cpp
 ** unit test \ref TypeDemangling_test
 */


#include "lib/symbol.hpp"
#include "lib/test/run.hpp"
#include "lib/meta/util.hpp"

#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;


namespace lib {
namespace meta{
namespace test{
  
  template<class T>
  struct Outer
    {
      struct Inner { };
      
      static const T*
      cloak (Inner const&&)
        {
          return nullptr;
        }
    };
  
  struct Space { };
  
  
  
  
  /**********************************************//**
   * @test verify the demangling of C++ names,
   *       as available through the GCC platform ABI.
   *       The Lumiera support library exposes this
   *       non-portable feature through a convenience
   *       helper to ease the writing of unit tests.
   *       
   * @see test-helper.hpp
   */
  class TypeDemangling_test : public Test
    {
      void
      run (Arg)
        {
          Outer<Space> ship;
          auto magic = &ship.cloak;
          auto rawType = typeid(magic).name();
          
          cout << rawType << endl;
          cout << demangleCxx(rawType) << endl;
        }
    };
  
  LAUNCHER (TypeDemangling_test, "unit common");
  
  
}}} // namespace lib::meta::test
