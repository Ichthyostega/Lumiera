/*
  BlockFlow(Test)  -  verify scheduler memory management scheme

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

/** @file block-flow-test.cpp
 ** unit test \ref BlockFlow_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "vault/gear/block-flow.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/format-cout.hpp"
#include "lib/test/diagnostic-output.hpp" ////////////////////////////////TODO
//#include "lib/util.hpp"

//#include <utility>

using test::Test;
//using std::move;
//using util::isSameObject;
using lib::test::randTime;


namespace vault{
namespace gear {
namespace test {
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
//  using lib::time::Time;
  
  
  
  
  
  /*****************************************************************//**
   * @test document the memory management scheme used by the Scheduler.
   * @see SchedulerActivity_test
   * @see SchedulerUsage_test
   */
  class BlockFlow_test : public Test
    {
      
      virtual void
      run (Arg)
        {
           simpleUsage();
           verifyAPI();
           handleEpoch();
           placeActivity();
           adjustEpochs();
           storageFlow();
        }
      
      
      /** @test TODO demonstrate a simple usage scenario
       * @todo WIP 7/23 ⟶ ✔define ⟶ 🔁implement
       */
      void
      simpleUsage()
        {
          BlockFlow bFlow;
          Time deadline = randTime();
          
          Activity& tick = bFlow.until(deadline).create();
          CHECK (tick.verb_ == Activity::TICK);
          CHECK (1 == watch(bFlow).cntEpochs());
          CHECK (watch(bFlow).first() > deadline);
          CHECK (watch(bFlow).first() - deadline == bFlow.currEpochStep());
SHOW_EXPR(watch(bFlow).cntEpochs());
SHOW_EXPR(watch(bFlow).poolSize());
SHOW_EXPR(watch(bFlow).first());
          
          bFlow.discardBefore (deadline + Time{0,5});
SHOW_EXPR(watch(bFlow).cntEpochs());
          CHECK (0 == watch(bFlow).cntEpochs());
        }
      
      
      
      /** @test verify the primary BlockFlow API functions in isolation
       * @todo WIP 7/23 ⟶ define ⟶ implement
       */
      void
      verifyAPI()
        {
        }
      
      
      
      /** @test TODO cover the handling of Epochs
       * @todo WIP 7/23 ⟶ define ⟶ implement
       */
      void
      handleEpoch()
        {
        }
      
      
      
      /** @test TODO place Activity record into storage
       * @todo WIP 7/23 ⟶ define ⟶ implement
       */
      void
      placeActivity()
        {
        }
      
      
      
      /** @test TODO load based regulation of Epoch spacing
       * @todo WIP 7/23 ⟶ define ⟶ implement
       */
      void
      adjustEpochs()
        {
        }
      
      
      
      /** @test TODO maintain progression of epochs.
       * @todo WIP 7/23 ⟶ define ⟶ implement
       */
      void
      storageFlow()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BlockFlow_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
