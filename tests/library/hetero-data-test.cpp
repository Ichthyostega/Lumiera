/*
  HeteroData(Test)  -  verify maintaining chained heterogeneous data in local storage

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file del-stash-test.cpp
 ** unit test \ref HeteroData_test
 */


#include "lib/test/run.hpp"
#include "lib/hetero-data.hpp"
#include "lib/meta/trait.hpp"
#include "lib/test/diagnostic-output.hpp"/////////////////TODO



namespace lib {
namespace test{
  
  using meta::is_Subclass;
  
  namespace { // probe victims
    
  }//(End) test data
  
  
  
  
  /*************************************************************//**
   * @test maintain a sequence of data tuples in local storage,
   *       providing pre-configured type-safe data access.
   * 
   * @see lib::HeteroData
   * @see NodeBase_test::verify_TurnoutSystem()
   */
  class HeteroData_test : public Test
    {
      
      virtual void
      run (Arg)
        {
//          seedRand();
//          checksum = 0;
          
          checkSingleKill();
        }
      
      
      void
      checkSingleKill ()
        {
          using Block1 = HeteroData<uint,double>;
          CHECK ((is_Subclass<Block1, std::tuple<uint,double>>()));
          
          auto b1 = Block1::build (42, 1.61803);
SHOW_EXPR(b1)
          CHECK (1.61803 == std::get<1> (b1));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (HeteroData_test, "unit common");
  
  
}} // namespace lib::test
