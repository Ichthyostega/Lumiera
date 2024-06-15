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
#include "lib/format-cout.hpp"

#include <string>

using std::string;
using util::toString;


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
          
          log.note("type=ID",alpha.val);                                       // (2) α has an random ID
          {
            Tracker beta{55};                                                  // (3) create β
            alpha = beta;                                                      // (4) assign α ≔ β
          }
          log.note("type=ID",alpha.val);                                       // (5) thus α now also bears the ID 55 of β
          Tracker gamma = move(alpha);                                         // (6) create γ by move-defuncting α
          {
            Tracker delta(23);                                                 // (7) create δ with ID 23
            delta = move(gamma);                                               // (8) move-assign δ ⟵ γ
            log.note("type=ID",delta.val);                                     // (9) thus δ now bears the ID 55 (moved α ⟶ γ ⟶ δ)
          }
          log.note("type=ID",alpha.val);                                       // (X) and thus α is now a zombie object
          
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
          UNIMPLEMENTED ("dummy");
        }
      
      
      /** @test custom allocator to track memory handling.
       */
      void
      demonstrate_checkAllocator ()
        {
          UNIMPLEMENTED ("allo");
        }
    };
  
  LAUNCHER (TestTracking_test, "unit common");
  
  
}}} // namespace lib::test::test

