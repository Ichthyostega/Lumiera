/*
  TestChainLoad(Test)  -  verify diagnostic setup to watch scheduler activities

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file test-chain-load-test.cpp
 ** unit test \ref TestChainLoad_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "test-chain-load.hpp"
//#include "vault/real-clock.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/format-cout.hpp"
#include "lib/test/diagnostic-output.hpp"//////////////////////////TODO TOD-oh
//#include "lib/util.hpp"


//using lib::time::Time;
//using lib::time::FSecs;

//using util::isSameObject;
//using lib::test::randStr;
//using lib::test::randTime;


namespace vault{
namespace gear {
namespace test {
  
  
  /*****************************************************************//**
   * @test verify a tool to generate synthetic load for Scheduler tests.
   * @see SchedulerService_test
   * @see SchedulerStress_test
   */
  class TestChainLoad_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verify_Node();

          witch_gate();
        }
      
      
      /** @test TODO demonstrate simple usage of the test-load
       * @todo WIP 11/23 🔁 define ⟶ 🔁 implement
       */
      void
      simpleUsage()
        {
          TestChainLoad testLoad;
        }
      
      
      
      /** @test TODO diagnostic blah
       * @todo WIP 11/23 🔁 define ⟶ implement
       */
      void
      verify_Node()
        {
          using Node = TestChainLoad<>::Node;
          
          Node n0;
          CHECK (n0.hash == 0);
          
          Node n1{23}, n2{55};
          CHECK (n1.hash == 23);
          CHECK (n2.hash == 55);
        }
      
      
      
      /** @test TODO diagnostic blah
       * @todo WIP 11/23 🔁 define ⟶ implement
       */
      void
      witch_gate()
        {
          UNIMPLEMENTED ("witch gate");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TestChainLoad_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
