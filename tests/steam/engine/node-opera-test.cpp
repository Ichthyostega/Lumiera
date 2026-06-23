/*
  NodeOpera(Test)  -  verify proper render node operation modes

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file node-opera-test.cpp
 ** Let the nodes sing with \ref NodeOpera_test.
 */


#include "test/run.hpp"
//#include "lib/util.hpp"


//using std::string;


namespace steam {
namespace engine{
namespace test  {
  
  
  
  
  /***************************************************************//**
   * @test check render node operation modes and collaboration.
   * @todo 6/2026 this test shell demonstrate all relevant operation modes
   *       of render nodes, at the level of an component-integration-test.
   *       It is not clear yet what further operation modes this entails,
   *       beyond (obviously) invoking a render job...       ////////////////////////////////////////////////TICKET #1367 : build a node invocation
   * @remark the purpose of this test is to accomplish component integration
   *       regarding relevant usages of the render node network, seen from the
   *       perspective of an (exit) render node that is used / invoked / pulled.
   */
  class NodeOpera_test : public Test
    {
      virtual void
      run (Arg)
        {
          UNIMPLEMENTED ("operate some render nodes as linked together");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeOpera_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
