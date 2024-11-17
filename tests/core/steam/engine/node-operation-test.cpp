/*
  NodeOperation(Test)  -  verify proper render node operation and calldown

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file node-operation-test.cpp
 ** unit test \ref NodeOperation_test
 */


#include "lib/test/run.hpp"
//#include "lib/util.hpp"


//using std::string;


namespace steam {
namespace engine{
namespace test  {
  
  
  
  
  /***************************************************************//**
   * @test check render node operation modes and collaboration.
   */
  class NodeOperation_test : public Test
    {
      virtual void run(Arg) 
        {
          UNIMPLEMENTED ("operate some render nodes as linked together");
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeOperation_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
