/*
  TestTracking(Test)  -  demonstrate test helpers for tracking automated clean-up

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

/** @file test-tracking-test.cpp
 ** unit test \ref TestTracking_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/tracking-dummy.hpp"
#include "lib/test/tracking-allocator.hpp"
#include "lib/allocator-handle.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"
#include "lib/test/diagnostic-output.hpp"///////////////////////TODO

#include <string>

using std::string;
using util::toString;
using util::join;


namespace lib {
namespace test{
namespace test{
  
  
  /***************************************************//**
   * @test verify proper working of test helpers to track
   *       automated clean-up and memory deallocation.
   * @see TestHelper_test
   * @see tracking-dummy.hpp
   * @see tracking-allocator.hpp
   */
  class TestTracking_test : public Test
    {
      void
      run (Arg)
        {
          demonstrate_logObject();
          demonstrate_checkObject();
          demonstrate_checkAllocator();
        }
      
      
      /** @test capture object lifecycle events in the EventLog.
       *  @see EventLog_test
       *  @see LateBindInstance_test
       */
      void
      demonstrate_logObject ()
        {
          auto& log = Tracker::log;
          log.clear (this);
          
          Tracker alpha;                                                       // (1) create α
          auto randomAlpha = toString(alpha.val);
          
          log.event("ID",alpha.val);                                           // (2) α has an random ID
          {
            Tracker beta{55};                                                  // (3) create β
            alpha = beta;                                                      // (4) assign α ≔ β
          }
          log.event ("ID",alpha.val);                                          // (5) thus α now also bears the ID 55 of β
          Tracker gamma = move(alpha);                                         // (6) create γ by move-defuncting α
          {
            Tracker delta(23);                                                 // (7) create δ with ID 23
            delta = move(gamma);                                               // (8) move-assign δ ⟵ γ
            log.event ("ID",delta.val);                                        // (9) thus δ now bears the ID 55 (moved α ⟶ γ ⟶ δ)
          }
          log.event("ID",alpha.val);                                           // (X) and thus α is now a zombie object
          
          cout << "____Tracker-Log_______________\n"
               << util::join(Tracker::log,      "\n")
               << "\n───╼━━━━━━━━━━━╾──────────────"<<endl;
          
          CHECK (log.verify("EventLogHeader").on("TestTracking_test")
                    .beforeCall("ctor").on(&alpha)                             // (1) create α
                    .beforeEvent("ID", randomAlpha)                            // (2) α has an random ID
                    .beforeCall("ctor").arg(55)                                // (3) create β
                    .beforeCall("assign-copy").on(&alpha).arg("Track{55}")     // (4) assign α ≔ β
                    .beforeCall("dtor").arg(55)
                    .beforeEvent("ID", "55")                                   // (5) thus α now also bears the ID 55 of β
                    .beforeCall("ctor-move").on(&gamma).arg("Track{55}")       // (6) create γ by move-defuncting α
                    .beforeCall("ctor").arg(23)                                // (7) create δ with ID 23
                    .beforeCall("assign-move").arg("Track{55}")                // (8) move-assign δ ⟵ γ
                    .beforeEvent("ID", "55")                                   // (9) thus δ now bears the ID 55 (moved α ⟶ γ ⟶ δ)
                    .beforeCall("dtor").arg(55)
                    .beforeEvent("ID", toString(Tracker::DEFUNCT))             // (X) and thus α is now a zombie object
                );
        }
      
      
      /** @test dummy object with a tracking checksum.
       */
      void
      demonstrate_checkObject ()
        {
          CHECK (Dummy::checksum() == 0);
          {
            Dummy dum1;  // picks a random positive int by default...
            CHECK (0 < dum1.getVal() and dum1.getVal() <= 100'000'000);
            CHECK (Dummy::checksum() == dum1.getVal());
            
            Dummy dum2{55};
            CHECK (55 == dum2.getVal());
            CHECK (Dummy::checksum() == dum1.getVal() + 55);
            
            Dummy dum3{move (dum2)};
            CHECK (55 == dum3.getVal());
            CHECK (0  == dum2.getVal());
            
            dum3.setVal (23);
            CHECK (23 == dum3.getVal());
            
            dum1 = move (dum3);
            CHECK (23 == dum1.getVal());
            CHECK (0  == dum2.getVal());
            CHECK (0  == dum3.getVal());
            CHECK (Dummy::checksum() == 23);
            
            Dummy::activateCtorFailure (true);
            try {
                Dummy kabooom;
              }
            catch (int v)
              {
                CHECK (0 < v and v <= 100'000'000);
                CHECK (Dummy::checksum() == 23 + v);
                Dummy::checksum() -= v;
              }
            Dummy::activateCtorFailure (false);
            CHECK (23 == dum1.getVal());
            CHECK (0  == dum2.getVal());
            CHECK (0  == dum3.getVal());
            CHECK (Dummy::checksum() == 23);
          }
          CHECK (Dummy::checksum() == 0);
        }
      
      
      /** @test custom allocator to track memory handling
       *      - use the base allocator to perform raw memory allocation
       *      - demonstrate checksum and diagnostic functions
       *      - use a standard adapter to create objects with `unique_ptr`
       *      - use as _custom allocator_ within STL containers
       */
      void
      demonstrate_checkAllocator()
        {
          // setup a common lock for the tracking objects and -allocator
          auto& log = TrackingAllocator::log;
          Tracker::log.clear("Tracking-Allocator-Test");
          Tracker::log.joinInto(log);
          
          // everything is safe and sound initially....
          CHECK (TrackingAllocator::checksum() == 0, "Testsuite is broken");
          CHECK (TrackingAllocator::use_count() == 0);
          
          { // Test-1 : raw allocations....
            log.event("Test-1");
            TrackingAllocator allo;
            CHECK (TrackingAllocator::use_count() == 1);
            CHECK (TrackingAllocator::numAlloc()  == 0);
            CHECK (TrackingAllocator::numBytes()  == 0);
            
            void* mem = allo.allocate (55);
            CHECK (TrackingAllocator::numAlloc()  == 1);
            CHECK (TrackingAllocator::numBytes()  == 55);
            
            CHECK (allo.manages (mem));
            CHECK (allo.getSize (mem) == 55);
            HashVal memID = allo.getID (mem);
            CHECK (0 < memID);
            CHECK (TrackingAllocator::checksum() == memID*55);
            
            allo.deallocate (mem, 42);                      // note: passing a wrong number here is marked as ERROR in the log
            CHECK (not allo.manages (mem));
            CHECK (allo.getSize (mem) == 0);
            CHECK (allo.getID (mem)   == 0);
            CHECK (TrackingAllocator::use_count() == 1);
            CHECK (TrackingAllocator::numAlloc()  == 0);
            CHECK (TrackingAllocator::numBytes()  == 0);
          }
          CHECK (log.verify("EventLogHeader").on("Tracking-Allocator-Test")
                    .before("logJoin")
                    .beforeEvent("Test-1")
                    .beforeCall("allocate").on(GLOBAL).argPos(0, 55)
                    .beforeEvent("error", "SizeMismatch-42-≠-55")
                    .beforeCall("deallocate").on(GLOBAL).argPos(0, 42)
                );
          CHECK (TrackingAllocator::checksum() == 0);
          
          
          { // Test-2 : attach scoped-ownership-front-End
            log.event("Test-2");
            
            allo::OwnUniqueAdapter<TrackingFactory> uniFab;
            CHECK (sizeof(uniFab) == sizeof(TrackingFactory));
            CHECK (sizeof(uniFab) == sizeof(std::shared_ptr<byte>));
            CHECK (not allo::is_Stateless_v<decltype(uniFab)>);
            
            CHECK (TrackingAllocator::use_count() == 1);
            CHECK (TrackingAllocator::numAlloc()  == 0);
            CHECK (TrackingAllocator::numBytes()  == 0);
            {
              log.event("fabricate unique");
              auto uniqueHandle = uniFab.make_unique<Tracker> (77);
              CHECK (uniqueHandle);
              CHECK (uniqueHandle->val == 77);
              CHECK (TrackingAllocator::use_count() == 2);
              CHECK (TrackingAllocator::numAlloc()  == 1);
              CHECK (TrackingAllocator::numBytes()  == sizeof(Tracker));
              
              // all the default tracking allocators indeed attach to the same pool
              TrackingAllocator allo;
              void* mem = uniqueHandle.get();
              CHECK (allo.manages (mem));
              HashVal memID = allo.getID (mem);
              CHECK (0 < memID);
              CHECK (TrackingAllocator::checksum() == memID*sizeof(Tracker));
              
            }// and it's gone...
            CHECK (TrackingAllocator::use_count() == 1);
            CHECK (TrackingAllocator::numAlloc()  == 0);
            CHECK (TrackingAllocator::numBytes()  == 0);
          }
          
          CHECK (log.verifyEvent("Test-2")
                    .beforeEvent("fabricate unique")
                    .beforeCall("allocate").on(GLOBAL).argPos(0, sizeof(Tracker))
                    .beforeCall("create-Tracker").on(GLOBAL).arg(77)
                    .beforeCall("ctor").on("Tracker").arg(77)
                    .beforeCall("destroy-Tracker").on(GLOBAL)
                    .beforeCall("dtor").on("Tracker").arg(77)
                    .beforeCall("deallocate").on(GLOBAL).argPos(0, sizeof(Tracker))
                );
          CHECK (TrackingAllocator::checksum() == 0);
          
          
          Tracker *t1, *t2, *t3, *t4;
          
          { // Test-3 : use as STL allocator
            log.event("Test-3");
            using SpyVec = std::vector<Tracker, TrackAlloc<Tracker>>;
            
            log.event("fill with 3 default instances");
            SpyVec vec1(3);
            
            int v3 = vec1.back().val;
            
            SpyVec vec2;
            log.event("move last instance over into other vector");
            vec2.emplace_back (move (vec1[2]));
            CHECK (vec2.back().val == v3);
            CHECK (vec1.back().val == Tracker::DEFUNCT);
            
            // capture object locations for log verification
            t1 = & vec1[0];
            t2 = & vec1[1];
            t3 = & vec1[2];
            t4 = & vec2.front();
            log.event("leave scope");
          }
          CHECK (log.verifyEvent("Test-3")
                    .beforeEvent("fill with 3 default instances")
                    .beforeCall("allocate").on(GLOBAL)
                    .beforeCall("ctor").on(t1)
                    .beforeCall("ctor").on(t2)
                    .beforeCall("ctor").on(t3)
                    .beforeEvent("move last instance over into other vector")
                    .beforeCall("allocate").on(GLOBAL)
                    .beforeCall("ctor-move").on(t4)
                    .beforeEvent("leave scope")
                    .beforeCall("dtor").on(t4)
                    .beforeCall("deallocate").on(GLOBAL)
                    .beforeCall("dtor").on(t1)             // (problematic test? order may be implementation dependent)
                    .beforeCall("dtor").on(t2)
                    .beforeCall("dtor").on(t3)
                    .beforeCall("deallocate").on(GLOBAL)
                );
          
          cout << "____Tracking-Allo-Log_________\n"
               << util::join(Tracker::log,      "\n")
               << "\n───╼━━━━━━━━━━━━━━━━━╾────────"<<endl;
          
          CHECK (TrackingAllocator::checksum() == 0);
        }
    };
  
  LAUNCHER (TestTracking_test, "unit common");
  
  
}}} // namespace lib::test::test

