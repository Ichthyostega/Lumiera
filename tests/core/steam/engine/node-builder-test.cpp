/*
  NodeBuilder(Test)  -  creation and setup of render nodes

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file node-builder-test.cpp
 ** unit test \ref NodeBuilder_test
 */


#include "lib/test/run.hpp"
#include "steam/engine/node-builder.hpp"
//#include "lib/util.hpp"


using std::string;


namespace steam {
namespace engine{
namespace test  {
  
  
  
  
  /***************************************************************//**
   * @test creating and configuring various kinds of render nodes.
   */
  class NodeBuilder_test : public Test
    {
      virtual void
      run (Arg)
        {
          UNIMPLEMENTED ("build and wire some render nodes");
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeBuilder_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
