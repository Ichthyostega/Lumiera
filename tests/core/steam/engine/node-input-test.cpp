/*
  NodeInput(Test)  -  unit test of the source reading render node

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file node-input-test.cpp
 ** unit test \ref NodeInput_test
 */


#include "lib/test/run.hpp"
//#include "lib/util.hpp"


//using std::string;


namespace steam {
namespace engine{
namespace test  {
  
  
  
  
  /***************************************************************//**
   * @test the source reading render node.
   */
  class NodeInput_test : public Test
    {
      virtual void run(Arg) 
        {
          UNIMPLEMENTED ("render node pulling source data from vault");
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeInput_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
