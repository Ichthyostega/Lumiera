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
          CHECK (watch(bFlow).first() - deadline == bFlow.getEpochStep());
          
          bFlow.discardBefore (deadline + Time{0,5});
          CHECK (0 == watch(bFlow).cntEpochs());
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
          
          // allocate a new Activity into the next free slot (using a faked AllocatorHandle)
          BlockFlow::AllocatorHandle allocHandle{alloc.begin(), nullptr};
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
            gate.claimNextSlot();
          
          // one final slot is left (beyond of the EpochGate itself)
          CHECK (isSameObject (*gate.next, epoch[1]));
          CHECK (gate.hasFreeSlot());

          gate.claimNextSlot();
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
      
      
      
      /** @test place Activity record into storage
       *        - new Activity without any previously established Epoch
       *        - place Activity into future, expanding the Epoch grid
       *        - locate Activity relative to established Epoch grid
       *        - fill up existing Epoch, causing overflow to next one
       *        - exhaust multiple adjacent Epochs, overflowing to first free one
       *        - exhaust last Epoch, causing setup of new Epoch, with reduced spacing
       *        - use this reduced spacing also for subsequently created Epochs
       *        - clean up obsoleted Epochs, based on given deadline
       * @todo WIP 7/23 ⟶ ✔define ⟶ ✔implement
       */
      void
      placeActivity()
        {
          BlockFlow bFlow;
          
          Time t1 = Time{  0,10};
          Time t2 = Time{500,10};
          Time t3 = Time{  0,11};
          
          // no Epoch established yet...
          auto& a1 = bFlow.until(t1).create();
          CHECK (watch(bFlow).allEpochs() == "10s200ms"_expect);
          CHECK (watch(bFlow).find(a1)    == "10s200ms"_expect);
          
          // setup Epoch grid into the future
          auto& a3 = bFlow.until(t3).create();
          CHECK (watch(bFlow).allEpochs() == "10s200ms|10s400ms|10s600ms|10s800ms|11s"_expect);
          CHECK (watch(bFlow).find(a3)    == "11s"_expect);
          
          // associate to existing Epoch
          auto& a2 = bFlow.until(t2).create();
          CHECK (watch(bFlow).allEpochs() == "10s200ms|10s400ms|10s600ms|10s800ms|11s"_expect);
          CHECK (watch(bFlow).find(a2)    == "10s600ms"_expect);
          
          Time t0 = Time{0,5};
          // late(past) Activity is placed in the oldest Epoch alive
          auto& a0 = bFlow.until(t0).create();
          CHECK (watch(bFlow).allEpochs() == "10s200ms|10s400ms|10s600ms|10s800ms|11s"_expect);
          CHECK (watch(bFlow).find(a0)    == "10s200ms"_expect);
          
          // provoke Epoch overflow by exhausting all available storage slots
          BlockFlow::AllocatorHandle allocHandle = bFlow.until(Time{300,10});
          for (uint i=1; i<Epoch::SIZ(); ++i)
            allocHandle.create();
          
          CHECK (allocHandle.currDeadline() == Time(400,10));
          CHECK (not allocHandle.hasFreeSlot());
          
          // ...causing next allocation to be shifted into subsequent Epoch
          auto& a4 = allocHandle.create();
          CHECK (allocHandle.currDeadline() == Time(600,10));
          CHECK (allocHandle.hasFreeSlot());
          CHECK (watch(bFlow).find(a4)    == "10s600ms"_expect);

          // fill up and exhaust this Epoch too....
          for (uint i=1; i<Epoch::SIZ(); ++i)
            allocHandle.create();
          
          // so the handle has moved to the after next Epoch
          CHECK (allocHandle.currDeadline() == Time(800,10));
          CHECK (allocHandle.hasFreeSlot());
          
          // even allocation with way earlier deadline is shifted here now
          auto& a5 = bFlow.until(Time{220,10}).create();
          CHECK (watch(bFlow).find(a5)    == "10s800ms"_expect);
          
          // now repeat the same pattern, but now towards uncharted Epochs
          allocHandle = bFlow.until(Time{900,10});
          for (uint i=2; i<Epoch::SIZ(); ++i)
            allocHandle.create();
          
          CHECK (allocHandle.currDeadline() == Time(0,11));
          CHECK (not allocHandle.hasFreeSlot());
          auto& a6 = bFlow.until(Time{850,10}).create();
          // Note: encountered four overflow-Events, leading to decreased Epoch spacing for new Epochs
          CHECK (watch(bFlow).find(a6)    == "11s131ms"_expect);
          CHECK (watch(bFlow).allEpochs() == "10s200ms|10s400ms|10s600ms|10s800ms|11s|11s131ms"_expect);
          
          auto& a7 = bFlow.until(Time{500,11}).create();
          // this allocation does not count as overflow, but has to expand the Epoch grid, now using the reduced Epoch spacing
          CHECK (watch(bFlow).allEpochs() == "10s200ms|10s400ms|10s600ms|10s800ms|11s|11s131ms|11s262ms|11s393ms|11s524ms"_expect);
          CHECK (watch(bFlow).find(a7)    == "11s524ms"_expect);
          
          bFlow.discardBefore (Time{999,10});
          CHECK (watch(bFlow).allEpochs() == "11s|11s131ms|11s262ms|11s393ms|11s524ms"_expect);

          // placed into the oldest Epoch still alive
          auto& a8 = bFlow.until(Time{500,10}).create();
          CHECK (watch(bFlow).find(a8)    == "11s131ms"_expect);
        }
      
      
      
      /** @test TODO load based regulation of Epoch spacing
       * @todo WIP 7/23 ⟶ ✔define ⟶ 🔁implement
       */
      void
      adjustEpochs()
        {
          BlockFlow bFlow;
          CHECK (bFlow.getEpochStep() == INITIAL_EPOCH_STEP);
          
          bFlow.markEpochOverflow();
          CHECK (bFlow.getEpochStep() == INITIAL_EPOCH_STEP * OVERFLOW_BOOST_FACTOR);
          bFlow.markEpochOverflow();
          CHECK (bFlow.getEpochStep() == INITIAL_EPOCH_STEP * OVERFLOW_BOOST_FACTOR*OVERFLOW_BOOST_FACTOR);
          
          Duration dur1 = INITIAL_EPOCH_STEP;
          Duration dur2 = INITIAL_EPOCH_STEP * OVERFLOW_BOOST_FACTOR;
          
          TimeVar step = bFlow.getEpochStep();
          Rat     fill = 8_r/10;
          Rat N = AVERAGE_EPOCHS;
          
          bFlow.markEpochUnderflow (dur1, fill);
          CHECK (bFlow.getEpochStep() == step*((N-1)/N) + dur1*(1/N /fill));
          
          step = bFlow.getEpochStep();
          fill = 3_r/10;
          bFlow.markEpochUnderflow (dur2, fill);
          CHECK (bFlow.getEpochStep() == step*((N-1)/N) + dur2*(1/N /fill));
        }
      
      
      
      /** @test TODO maintain progression of epochs.
       * @todo WIP 7/23 ⟶ 🔁define ⟶ implement
       */
      void
      storageFlow()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BlockFlow_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
