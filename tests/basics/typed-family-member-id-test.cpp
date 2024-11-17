/*
  TypedFamilyMemberID(Test)  -  verify type based member ID

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file typed-family-member-id-test.cpp
 ** Unit test to verify generation of a Member ID within a type based family of objects.
 */



#include "lib/test/run.hpp"
#include "lib/typed-counter.hpp"


namespace lib {
namespace test{
  
  
  
  
  /*******************************************************************//**
   * @test simplistic unit test to demonstrate generating _member IDs
   *       within a family of objects delineated by type.
   * @warning this test does not cover the thread safety,
   *       because FamilyMember relies on lib::ClassLock,
   *       which is assumed to be covered separately
   * 
   * @see SessionCommandFunction_test::perform_massivelyParallel
   * @see typed-counter.hpp
   */
  class TypedFamilyMemberID_test : public Test
    {
      
      void
      run (Arg)
        {
          struct X { };
          struct Y { };
          struct Z { };
          
          CHECK (0 == FamilyMember<X>{});
          CHECK (1 == FamilyMember<X>{});
          CHECK (2 == FamilyMember<X>{});
          
          CHECK (0 == FamilyMember<Y>{});
          CHECK (1 == FamilyMember<Y>{});
          
          CHECK (0 == FamilyMember<Z>{});
          
          CHECK (2 == FamilyMember<Y>{});
          CHECK (3 == FamilyMember<Y>{});
          
          CHECK (1 == FamilyMember<Z>{});
          CHECK (2 == FamilyMember<Z>{});
          CHECK (3 == FamilyMember<Z>{});
          CHECK (4 == FamilyMember<Z>{});
          
          CHECK (4 == FamilyMember<Y>{});
          CHECK (5 == FamilyMember<Z>{});
          CHECK (3 == FamilyMember<X>{});
          CHECK (5 == FamilyMember<Y>{});
          CHECK (4 == FamilyMember<X>{});
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TypedFamilyMemberID_test, "unit common");
  
  
}} // namespace lib::test
