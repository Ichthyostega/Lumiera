/*
  TypeListUtil(Test)  -  check the simple typelist metaprogramming helpers 

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file typelist-util-test.cpp
 ** unit test \ref TypeListUtil_test
 */


#include "lib/test/run.hpp"
#include "lib/meta/typelist-util.hpp"



namespace lib  {
namespace meta {
namespace test {
  
  
  
  using TheList = TySeq< int
                       , uint
                       , int64_t
                       , uint64_t
                       >::List;
  
  using EmptyList = TySeq<  >::List;
  
  
  
  /*********************************************************************//**
   * @test verify the simple helpers for working with lists-of-types.
   *       These are simple metafunctions to count the number of elements,
   *       calculate the maximum size or check for inclusion.
   *       
   *       Because these metafunctions will be computed during compilation,
   *       mostly the function is already verified when it passes compilation.
   *       All we can do here, at runtime, is to verify some of the (expected)
   *       constant results.
   */
  class TypeListUtil_test : public Test
    {
      void
      run (Arg)
        {
          CHECK (4 == count<TheList>()  );
          CHECK (0 == count<EmptyList>());
          
          CHECK (sizeof(int64_t)  == maxSize<TheList>()   );
          CHECK (0                == maxSize<EmptyList>() );
          CHECK (alignof(int64_t) == maxAlign<TheList>()  );
          CHECK (0                == maxAlign<EmptyList>());
          
          CHECK ( bool(isInList<int     , TheList>()));
          CHECK ( bool(isInList<uint    , TheList>()));
          CHECK ( bool(isInList<int64_t , TheList>()));
          CHECK ( bool(isInList<uint64_t, TheList>()));
          
          CHECK (!bool(isInList<double  , TheList>()));
          CHECK (!bool(isInList<int     , EmptyList>()));
          CHECK (!bool(isInList<int     , int>()));         // Note: not-a-typelist yields false too
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TypeListUtil_test, "unit meta");
  
  
  
}}} // namespace lib::meta::test
