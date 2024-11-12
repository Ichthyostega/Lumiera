/*
  NodeDevel(Test)  -  Render Node development and test support

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file node-devel-test.cpp
 ** unit test \ref NodeDevel_test
 */


#include "lib/test/run.hpp"
#include "steam/engine/test-rand-ontology.hpp" ///////////TODO
#include "lib/test/diagnostic-output.hpp"/////////////////TODO
#include "lib/random.hpp"
//#include "lib/util.hpp"


//using std::string;


namespace steam {
namespace engine{
namespace test  {
  
  
  namespace {
    /** uninitialised local storage that can be passed
     *  as working buffer and accessed as TestFrame */
    struct Buffer
      : util::NonCopyable
      {
        alignas(TestFrame)
          std::byte storage[sizeof(TestFrame)];
        
        operator TestFrame*  () { return   std::launder (reinterpret_cast<TestFrame* > (&storage)); } 
        TestFrame* operator->() { return   std::launder (reinterpret_cast<TestFrame* > (&storage)); } 
        TestFrame& operator* () { return * std::launder (reinterpret_cast<TestFrame* > (&storage)); } 
      };
  }
  
  
  
  /***************************************************************//**
   * @test verify support for developing Render Node functionality.
   */
  class NodeDevel_test : public Test
    {
      virtual void
      run (Arg)
        {
          seedRand();
          
          processing_generateFrame();
          processing_generateMultichan();
        }
      
      
      /** @test function to generate random test data frames
       */
      void
      processing_generateFrame()
        {
          size_t frameNr = defaultGen.u64();
          uint flavour   = defaultGen.u64();
          
          Buffer buff;
          CHECK (not buff->isSane());
          
          generateFrame (buff, frameNr, flavour);
          CHECK ( buff->isSane());
          CHECK (*buff == TestFrame(frameNr,flavour));
        }
      
      /** @test function to generate an array of random test data frames
       *        for consecutive channels
       */
      void
      processing_generateMultichan()
        {
          size_t frameNr = defaultGen.u64();
          uint flavour   = defaultGen.u64();
          
          uint channels  = 1 + rani(50);
          CHECK (1 <= channels and channels <= 50);
          
          Buffer buffs[50];
          for (uint i=0; i<channels; ++i)
            CHECK (not buffs[i]->isSane());
          
          generateMultichan (channels, buffs[0], frameNr, flavour);
          for (uint i=0; i<channels; ++i)
            {
              CHECK (buffs[i]->isSane());
              CHECK (*(buffs[i]) == TestFrame(frameNr,flavour+i));
            }
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeDevel_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
