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
#include "lib/test/microbenchmark.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/meta/function.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <chrono>
#include <vector>
#include <tuple>

using test::Test;
using util::isSameObject;
using lib::test::randTime;
using lib::test::showType;
using lib::time::Offset;

using std::vector;
using std::pair;
using std::reference_wrapper;


namespace vault{
namespace gear {
namespace test {
  
  namespace { // shorthand for test parametrisation
    
    using BlockFlow = gear::BlockFlow<>;
    using Allocator = BlockFlow::Allocator;
    using Strategy  = BlockFlow::Strategy;
    using Extent    = BlockFlow::Extent;
    using Epoch     = BlockFlow::Epoch;
    
    const size_t EXTENT_SIZ         = Extent::SIZ();
    Duration     INITIAL_EPOCH_STEP = Strategy{}.initialEpochStep();
    const size_t AVERAGE_EPOCHS     = Strategy{}.averageEpochs();
    const double BOOST_OVERFLOW     = Strategy{}.boostFactorOverflow();
    const double TARGET_FILL        = Strategy{}.config().TARGET_FILL;
    const double ACTIVITIES_P_FR    = Strategy{}.config().ACTIVITIES_PER_FRAME;
  }
  
  
  
  
  
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
          Allocator alloc;
          alloc.openNew();
          
          // the raw storage Extent is a compact block
          // providing uninitialised storage typed as `vault::gear::Activity`
          Extent& extent = *alloc.begin();
          CHECK (extent.size() == Extent::SIZ::value);
          CHECK (sizeof(extent) == extent.size() * sizeof(Activity));
          CHECK (showType<Extent::value_type>()  == "vault::gear::Activity"_expect);
          
          // we can just access some slot and place data there
          extent[55].data_.feed.one = 555555555555555;
          
          // now establish an Epoch placed into this storage block:
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
          CHECK (0 == gate.filledSlots());
          CHECK (0 == epoch.getFillFactor());
          
          // the storage there is not used yet....
          epoch[extent.size()-1].data_.timing.instant = Time{5,5};
          // ....but will be overwritten by the following ctor call
          
          // allocate a new Activity into the next free slot (using a faked AllocatorHandle)
          BlockFlow::AllocatorHandle allocHandle{alloc.begin(), nullptr};
          Activity& timeStart = allocHandle.create (Activity::WORKSTART);
          CHECK (isSameObject (timeStart, epoch[extent.size()-1]));
          
          // this Activity object is properly initialised (and memory was altered)
          CHECK (epoch[extent.size()-1].data_.timing.instant != Time(5,5));
          CHECK (epoch[extent.size()-1].data_.timing.instant == Time::NEVER);
          CHECK (timeStart.verb_ == Activity::WORKSTART);
          CHECK (timeStart.data_.timing.instant == Time::NEVER);
          CHECK (timeStart.data_.timing.quality == 0);
          
          // and the free-pointer was decremented to point to the next free slot
          CHECK (isSameObject (*gate.next, epoch[extent.size()-2]));
          
          // which also implies that there is still ample space left...
          CHECK (1 == gate.filledSlots());
          CHECK (gate.hasFreeSlot());
          
          CHECK (epoch.getFillFactor() == double(gate.filledSlots()) / (EXTENT_SIZ-1));
          
          // so let's eat this space up...
          for (uint i=extent.size()-2; i>1; --i)
            gate.claimNextSlot();
          
          // one final slot is left (beyond of the EpochGate itself)
          CHECK (isSameObject (*gate.next, epoch[1]));
          CHECK (gate.filledSlots() == EXTENT_SIZ-2);
          CHECK (gate.hasFreeSlot());

          gate.claimNextSlot();
          // aaand the boat is full...
          CHECK (not gate.hasFreeSlot());
          CHECK (isSameObject (*gate.next, epoch[0]));
          CHECK (gate.filledSlots() == EXTENT_SIZ-1);
          CHECK (epoch.getFillFactor() == 1);
          
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
          for (uint i=1; i<EXTENT_SIZ; ++i)
            allocHandle.create();
          
          CHECK (allocHandle.currDeadline() == Time(400,10));
          CHECK (not allocHandle.hasFreeSlot());
          
          // ...causing next allocation to be shifted into subsequent Epoch
          auto& a4 = allocHandle.create();
          CHECK (allocHandle.currDeadline() == Time(600,10));
          CHECK (allocHandle.hasFreeSlot());
          CHECK (watch(bFlow).find(a4)    == "10s600ms"_expect);

          // fill up and exhaust this Epoch too....
          for (uint i=1; i<EXTENT_SIZ; ++i)
            allocHandle.create();
          
          // so the handle has moved to the after next Epoch
          CHECK (allocHandle.currDeadline() == Time(800,10));
          CHECK (allocHandle.hasFreeSlot());
          
          // even allocation with way earlier deadline is shifted here now
          auto& a5 = bFlow.until(Time{220,10}).create();
          CHECK (watch(bFlow).find(a5)    == "10s800ms"_expect);
          
          // now repeat the same pattern, but now towards uncharted Epochs
          allocHandle = bFlow.until(Time{900,10});
          for (uint i=2; i<EXTENT_SIZ; ++i)
            allocHandle.create();
          
          CHECK (allocHandle.currDeadline() == Time(0,11));
          CHECK (not allocHandle.hasFreeSlot());
          auto& a6 = bFlow.until(Time{850,10}).create();
          // Note: encountered four overflow-Events, leading to decreased Epoch spacing for new Epochs
          CHECK (watch(bFlow).find(a6)    == "11s192ms"_expect);
          CHECK (watch(bFlow).allEpochs() == "10s200ms|10s400ms|10s600ms|10s800ms|11s|11s192ms"_expect);
          
          auto& a7 = bFlow.until(Time{500,11}).create();
          // this allocation does not count as overflow, but has to expand the Epoch grid, now using the reduced Epoch spacing
          CHECK (watch(bFlow).allEpochs() == "10s200ms|10s400ms|10s600ms|10s800ms|11s|11s192ms|11s384ms|11s576ms"_expect);
          CHECK (watch(bFlow).find(a7)    == "11s576ms"_expect);
          
          // on clean-up, actual fill ratio is used to adjust to optimise Epoch length for better space usage
          CHECK (bFlow.getEpochStep() == "≺192ms≻"_expect);
          bFlow.discardBefore (Time{999,10});
          CHECK (bFlow.getEpochStep() == "≺218ms≻"_expect);
          CHECK (watch(bFlow).allEpochs() == "11s|11s192ms|11s384ms|11s576ms"_expect);

          // placed into the oldest Epoch still alive
          auto& a8 = bFlow.until(Time{500,10}).create();
          CHECK (watch(bFlow).find(a8)    == "11s192ms"_expect);
        }
      
      
      
      /** @test load based regulation of Epoch spacing
       *        - on overflow, capacity is boosted by a fixed factor
       *        - on clean-up, a moving average of (in hindsight) optimal length
       *          is computed and used as the new Epoch spacing
       */
      void
      adjustEpochs()
        {
          BlockFlow bFlow;
          CHECK (bFlow.getEpochStep() == INITIAL_EPOCH_STEP);
          
          // whenever an Epoch overflow happens, capacity is boosted by reducing the Epoch duration
          bFlow.markEpochOverflow();
          CHECK (bFlow.getEpochStep() == INITIAL_EPOCH_STEP * BOOST_OVERFLOW);
          bFlow.markEpochOverflow();
          CHECK (bFlow.getEpochStep() == INITIAL_EPOCH_STEP * BOOST_OVERFLOW*BOOST_OVERFLOW);
          
          // To counteract this increase, on clean-up the actual fill rate of the Extent
          // serves to guess an optimal Epoch duration, which is averaged exponentially
          
          // Using just arbitrary demo values for some fictional Epochs
          TimeVar dur1 = INITIAL_EPOCH_STEP;
          double  fac1 = 0.8;
          TimeVar dur2 = INITIAL_EPOCH_STEP * BOOST_OVERFLOW;
          double  fac2 = 0.3;
          
          double  goal1 = double(_raw(dur1)) / (fac1/TARGET_FILL);
          double  goal2 = double(_raw(dur2)) / (fac2/TARGET_FILL);

          auto movingAverage = [&](TimeValue old, double contribution)
                                  {
                                    auto N = AVERAGE_EPOCHS;
                                    auto averageTicks = double(_raw(old))*(N-1)/N + contribution/N;
                                    return TimeValue{gavl_time_t (floor (averageTicks))};
                                  };
          
          TimeVar step = bFlow.getEpochStep();
          bFlow.markEpochUnderflow (dur1, fac1);
          CHECK (bFlow.getEpochStep() == movingAverage(step, goal1));
          
          step = bFlow.getEpochStep();
          bFlow.markEpochUnderflow (dur2, fac2);
          CHECK (bFlow.getEpochStep() == movingAverage(step, goal2));
        }
      
      
      
      
      /** @test investigate progression of epochs under realistic load
       *        - expose the allocator to a load of 200fps for simulated 3 Minutes
       *        - assuming 10 Activities per frame, this means a throughput of 360000 Activities
       *        - run this load exposure under saturation for performance measurement
       *        - use a planning to deadline delay of 500ms, but with ±200ms random spread
       *        - after 250ms (500 steps), »invoke« by accessing and adding the random checksum
       *        - run a comparison of all-pre-allocated ⟷ heap allocated ⟷ Refcount ⟷ BlockFlow
       * @remarks
       *  This test setup can be used to investigate different load scenarios.
       *  In the standard as defined, the BlockFlow allocator is overloaded initially;
       *  within 5 seconds, the algorithm should have regulated the Epoch stepping down
       *  to accommodate the load peak. As immediate response, excess allocation requests
       *  are shifted into later Epochs. To cope with a persisting higher load, the spacing
       *  is reduced swiftly, by growing the internal pool with additional heap allocated Extents.
       *  In the following balancing phase, the mechanism aims at bringing back the Epoch duration
       *  into a narrow corridor, to keep the usage quotient as close as possible to 90%
       */
      void
      storageFlow()
        {
          const size_t      FPS = 200;
          const size_t TICK_P_S = FPS * ACTIVITIES_P_FR;   // Simulated throughput 200 frames per second
          const gavl_time_t STP = Time::SCALE / TICK_P_S;  // Simulation stepping (here 2 steps per ms)
          const gavl_time_t RUN = _raw(Time{0,0,3});       // nominal length of the simulation time axis
          Offset BASE_DEADLINE{FSecs{1,2}};                // base pre-roll before deadline
          Offset SPREAD_DEAD{FSecs{2,100}};                // random spread of deadline around base
          const uint INVOKE_LAG = _raw(Time{250,0}) /STP;  // „invoke“ the Activity after simulated 250ms (≙ 500 steps)
          const uint CLEAN_UP   = _raw(Time{100,0}) /STP;  // perform clean-up every 200 steps
          const uint INSTANCES  = RUN /STP;                // 120000 Activity records to send through the test subject
          const uint MAX_TIME   = INSTANCES
                                  +INVOKE_LAG+2*CLEAN_UP;  // overall count of Test steps to perform
          
          using TestData = vector<pair<TimeVar, size_t>>;
          using Subjects = vector<reference_wrapper<Activity>>;
          
          // pre-generate random test data
          TestData testData{INSTANCES};
          for (size_t i=0; i<INSTANCES; ++i)
            {
              const size_t SPREAD   =  2*_raw(SPREAD_DEAD);
              const size_t MIN_DEAD = _raw(BASE_DEADLINE) - _raw(SPREAD_DEAD);
              
              auto&[t,r] = testData[i];
              r = rand() % SPREAD;
              t = TimeValue(i*STP + MIN_DEAD + r);
            }
          
          Activity dummy;  // reserve memory for test subject index
          Subjects subject{INSTANCES, std::ref(dummy)};
          
          auto runTest = [&](auto allocate, auto invoke) -> size_t
                            {
                              // allocate Activity record for deadline and with given random payload
                              ASSERT_VALID_SIGNATURE (decltype(allocate), Activity&(Time, size_t));
                              
                              // access the given Activity, read the payload, then trigger disposal
                              ASSERT_VALID_SIGNATURE (decltype(invoke),   size_t(Activity&));
                              
                              size_t checksum{0};
                              for (size_t i=0; i<MAX_TIME; ++i)
                                {
                                  if (i < INSTANCES)
                                    {
                                      auto const& data = testData[i];
                                      subject[i] = allocate(data.first, data.second);
                                    }
                                  if (INVOKE_LAG <= i and i-INVOKE_LAG < INSTANCES)
                                    checksum += invoke(subject[i-INVOKE_LAG]);
                                }
                              return checksum;
                            };
          
          auto benchmark = [INSTANCES](auto invokeTest)
                            {         //  does the timing measurement with result in nanoseconds
                              return lib::test::benchmarkTime(invokeTest, INSTANCES);
                            };
          
          
          
          /* =========== Test-Setup-1: no individual allocations/deallocations ========== */
          size_t sum1{0};
          vector<Activity> storage{INSTANCES};
          auto noAlloc = [&]{ // use pre-allocated storage block
                              auto allocate = [i=0, &storage](Time, size_t check) mutable -> Activity&
                                                  {
                                                    return *new(&storage[i++]) Activity{check, size_t{55}};
                                                  };
                              auto invoke   = [](Activity& feedActivity)
                                                  {
                                                    return feedActivity.data_.feed.one;
                                                  };
                              
                              sum1 = runTest (allocate, invoke);
                            };
          
          
          /* =========== Test-Setup-2: individual heap allocations ========== */
          size_t sum2{0};
          auto heapAlloc = [&]{
                              auto allocate = [](Time, size_t check) mutable -> Activity&
                                                  {
                                                    return *new Activity{check, size_t{55}};
                                                  };
                              auto invoke   = [](Activity& feedActivity)
                                                  {
                                                    size_t check = feedActivity.data_.feed.one;
                                                    delete &feedActivity;
                                                    return check;
                                                  };
                              
                              sum2 = runTest (allocate, invoke);
                            };
          
          
          /* =========== Test-Setup-3: manage individually by ref-cnt  ========== */
          size_t sum3{0};
          vector<std::shared_ptr<Activity>> manager{INSTANCES};
          auto sharedAlloc = [&]{
                              auto allocate = [&, i=0](Time, size_t check) mutable -> Activity&
                                                  {
                                                    Activity* a = new Activity{check, size_t{55}};
                                                    manager[i].reset(a);
                                                    ++i;
                                                    return *a;
                                                  };
                              auto invoke   = [&, i=0](Activity& feedActivity) mutable
                                                  {
                                                    size_t check = feedActivity.data_.feed.one;
                                                    manager[i].reset();
                                                    return check;
                                                  };
                              
                              sum3 = runTest (allocate, invoke);
                            };
          
          
          /* =========== Test-Setup-4: use BlockFlow allocation scheme ========== */
          
          size_t sum4{0};
          gear::BlockFlow<blockFlow::RenderConfig> blockFlow;
          // Note: using the RenderConfig, which uses larger blocks and more pre-allocation
          auto blockFlowAlloc = [&]{
                              auto allocHandle = blockFlow.until(Time{400,0});
                              auto allocate = [&, j=0](Time t, size_t check) mutable -> Activity&
                                                  {
                                                    if (++j >= 10) // typically several Activities are allocated on the same deadline
                                                      {
                                                        allocHandle = blockFlow.until(t);
                                                        j = 0;
                                                      }
                                                    return allocHandle.create (check, size_t{55});
                                                  };
                              auto invoke   = [&, i=0](Activity& feedActivity) mutable
                                                  {
                                                    size_t check = feedActivity.data_.feed.one;
                                                    if (i % CLEAN_UP == 0)
                                                      blockFlow.discardBefore (Time{TimeValue{i*STP}});
                                                    ++i;
                                                    return check;
                                                  };
                              
                              sum4 = runTest (allocate, invoke);
                            };
          
          // INVOKE Setup-1
          auto time_noAlloc = benchmark(noAlloc);
          
          // INVOKE Setup-2
          auto time_heapAlloc = benchmark(heapAlloc);
          
          // INVOKE Setup-3
          auto time_sharedAlloc = benchmark(sharedAlloc);

          cout<<"\n\n■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■"<<endl;
          
          // INVOKE Setup-4
          auto time_blockFlow = benchmark(blockFlowAlloc);
          
          Duration expectStep{FSecs{blockFlow.framesPerEpoch(), FPS} * 9/10};
          
          cout<<"\n___Microbenchmark____"
              <<"\nnoAlloc     : "<<time_noAlloc
              <<"\nheapAlloc   : "<<time_heapAlloc
              <<"\nsharedAlloc : "<<time_sharedAlloc
              <<"\nblockFlow   : "<<time_blockFlow
              <<"\n_____________________\n"
              <<"\ninstances.... "<<INSTANCES
              <<"\nfps.......... "<<FPS
              <<"\nActivities/s. "<<TICK_P_S
              <<"\nEpoch(expect) "<<expectStep
              <<"\nEpoch  (real) "<<blockFlow.getEpochStep()
              <<"\ncnt Epochs... "<<watch(blockFlow).cntEpochs()
              <<"\nalloc pool... "<<watch(blockFlow).poolSize()
              <<endl;
          
          // all Activities have been read in all test cases,
          // yielding identical checksum
          CHECK (sum1 == sum2);
          CHECK (sum1 == sum3);
          CHECK (sum1 == sum4);
          
          // Epoch spacing regulation must be converge up to ±10ms
          CHECK (expectStep - blockFlow.getEpochStep() < Time(10,0));
          
          // after the initial overload is levelled,
          // only a small number of Epochs should be active
          CHECK (watch(blockFlow).cntEpochs() < 8);
          
          // Due to Debug / Release builds, we can not check the runtime only a very rough margin.
          // With -O3, this amortised allocation time should be way below time_sharedAlloc
          CHECK (time_blockFlow < 800);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BlockFlow_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
