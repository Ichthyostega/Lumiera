/*
  SegmentationDatastructure(Test)  -  verify basic properties of the Segmentation

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/

/** @file segmentation-datastructure-test.cpp
 ** unit test \ref SegmentationDatastructure_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "proc/mobject/builder/fixture-change-detector.hpp"
#include "proc/asset/timeline.hpp"
#include "proc/asset/pipe.hpp"
#include "common/query.hpp"
#include "lib/util.hpp"


namespace proc {
namespace mobject {
namespace builder {
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
   * @test TODO blubb
   *       
   * @see  mobject::builder::FixtureChangeDetector
   */
  class SegmentationDatastructure_test : public Test
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
  LAUNCHER (SegmentationDatastructure_test, "unit builder");
  
  
  
}}}} // namespace proc::mobject::builder::test
