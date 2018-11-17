/*
  TimeControl(Test)  -  mutating time entities with life connection and feedback

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file time-control-test.cpp
 ** unit test \ref TimeControl_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/timequant.hpp"
#include "lib/time/control.hpp"

#include "lib/meta/generator-combinations.hpp"
#include "steam/asset/meta/time-grid.hpp"
#include "lib/scoped-holder.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
#include <string>
#include <limits>

using boost::lexical_cast;
using util::typeStr;
using util::isnil;
using std::string;


namespace lib {
namespace time{
namespace test{
  
  namespace error = lumiera::error;
  
  using lib::ScopedHolder;
  using steam::asset::meta::TimeGrid;
  using lib::meta::Types;
  using lib::meta::InstantiateChainedCombinations;
  using error::LERR_(UNCONNECTED);
  
  
  
  namespace { // Test setup and helpers....
    
    inline string
    pop (Arg arg)
    {
      if (isnil (arg)) return "";
      string entry = arg[0];
      arg.erase (arg.begin());
      return entry;
    }
    
    
    /**
     * Mock object to receive change notifications.
     * A copy of the most recently received value
     * is memorised within an embedded buffer,
     * to be verified by the actual tests.
     */
    template<class TI>
    class TestListener
      : util::NonCopyable
      {
        mutable
        ScopedHolder<TI> received_;
        
      public:
        TestListener()
          { 
            received_.create (Time::ZERO); 
          }
        
        TestListener(TI const& initialValue)
          { 
            received_.create (initialValue);
          }
        
        void
        operator() (TI const& changeValue)  const
          {
            received_.clear();
            received_.create (changeValue);
          }
        
        TI const&
        receivedValue()  const
          {
            return *received_;
          }
      };
      
  }//(End)Test helpers
  
  
  
  
  /*******************************************************************//**
   * @test use the time::Control to push a sequence of modifications to
   *       various time entities; in all cases, a suitable change should
   *       be imposed to the target and then a notification signal
   *       should be invoked.
   *       
   *       After covering a simple basic case, this test uses
   *       template metaprogramming techniques to build a matrix of all
   *       possible type combinations and then performs a standard test
   *       sequence for each of these type combinations. Within this
   *       test sequence, verification functions are invoked, which
   *       are defined with specialisations to adapt for the various
   *       types to be covered.
   */
  class TimeControl_test : public Test
    {
      gavl_time_t
      random_or_get (string arg)
        {
          if (isnil(arg))
            return gavl_time_t (1 + (rand() % 100000)) * TimeValue::SCALE;
          else
            return lexical_cast<gavl_time_t> (arg);
        }
      
      
      virtual void
      run (Arg arg) 
        {
          TimeValue o (random_or_get (pop(arg)));
          TimeValue c (random_or_get (pop(arg)));
          CHECK (c!=Time::ZERO && o != c, "unsuitable testdata");
          
          // 25fps-grid, but with an time origin offset by 1/50sec
          TimeGrid::build("test_grid_PAL", FrameRate::PAL, Time(FSecs(1,50)));
          
          // disjoint NTSC-framerate grid for grid aligned changes
          TimeGrid::build("test_grid_NTSC", FrameRate::NTSC);
          
          verifyBasics();
          verifyMatrix_of_MutationCases(o,c);
        } 
      
      
      void
      verifyBasics()
        {
          TimeSpan target(Time(0,10), FSecs(5));
          
          Control<Time> controller;
          TestListener<Time> follower;
          
          VERIFY_ERROR (UNCONNECTED, controller(Time::ZERO) );
          
          target.accept (controller);
          CHECK (Time(0,10) == target);
          controller (Time(FSecs(21,2)));
          CHECK (Time(500,10) == target);
          
          CHECK (follower.receivedValue() == Time::ZERO);
          controller.connectChangeNotification (follower);
          CHECK (follower.receivedValue() == Time(500,10));
          
          controller (Offset(-Time(500,1)));
          CHECK (Time(0,9) == target);
          CHECK (Time(0,9) == follower.receivedValue());
        }
      
      
      /** @test cover all possible combinations of input change values
       *        and target time value entities to be handled by time::Control.
       *        Each of these cases executes a standard test sequence, which is
       *        defined in TestCase#performTestSequence
       */
      void verifyMatrix_of_MutationCases (TimeValue const& o, TimeValue const& c);
    };
  
  
  namespace { // Implementation: Matrix of individual test combinations
    
    
    template<class T>
    inline bool
    isDuration()
      {
        return std::is_same<T,Duration>::value;
      }
    
    template<class T>
    inline bool
    isQuTime()
      {
        return std::is_same<T,QuTime>::value;
      }
    
    template<class T>
    inline TimeValue
    materialise (T const& someTime)
      {
        return someTime;
      }
    inline TimeValue
    materialise (QuTime const& alignedTime)
      {
        PQuant grid(alignedTime);
        return grid->materialise (alignedTime);
      }
    
    
    template<class TAR>
    struct TestTarget
      {
        static TAR
        build (TimeValue const& org)
          {
            return TAR(org);
          }
      };
    
    template<>
    struct TestTarget<TimeSpan>
      {
        static TimeSpan
        build (TimeValue const& org)
          {
            return TimeSpan (org, FSecs(3,2));
          }
      };
    
    template<>
    struct TestTarget<QuTime>
      {
        static QuTime
        build (TimeValue const& org)
          {
            return QuTime (org, "test_grid_PAL");
          }
      };
    
    
    template<class SRC>
    struct TestChange
      {
        static SRC
        prepareChangeValue (TimeValue const& c)
        {
          return SRC(c);
        }
      };
    
    template<>
    struct TestChange<TimeSpan>
      {
        static TimeSpan
        prepareChangeValue (TimeValue const& c)
        {
          return TimeSpan (c, Duration(c));
        }
      };
    
    template<>
    struct TestChange<QuTime>
      {
        static QuTime
        prepareChangeValue (TimeValue const& c)
        {
          return QuTime (c, "test_grid_NTSC");
        }
      };
    
    
    
    template<class TAR, class SRC>
    void
    ____verify_wasChanged (TAR const& target, TimeValue const& org, SRC const& change)
    {
      if (isDuration<TAR>())
        {
          CHECK (target == org, "Logic error: Duration was changed by time value");
        }
      else
      if (isDuration<SRC>())
        {
          CHECK (target == org, "Logic error: Duration used to change time value");
        }
      else
      if (isQuTime<SRC>())
        {
          CHECK (target != org);
          CHECK (target == materialise(change));
        }
      else
        {
          CHECK (target != org);
          CHECK (target == change);
        }
    }
    
    void
    ____verify_wasChanged (Duration const& target, TimeValue const& org, Duration const& otherDuration)
    {
      CHECK (target != org);
      CHECK (target == otherDuration);
    }
    void
    ____verify_wasChanged (Duration const& target, TimeValue const& org, TimeSpan const& span_as_change)
    {
      CHECK (target != org);
      CHECK (target == span_as_change.duration());
    }
    void
    ____verify_wasChanged (TimeSpan const& target, TimeValue const& org, Duration const& changedDur)
    {
      CHECK (target == org, "Logic error: Duration was used as start point of the target TimeSpan");
      CHECK (target.duration() != Time(FSecs(3,2)), "length of the timespan should have been changed");
      CHECK (target.duration() == changedDur);
    }
    
    
    
    template<class TAR>
    void
    ____verify_wasOffset (TAR const& target, TAR const& refState, Offset const& offset)
    {
      CHECK (target != refState);
      CHECK (target == Time(refState)+offset);
    }
    
    template<class TAR>
    void
    ____verify_wasOffsetBack (TAR const& target, TAR const& refState)
    {
      CHECK (target == refState);
    }
    
    
    template<class TAR>
    void
    ____verify_nudged (TAR const& target, TAR const& refState, FrameCnt offsetSteps)
    {
      CHECK (target != refState  || !offsetSteps);
      CHECK (target == Time(refState)+Time(FSecs(offsetSteps)));
    }
    template<>
    void
    ____verify_nudged (QuTime const& target, QuTime const& refState, FrameCnt offsetSteps)
    {
      CHECK (target != refState  || !offsetSteps);
      CHECK (target == Time (materialise(refState))
                     + Offset(offsetSteps, FrameRate::PAL));
    }
    
    
    template<class TAR, class SRC>
    void
    ____verify_notification (TAR const& target, TestListener<SRC> const& follower)
    {
      if (isDuration<SRC>())
        {
          CHECK (materialise(target) == follower.receivedValue()
                 ||    Duration::NIL == follower.receivedValue() );
        }
      else
      if (isQuTime<TAR>())
        {
          CHECK (materialise (target) == follower.receivedValue());
        }
      else
        {
          CHECK (target == follower.receivedValue());
        }
    }
    void
    ____verify_notification (TimeSpan const& targetTimeSpan, TestListener<Duration> const& follower)
    {
      CHECK (follower.receivedValue() == targetTimeSpan.duration());
    }
    void
    ____verify_notification (Duration const& target, TestListener<Duration> const& follower)
    {
      CHECK (target == follower.receivedValue());
    }
    void
    ____verify_notification (Duration const& targetDuration, TestListener<TimeSpan> const& follower)
    {
      CHECK (Time::ZERO     == follower.receivedValue());
      CHECK (targetDuration == follower.receivedValue().duration());
    }
    
    
    
    
    template< class TAR     ///< type of the target time value entity to receive changes
            , class SRC     ///< type of the time value to be imposed as change
            , class BASE
            >
    struct TestCase
      : BASE
      {
        void
        performTestSequence(TimeValue const& org, TimeValue const& c)
          {
            cout << "Test-Case. Target=" << typeStr<TAR>()
                 << "\t <--feed--- "     << typeStr<SRC>()
                 << endl;
            
            // test subject
            Control<SRC> controller;
            
            TAR target = TestTarget<TAR>::build(org);
            SRC change = TestChange<SRC>::prepareChangeValue(c);
            TestListener<SRC> follower(change);
            
            controller.connectChangeNotification(follower);
            target.accept (controller);
            
            controller (change);
            ____verify_wasChanged (target, org, change);
            ____verify_notification(target,follower);
            
            TAR refState(target);
            
            Offset offset(c);
            controller (offset);
            ____verify_wasOffset (target, refState, offset);
            controller (-offset);
            ____verify_wasOffsetBack (target, refState);
            ____verify_notification(target,follower);
            
            controller (0);
            ____verify_nudged (target, refState, 0);
            ____verify_notification(target,follower);
            
            controller (+1);
            ____verify_nudged (target, refState, +1);
            ____verify_notification(target,follower);
            
            controller (-2);
            ____verify_nudged (target, refState, -1);
            ____verify_notification(target,follower);
            
            int maxInt = std::numeric_limits<int>::max();
            int minInt = std::numeric_limits<int>::min();
            
            controller (maxInt);
            ____verify_nudged (target, refState, -1LL + maxInt);
            ____verify_notification(target,follower);
            
            controller (minInt);
            ____verify_nudged (target, refState, -1LL + maxInt+minInt);
            ____verify_notification(target,follower);
            
            
            // tail recursion: further test combinations....
            BASE::performTestSequence(org,c);
          }
      };
    
    struct IterationEnd
      {
        void performTestSequence(TimeValue const&, TimeValue const&) { }
      };
    
  }//(End)Implementation Test-case matrix
  
  
  void
  TimeControl_test::verifyMatrix_of_MutationCases (TimeValue const& origVal, TimeValue const& change)
  {
    typedef Types<Duration,TimeSpan,QuTime>                KindsOfTarget;  // time entities to receive value changes
    typedef Types<TimeValue,Time,Duration,TimeSpan,QuTime> KindsOfSource;  // time entities to be used as change values
    typedef InstantiateChainedCombinations< KindsOfTarget
                                          , KindsOfSource
                                          , TestCase                       // template to be instantiated for each type
                                          , IterationEnd > TestMatrix;
    
    TestMatrix().performTestSequence(origVal, change);
  }
  
  
  
  
  
  /** Register this test class... */
  LAUNCHER (TimeControl_test, "unit common");
  
  
  
}}} // namespace lib::time::test
