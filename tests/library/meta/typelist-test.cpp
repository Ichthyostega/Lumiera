/*
  TypeList(Test)  -  check the typelist metaprogramming facility

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file typelist-test.cpp
 ** unit test \ref TypeList_test
 */


#include "lib/test/run.hpp"
#include "lib/meta/typelist.hpp"

#include <iostream>

using std::string;
using std::cout;


namespace lib  {
namespace meta {
namespace test {
      
      template<int I>
      struct Block    
        { 
          Block() { cout << "- "<<I<<" -"; }  
        };
      struct Zero 
        { 
          Zero()  { cout << "- The End -"; }
        };
      
      
      typedef Types< Block<1>
                   , Block<2>
                   , Block<3>
                   , Block<5>
                   , Block<8>
                   , Block<13>
                   >::List     TheList;
      
      
      template<class X, class P>
      class Chain 
        : X, P
      { }
      ;
      template<class H, class T, class P>
      class Chain<Node<H,T>, P>
        : H, Chain<T,P>
      { }
      ;
      
      typedef Chain<TheList,Zero> AssembledClass;
      
      /*********************************************************************//**
       * @test try out Loki-style typelist metaprogramming.
       *       Assembles a Class definition from parts specified as collection
       *       of types. The elementary building block has an talking ctor, so
       *       by automatic ctor chaining we get a message of each part.
       */
      class TypeList_test : public Test
        {
          void
          run (Arg) 
            {
              AssembledClass wow_me_haz_numbers;
              
              cout << "\n..Size of = " 
                   << sizeof(wow_me_haz_numbers) <<"\n";
            } 
        };
      
      
      /** Register this test class... */
      LAUNCHER (TypeList_test, "unit common");
      
      
      
}}} // namespace lib::meta::test
