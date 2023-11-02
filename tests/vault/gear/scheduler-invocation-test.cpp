/*
  SchedulerInvocation(Test)  -  verify queue processing in the scheduler

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

/** @file scheduler-invocation-test.cpp
 ** unit test \ref SchedulerInvocation_test
 */


#include "lib/test/run.hpp"
#include "vault/gear/scheduler-invocation.hpp"
#include "lib/util.hpp"
#include "lib/test/diagnostic-output.hpp"///////////////////TODO

using test::Test;
using util::isSameObject;


namespace vault{
namespace gear {
namespace test {
  
  
  
  
  /********************************************************************//**
   * @test Scheduler Layer-1: queue processing and invocation by priority.
   * @see SchedulerCommutator_test
   * @see SchedulerUsage_test
   */
  class SchedulerInvocation_test : public Test
    {
      
      virtual void
      run (Arg)
        {
           simpleUsage();
           verify_Queuing();
           verify_WaterLevel();
           verify_Significance();
           verify_stability();
           verify_isDue();
        }
      
      
      /** @test demonstrate a simple usage scenario of data passing
       */
      void
      simpleUsage()
        {
          SchedulerInvocation sched;
          Activity activity;
          Time when{1,2,3};
          Time dead{2,3,4};
          
          CHECK (not sched.peekHead());
          
          sched.instruct (activity, when, dead);
          sched.feedPrioritisation();
          CHECK (sched.peekHead());
          
          Activity* head = sched.pullHead();
          CHECK (not sched.peekHead());
          CHECK (isSameObject (*head, activity));
        }
      
      
      
      /** @test verify records are passed properly through the queues
       *        - add multiple elements to the instruct queue
       *        - after `feedPrioritisation` these appear as output
       */
      void
      verify_Queuing()
        {
          SchedulerInvocation sched;
          Activity one{1u,1u};
          Activity two{2u,2u};
          Activity ree{3u,3u};
          Time t{5,5};
          
          sched.instruct (one, t);
          sched.instruct (two, t);
          sched.instruct (ree, t);
          CHECK (not sched.peekHead());
          
          sched.feedPrioritisation();
          CHECK (isSameObject (*sched.pullHead(), one));
          CHECK (isSameObject (*sched.pullHead(), two));
          CHECK (isSameObject (*sched.pullHead(), ree));
          CHECK (not sched.peekHead());
          CHECK (sched.empty());
        }
      
      
      
      /** @test verify the given time point is utilised for prioritisation
       *        - order at output is determined by the time spec
       *        - even later added elements can push back
       *          previously visible elements at head
       */
      void
      verify_WaterLevel()
        {
          SchedulerInvocation sched;
          Activity a1{1u,1u};
          Activity a2{2u,2u};
          Activity a3{3u,3u};
          Activity a4{4u,4u};
          
          sched.instruct (a2, Time{2,0});
          sched.instruct (a4, Time{4,0});
          sched.feedPrioritisation();
          CHECK (isSameObject (*sched.peekHead(), a2));
          
          sched.instruct (a3, Time{3,0});
          sched.instruct (a1, Time{1,0});
          CHECK (isSameObject (*sched.peekHead(), a2));
          
          sched.feedPrioritisation();
          CHECK (isSameObject (*sched.pullHead(), a1));
          CHECK (isSameObject (*sched.pullHead(), a2));
          CHECK (isSameObject (*sched.pullHead(), a3));
          CHECK (isSameObject (*sched.pullHead(), a4));
        }
      
      
      
      /** @test sort order is not necessarily stable
       *        if using identical time specs on entrance
       */
      void
      verify_stability()
        {
          SchedulerInvocation sched;
          Activity a1{1u,1u};
          Activity a2{2u,2u};
          Activity a3{3u,3u};
          Activity a4{4u,4u};
          
          sched.feedPrioritisation (a1, Time{0,5});
          sched.feedPrioritisation (a2, Time{0,5});
          sched.feedPrioritisation (a3, Time{0,5});
          sched.feedPrioritisation (a4, Time{0,4});
          CHECK (isSameObject (*sched.pullHead(), a4));
          CHECK (isSameObject (*sched.pullHead(), a3));
          CHECK (isSameObject (*sched.pullHead(), a1));
          CHECK (isSameObject (*sched.pullHead(), a2));
          CHECK (not sched.pullHead());
        }
      
      
      
      /** @test the entry appearing at head _is due_
       *        when its time is at or before current time.
       */
      void
      verify_isDue()
        {
          SchedulerInvocation sched;
          Activity a1{1u,1u};
          
          sched.feedPrioritisation (a1, Time{0,5});
          CHECK (isSameObject (*sched.peekHead(), a1));
          CHECK (    sched.isDue (Time{0,10}));
          CHECK (    sched.isDue (Time{0,5}));
          CHECK (not sched.isDue (Time{0,1}));
          
          sched.pullHead();
          CHECK (not sched.peekHead());
          CHECK (not sched.isDue (Time{0,1}));
          CHECK (not sched.isDue (Time{0,10}));
        }
      
      
      
      /** @test verify that obsoleted or rejected entries are dropped transparently
       *      - add entries providing extra information regarding significance
       *      - verify that missing the deadline is detected
       *      - entries past deadline are marked _outdated_ (will be dropped by Layer-2)
       *      - entries can be tagged with an ManifestationID, allowing to enable
       *        or disable visibility for a »family« of schedule entries
       *      - use the flag for _compulsory_ entries, allowing to detect
       *        a fatal _jammed_ situation where the head entry is
       *        out of time, while marked mandatory to process.
       */
      void
      verify_Significance()
        {
          SchedulerInvocation sched;
          Activity act;
          
          sched.feedPrioritisation (act, Time{2,0}, Time{3,0});
          CHECK (Time(2,0) == sched.headTime());
          CHECK (    sched.isDue    (Time{2,0}));
          CHECK (not sched.isMissed (Time{2,0}));
          CHECK (not sched.isMissed (Time{3,0}));
          CHECK (    sched.isMissed (Time{4,0}));
          
          CHECK (not sched.isOutdated (Time{2,0}));
          CHECK (not sched.isOutdated (Time{3,0}));
          CHECK (    sched.isOutdated (Time{4,0}));
          
          sched.feedPrioritisation (act, Time{1,0}, Time{3,0}, ManifestationID{5});
          CHECK (Time(1,0) == sched.headTime());
          CHECK (    sched.isOutdated (Time{1,0}));
          CHECK (not sched.isMissed (Time{1,0}));
          
          sched.activate (ManifestationID{5});
          CHECK (Time(1,0) == sched.headTime());
          CHECK (    sched.isDue      (Time{1,0}));
          CHECK (not sched.isOutdated (Time{1,0}));
          CHECK (not sched.isOutdated (Time{3,0}));
          CHECK (    sched.isOutdated (Time{4,0}));
          CHECK (    sched.isMissed   (Time{4,0}));
          CHECK (    sched.isDue      (Time{4,0}));
          
          sched.drop (ManifestationID{5});
          CHECK (Time(1,0) == sched.headTime());
          CHECK (    sched.isOutdated (Time{1,0}));
          CHECK (    sched.isOutdated (Time{4,0}));
          CHECK (    sched.isMissed (Time{4,0}));
          CHECK (not sched.isMissed (Time{1,0}));
          CHECK (    sched.isDue    (Time{1,0}));
          
          sched.feedPrioritisation (act, Time{0,0}, Time{2,0}, ManifestationID{5}, true);
          CHECK (Time(0,0) == sched.headTime());                               //  ^^^^ marked as compulsory
          CHECK (not sched.isMissed (Time{1,0}));
          CHECK (    sched.isOutdated (Time{1,0}));                            // marked as outdated since manifestation 5 is not activated
          
          sched.activate (ManifestationID{5});
          CHECK (not sched.isOutdated (Time{1,0}));
          CHECK (not sched.isOutOfTime(Time{2,0}));                            // still OK /at/ deadline
          CHECK (    sched.isOutOfTime(Time{3,0}));                            // ↯ past deadline yet marked as compulsory
          CHECK (    sched.isOutdated (Time{3,0}));
          CHECK (    sched.isMissed   (Time{3,0}));
          
          sched.drop (ManifestationID{5});
          CHECK (Time(0,0) == sched.headTime());
          CHECK (    sched.isOutdated (Time{1,0}));
          CHECK (not sched.isOutOfTime(Time{2,0}));
          CHECK (not sched.isOutOfTime(Time{3,0}));                            // the disabled manifestation masks the fatal out-of-time state
          CHECK (    sched.isOutdated (Time{3,0}));
          CHECK (    sched.isMissed   (Time{3,0}));
          
          sched.pullHead();
          CHECK (Time(1,0) == sched.headTime());
          CHECK (    sched.isOutdated (Time{1,0}));
          CHECK (    sched.isDue      (Time{1,0}));
          
          sched.pullHead();
          CHECK (Time(2,0) == sched.headTime());
          CHECK (not sched.isOutdated (Time{2,0}));
          CHECK (    sched.isOutdated (Time{4,0}));
          CHECK (    sched.isMissed   (Time{4,0}));
          CHECK (    sched.isDue      (Time{4,0}));
          
          sched.pullHead();
          CHECK (Time::NEVER == sched.headTime());
          CHECK (not sched.isMissed   (Time{4,0}));
          CHECK (not sched.isOutdated (Time{4,0}));
          CHECK (not sched.isDue      (Time{4,0}));
          CHECK (sched.empty());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SchedulerInvocation_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
