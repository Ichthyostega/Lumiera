/*
  STRESS-TEST-RIG.hpp  -  setup for stress and performance investigation

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

*/

/** @file stress-test-rig.hpp
 ** A test bench to conduct performance measurement series. Outfitted especially
 ** to determine runtime behaviour of the Scheduler and associated parts of the
 ** Lumiera Engine through systematic execution of load scenarios.
 ** 
 ** # Scheduler Stress Testing
 ** 
 ** The point of departure for any stress testing is to show that the subject will
 ** break in controlled ways only. For the Scheduler this can easily be achieved by
 ** overloading until job deadlines are broken. Much more challenging however is the
 ** task to find out about the boundary of regular scheduler operation. This realm
 ** can be defined by the ability of the scheduler to follow and conform to the
 ** timings set out explicitly in the schedule. Obviously, short and localised
 ** load peaks can be accommodated, yet once a persistent backlog builds up,
 ** the schedule starts to slip and the calculation process will flounder.
 ** 
 ** A method to determine such a _»breaking point«_ in a systematic way is based on
 ** building a [synthetic calculation load](\ref test-chain-load.hpp) and establish
 ** the timings of a test schedule based on a simplified model of expected computation
 ** expense. By scaling and condensing these schedule timings, a loss of control can
 ** be provoked, and determined by statistical observation: since the process of
 ** scheduling contains an essentially random component, persistent overload will be
 ** indicated by an increasing variance of the overall runtime, and a departure from
 ** the nominal runtime of the executed schedule.
 ** 
 ** ## Setup
 ** To perform this test scheme, an operational Scheduler is required, and an instance
 ** of the TestChainLoad must be provided, configured with desired load properties.
 ** The _stressFactor_ of the corresponding generated schedule will be the active parameter
 ** of this test, performing a binary search for the _breaking point._
 ** 
 ** ## Observation tools
 ** 
 ** @see TestChainLoad_test
 ** @see SchedulerStress_test
 */


#ifndef VAULT_GEAR_TEST_STRESS_TEST_RIG_H
#define VAULT_GEAR_TEST_STRESS_TEST_RIG_H


#include "vault/common.hpp"
//#include "test-chain-load.hpp"
//#include "lib/test/transiently.hpp"

#include "vault/gear/scheduler.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/iter-explorer.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"//////////////////////////TODO RLY?
//#include "lib/util.hpp"

//#include <functional>
//#include <utility>
//#include <memory>
//#include <string>
//#include <vector>
#include <tuple>
//#include <array>


namespace vault{
namespace gear {
namespace test {
  
  using util::_Fmt;
//  using util::min;
//  using util::max;
//  using util::isnil;
//  using util::limited;
//  using util::unConst;
//  using util::toString;
//  using util::isLimited;
//  using lib::time::Time;
//  using lib::time::TimeValue;
//  using lib::time::FrameRate;
//  using lib::time::Duration;
//  using lib::test::Transiently;
//  using lib::meta::_FunRet;

//  using std::string;
//  using std::function;
//  using std::make_pair;
  using std::make_tuple;
//  using std::forward;
//  using std::string;
//  using std::swap;
//  using std::move;
  
  namespace err = lumiera::error;  //////////////////////////TODO RLY?
  
  namespace { // Default definitions ....
    
  }
  
  namespace stress_test_rig {
    
    template<class CONF>
    class BreakingPointBench
      {
      public:
        auto
        searchBreakingPoint()
          {
            double finalStress{0};
            double avgDelta{0};
            double avgTime{0};
            return make_tuple (finalStress, avgDelta, avgTime);
          }
      };
  }
  
  class StressRig
    : util::NonCopyable
    {
      
    public:
      using usec = std::chrono::microseconds;
      
      template<class CONF>
      static auto
      with()
        {
          return stress_test_rig::BreakingPointBench<CONF>{};
        }
    };
  
  
}}} // namespace vault::gear::test
#endif /*VAULT_GEAR_TEST_STRESS_TEST_RIG_H*/
