/*
  NodeFeed(Test)  -  verify render node data feeds

   Copyright (C)
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file node-feed-test.cpp
 ** Feeding into and retrieving data from render nodes is covered by \ref NodeFeed_test.
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
  class NodeFeed_test : public Test
    {
      virtual void run(Arg) 
        {
          UNIMPLEMENTED ("render node pulling source data from vault");
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeFeed_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
