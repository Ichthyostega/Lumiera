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
#include "lib/util.hpp"

//#include <utility>

using test::Test;
//using std::move;
using util::isSameObject;
using lib::test::randTime;
using lib::test::showType;


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
      
      
      /** @test demonstrate a simple usage scenario
       *        - open new Epoch to allocate an Activity
       *        - clean-up at a future time point
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
          
          bFlow.discardBefore (deadline + Time{0,5});
          CHECK (0 == watch(bFlow).cntEpochs());
        }
      
      
      
      /** @test verify the primary BlockFlow API functions in isolation
       * @todo WIP 7/23 ⟶ define ⟶ implement
       */
      void
      verifyAPI()
        {
//SHOW_EXPR(watch(bFlow).cntEpochs());
//SHOW_EXPR(watch(bFlow).poolSize());
//SHOW_EXPR(watch(bFlow).first());
        }
      
      
      
      /** @test cover properties and handling of Epochs (low-level)
       *        - demonstrate that Epoch is placed into an Extent
       *        - verify that both Extent and Epoch access the same memory block
       *        - demonstrate the standard setup and initialisation of an Epoch
       *        - allocate some Activities into the storage and observe free-managment
       *        - detect when the Epoch is filled up
       *        - verify alive / dead decision relative to given deadline
       * @note this test covers helpers and implementation structures of BlockFlow,
       *       without actually using a BlockFlow instance; rather, the typical handling
       *       and low-level bookkeeping aspects are emulated and observed
       */
      void
      handleEpoch()
        {
          using Extent = Allocator::Extent;
          // the raw storage Extent is a compact block
          // providing uninitialised storage typed as `vault::gear::Activity`
          
          Allocator alloc;
          alloc.openNew();
          Extent& extent = *alloc.begin();
          CHECK (extent.size() == Extent::SIZ::value);
          CHECK (sizeof(extent) == extent.size() * sizeof(Activity));
          CHECK (showType<Extent::value_type>()  == "vault::gear::Activity"_expect);
          
          // we can just access some slot and place data there
          extent[55].data_.feed.one = 555555555555555;
          
          // now establish an Epoch in this storage block:
          Epoch& epoch = Epoch::setup (alloc.begin(), Time{0,10});
          
          // the underlying storage is not touched yet...
          CHECK (epoch[55].data_.feed.one == 555555555555555);
          
          // but in the first slot, an »EpochGate« has been implanted
          Epoch::EpochGate& gate = epoch.gate();
          CHECK (isSameObject (gate, epoch[0]));
          CHECK (isSameObject (epoch[0], extent[0]));
          CHECK (Time{gate.deadline()} == Time(0,10));
          CHECK (Time{gate.deadline()} == Time{epoch[0].data_.condition.dead});
          CHECK (Activity::GATE  == epoch[0].verb_);
          
          // the gate's `next`-pointer is (ab)used to manage the next allocation slot
          CHECK (isSameObject (*gate.next, epoch[extent.size()-1]));
          
          // the storage there is not yet used, but will be overwritten by the ctor call
          epoch[extent.size()-1].data_.timing.instant = Time{5,5};
          
          // allocate a new Activity into the next free slot
          BlockFlow::AllocatorHandle allocHandle{alloc.begin()};
          Activity& timeStart = allocHandle.create (Activity::TIMESTART);
          CHECK (isSameObject (timeStart, epoch[extent.size()-1]));
          
          // this Activity object is properly initialised (and memory was altered)
          CHECK (epoch[extent.size()-1].data_.timing.instant != Time(5,5));
          CHECK (epoch[extent.size()-1].data_.timing.instant == Time::NEVER);
          CHECK (timeStart.verb_ == Activity::TIMESTART);
          CHECK (timeStart.data_.timing.instant == Time::NEVER);
          CHECK (timeStart.data_.timing.quality == 0);
          
          // and the free-pointer was decremented to point to the next free slot
          CHECK (isSameObject (*gate.next, epoch[extent.size()-2]));
          
          // which also implies that there is still ample space left...
          CHECK (gate.hasFreeSlot());
          
          // so let's eat this space up...
          for (uint i=extent.size()-2; i>1; --i)
            allocHandle.create();
          
          // one final slot is left (beyond of the EpochGate itself)
          CHECK (isSameObject (*gate.next, epoch[1]));
          CHECK (gate.hasFreeSlot());
          
          allocHandle.create (size_t(111), size_t(222));
          CHECK (epoch[1].verb_ == Activity::FEED);
          CHECK (epoch[1].data_.feed.one = 111);
          CHECK (epoch[1].data_.feed.two = 222);
          
          // aaand the boat is full...
          CHECK (not gate.hasFreeSlot());
          CHECK (isSameObject (*gate.next, epoch[0]));
          
          // a given Epoch can be checked for relevance against a deadline
          CHECK (gate.deadline() == Time(0,10));

          CHECK (    gate.isAlive (Time(0,5)));
          CHECK (    gate.isAlive (Time(999,9)));
          CHECK (not gate.isAlive (Time(0,10)));
          CHECK (not gate.isAlive (Time(1,10)));
                     ////////////////////////////////////////////////////////////////////////////////////////TICKET #1298 : actually use a GATE implementation and then also check the count-down latch
        }
      
      
      
      /** @test TODO place Activity record into storage
       * @todo WIP 7/23 ⟶ 🔁define ⟶ implement
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
