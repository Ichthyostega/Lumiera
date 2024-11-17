/*
  CalcStream(Test)  -  detailed coverage of engine calculation streams

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file calc-stream-test.cpp
 ** unit test \ref CalcStream_test
 */


#include "lib/test/run.hpp"
#include "lib/error.hpp"

#include "steam/engine/engine-service.hpp"

//#include <ctime>

using test::Test;
//using std::rand;


namespace steam  {
namespace engine{
namespace test  {

  namespace { // test fixture...
    
  } // (End) test fixture
  
  
  
  /***************************************************************//**
   * @test detailed coverage of the various CalcStream flavours
   *       supported by the render engine interface.
   * 
   * @todo WIP-WIP-WIP
   * 
   * @see EngineInterface_test for the plain flat standard case
   * @see EngineService
   * @see CalcStream
   * @see OutputSlotProtocol_test
   */
  class CalcStream_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          UNIMPLEMENTED ("in-depth coverage of calculation streams");
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (CalcStream_test, "function engine");
  
  
  
}}} // namespace steam::engine::test
