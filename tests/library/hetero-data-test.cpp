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



namespace lib {
namespace test{
  
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
          
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (HeteroData_test, "unit common");
  
  
}} // namespace lib::test
