/*
  IncidenceCount(Test)  -  observe and evaluate concurrent activations

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

/** @file incidence-count-test.cpp
 ** unit test \ref IncidenceCount_test
 */


#include "lib/test/run.hpp"
#include "lib/test/diagnostic-output.hpp"//////////////TODO RLY?
#include "lib/incidence-count.hpp"
#include "lib/util.hpp"

//#include <string>
#include <thread>


//using std::string;
using util::isLimited;
using std::this_thread::sleep_for;
using std::chrono_literals::operator ""ms;


namespace lib {
namespace test{
  
  
  
  
  /***************************************************************************//**
   * @test verifies capturing and restoring of std::ostream formatting state.
   * @see ios-savepoint.hpp
   */
  class IncidenceCount_test
    : public Test
    {
      void
      run (Arg)
        {
          demonstrate_usage();
          verify_incidentCount();
          verify_multithreadCount();
        }
      
      
      
      /** @test watch time spent in code bracketed by measurement calls.
       * @todo WIP 2/24 ✔ define ⟶ ✔ implement
       */
      void
      demonstrate_usage()
        {
          IncidenceCount watch;
          watch.markEnter();
          sleep_for (1ms);
          watch.markLeave();
          //
          sleep_for (5ms);
          //
          watch.markEnter();
          sleep_for (1ms);
          watch.markLeave();
          
          double time = watch.calcCumulatedTime();
          CHECK (time > 1900);
          CHECK (time < 2500);
        }
      
      
      /** @test TODO verify proper counting of possibly overlapping incidences
       * @todo WIP 2/24 ✔ define ⟶ 🔁 implement
       */
      void
      verify_incidentCount()
        {
          IncidenceCount watch;
          watch.expectThreads(1)
               .expectIncidents(20);
          
          watch.markEnter(1);
          sleep_for (1ms);
          watch.markEnter(3);
          sleep_for (2ms);
          watch.markEnter(2);
          watch.markLeave(3);
          sleep_for (1ms);
          watch.markLeave(1);
          watch.markEnter(3);
          sleep_for (3ms);
          watch.markEnter(1);
          watch.markLeave(2);
          sleep_for (1ms);
          watch.markLeave(3);
          sleep_for (1ms);
          watch.markLeave(1);
          
          auto stat = watch.evaluate();
SHOW_EXPR(stat.cumulatedTime);
SHOW_EXPR(stat.coveredTime);
SHOW_EXPR(stat.eventCnt);
SHOW_EXPR(stat.activationCnt);
SHOW_EXPR(stat.cntCase(0));
SHOW_EXPR(stat.cntCase(1));
SHOW_EXPR(stat.cntCase(2));
SHOW_EXPR(stat.cntCase(3));
SHOW_EXPR(stat.cntCase(4));
SHOW_EXPR(stat.timeCase(0));
SHOW_EXPR(stat.timeCase(1));
SHOW_EXPR(stat.timeCase(2));
SHOW_EXPR(stat.timeCase(3));
SHOW_EXPR(stat.timeCase(4));
SHOW_EXPR(stat.cntThread(0));
SHOW_EXPR(stat.cntThread(1));
SHOW_EXPR(stat.timeThread(0));
SHOW_EXPR(stat.timeThread(1));
         CHECK (isLimited (15500, stat.cumulatedTime, 17500));   // ≈ 16ms
         CHECK (isLimited ( 8500, stat.coveredTime,   10000));   // ≈ 9ms
         CHECK (10== stat.eventCnt);
         CHECK (5 == stat.activationCnt);
         CHECK (0 == stat.cntCase(0));
         CHECK (2 == stat.cntCase(1));
         CHECK (1 == stat.cntCase(2));
         CHECK (2 == stat.cntCase(3));
         CHECK (0 == stat.cntCase(4));
         CHECK (0 == stat.timeCase(0));
         CHECK (isLimited ( 5500, stat.timeCase(1), 6800));      // ≈ 6ms
         CHECK (isLimited ( 3500, stat.timeCase(2), 4500));      // ≈ 4ms
         CHECK (isLimited ( 5500, stat.timeCase(3), 6800));      // ≈ 6ms
         CHECK (0 == stat.timeCase(4));
         CHECK (5 == stat.cntThread(0));
         CHECK (0 == stat.cntThread(1));
         CHECK (stat.cumulatedTime == stat.timeThread(0));
         CHECK (0                  == stat.timeThread(1));
         CHECK (1 > abs(stat.cumulatedTime - (stat.timeCase(1) + stat.timeCase(2) + stat.timeCase(3))));
        }
      
      
      /** @test TODO verify thread-safe operation under pressure
       * @todo WIP 2/24 🔁 define ⟶ implement
       */
      void
      verify_multithreadCount()
        {
          UNIMPLEMENTED("verify thread-safe operation under pressure");
        }
    };
  
  LAUNCHER (IncidenceCount_test, "unit common");
  
  
}} // namespace lib::test

