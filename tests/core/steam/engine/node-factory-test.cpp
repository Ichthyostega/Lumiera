/*
  NodeFactory(Test)  -  building render nodes

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file node-factory-test.cpp
 ** unit test \ref NodeFactory_test
 ** @todo 12/2024 this test will focus on the high-level integration,
 **       which is future work and possibly addressed in the next »Vertical Slice«
 **       when we add processing of a given media clip from disk.
 */


#include "lib/test/run.hpp"
//#include "lib/util.hpp"


using std::string;


namespace steam {
namespace engine{
namespace test  {
  
  
  
  
  /***************************************************************//**
   * @test creating and wiring various kinds of render nodes.
   */
  class NodeFactory_test : public Test
    {
      virtual void
      run (Arg)
        {
          UNIMPLEMENTED ("build and wire some render nodes");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeFactory_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
