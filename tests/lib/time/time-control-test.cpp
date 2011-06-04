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


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/timequant.hpp"
#include "lib/time/control.hpp"
#include "proc/asset/meta/time-grid.hpp"
#include "lib/meta/generator-combinations.hpp"
#include "lib/scoped-holder.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>

using lib::test::showType;
using boost::lexical_cast;
using util::isnil;
using std::cout;
using std::endl;
using std::string;


namespace lib {
namespace time{
namespace test{
  
  namespace error = lumiera::error;
  
  using lib::ScopedHolder;
  using asset::meta::TimeGrid;
  using lumiera::typelist::Types;
  using lumiera::typelist::InstantiateChainedCombinations;
  using error::LUMIERA_ERROR_UNCONNECTED;
  
  namespace {
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
      : boost::noncopyable
      {
        mutable
        ScopedHolder<TI> received_;
        
      public:
        void
        operator() (TI const& changeValue)  const
          {
            received_.clear();
            received_.create (changeValue);
          }
        
        TI&
        reveivedValue()
          {
            return *received_;
          }
      };
    
    
    
    template<class TAR, class SRC, class BASE>
    struct TestCase
      : BASE
      {
        void
        performTestCases(TimeValue const& o, TimeValue const& c)
          {
            cout << "Test-Case. Target=" << showType<TAR>() 
                 <<" <--feed--- "        << showType<SRC>() 
                 <<endl;
            BASE::performTestCases(o,c);
          }
      };
    
    struct IterationEnd
      {
        void performTestCases(TimeValue const&, TimeValue const&) { }
      };
    
  }
  
  
  
  
  /****************************************************************
   * @test use the time::Control to push a sequence of modifications
   *       to various time entities; in all cases, a suitable change
   *       should be imposed to the target and then a notification signal
   *       should be invoked.
   * @todo cover the cases.....
   *       - change to a given value
   *       - change by an offset
   *       - change using a grid value
   *       - apply an (grid) increment
   */
  class TimeControl_test : public Test
    {
      gavl_time_t
      random_or_get (string arg)
        {
          if (isnil(arg))
            return gavl_time_t (1 + (rand() % 100000)) * GAVL_TIME_SCALE;
          else
            return lexical_cast<gavl_time_t> (arg);
        }
      
      
      virtual void
      run (Arg arg) 
        {
          TimeValue o (random_or_get (pop(arg)));
          TimeValue c (random_or_get (pop(arg)));
          CHECK (o != c, "unsuitable testdata");
          
          // using a 25fps-grid, but with an time origin offset by 1/50sec
          TimeGrid::build("test_grid", FrameRate::PAL, Time(FSecs(1,50)));
          
          QuTime qChange (c, "test_grid");
          FrameNr count(qChange);
          
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
          controller (Time(FSecs(21,5)));
          CHECK (Time(500,10) == target);
          
          CHECK (follower.reveivedValue() == Time::ZERO);
          controller.connectChangeNotification (follower);
          CHECK (follower.reveivedValue() == Time(500,10));
          
          controller (Offset(-Time(500,1)));
          CHECK (Time(0,10) == target);
          CHECK (Time(0,10) == follower.reveivedValue());
        }
      
      
      
      void
      verifyMatrix_of_MutationCases (TimeValue const& o, TimeValue const& c)
        {
          typedef Types<Duration,TimeSpan,QuTime> KindsOfTarget;
          typedef Types<Time,Duration,TimeSpan,QuTime> KindsOfSource;
          typedef InstantiateChainedCombinations< KindsOfTarget
                                                , KindsOfSource
                                                , TestCase
                                                , IterationEnd > TestMatrix;
          
          TestMatrix().performTestCases(o,c);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimeControl_test, "unit common");
  
  
  
}}} // namespace lib::time::test
