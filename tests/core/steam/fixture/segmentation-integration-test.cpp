/*
  SegmentationIntegration(Test)  -  verify basic properties of the Segmentation

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file segmentation-integration-test.cpp
 ** integration test \ref SegmentationIntegration_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/fixture/fixture-change-detector.hpp"
#include "steam/asset/timeline.hpp"
#include "steam/asset/pipe.hpp"
#include "common/query.hpp"
#include "lib/util.hpp"


namespace steam {
namespace fixture {
namespace test  {
  
//  using util::isSameObject;
//  using util::isnil;
//  
  using asset::Pipe;
  using asset::PPipe;
  using asset::Struct;
  using asset::Timeline;
  using asset::PTimeline;
  using lumiera::Query;
//  
  typedef asset::ID<Pipe> PID;
  typedef asset::ID<Struct> TID;
//    
//  typedef ModelPortRegistry::ModelPortDescriptor const& MPDescriptor;
  
  
  namespace { // test environment
    
    inline PID
    getPipe (string id)
    {
      return Pipe::query ("id("+id+")");
    }
    
    inline TID
    getTimeline (string id)
    {
      return asset::Struct::retrieve (Query<Timeline> ("id("+id+")"))->getID();
    }
    
    struct TestContext
      {
        
        /** setup */
        TestContext()
          { }
        
        /** tear-down */
       ~TestContext()
          {
          }
      };
  }
  
  
  
  
  /*****************************************************************************//**
   * @test Properties and behaviour of a complete Segmentation data structure.
   *       - access segments keyed by nominal time
   *       - determine the index-Nr of a ModelPort
   *       - transactional remoulding of the Segmentation
   *       - retrieve effective changes after remoulding
   *       - integration with AllocationCluster memory managment
   * @todo 2010 ⁐ 2023 a sketch for a test setup, which was meanwhile elaborated
   *       into the DummyPlayConnection; indeed such a component integration test
   *       is still required to document and cover the fixture data structure.... 
   * @see  mobject::builder::FixtureChangeDetector
   */
  class SegmentationIntegration_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          TestContext ctx;
        }
      
      
      void
      fabricating_ModelPorts ()
        {
        }
      
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (SegmentationIntegration_test, "unit fixture");
  
  
  
}}} // namespace steam::fixture::test
