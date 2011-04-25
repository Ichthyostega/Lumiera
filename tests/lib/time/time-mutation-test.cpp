/*
  TimeMutation(Test)  -  explicitly changing time spcifications

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
#include "lib/time/mutation.hpp"
#include "proc/asset/meta/time-grid.hpp"
//#include "lib/time/display.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
//#include <cstdlib>
#include <string>

using boost::lexical_cast;
using util::isnil;
//using std::rand;
using std::cout;
using std::endl;
using std::string;

//using lumiera::error::LUMIERA_ERROR_BOTTOM_VALUE;

namespace lib {
namespace time{
namespace test{
  
  using asset::meta::TimeGrid;
  
  namespace {
    inline string
    pop (Arg arg)
    {
      if (isnil (arg)) return "";
      string entry = arg[0];
      arg.erase (arg.begin());
      return entry;
    }
  }
  
  
  /****************************************************************
   * @test cover all basic cases for mutating a time specification.
   *       - change to a given value
   *       - change by an offset
   *       - change using a grid value
   *       - apply an (grid) increment
   */
  class TimeMutation_test : public Test
    {
      gavl_time_t
      random_or_get (string arg)
        {
          if (isnil(arg))
            return 1 + (rand() % 10000);
          else
            return lexical_cast<gavl_time_t> (arg);
        }
      
      struct TestValues
        {
          TimeVar var;
          Duration dur;
          TimeSpan span;
          QuTime quant;
          
          TestValues (TimeValue o)
            : var(o)
            , dur(o)
            , span(o, Offset(o))
            , quant(o, "test_grid")
            { }
        };
      
      
      virtual void
      run (Arg arg) 
        {
          TimeValue o (random_or_get (pop(arg)));
          TimeValue c (random_or_get (pop(arg)));
          
          // using a 25fps-grid, but with an time origin offset by 1/50sec
          TimeGrid::build("test_grid", FrameRate::PAL, Time(FSecs(1,50)));
          
          QuTime qVal (o, "test_grid");
          FrameNr count(qVal);
          
          mutate_by_Value (o, Time(c));
          mutate_by_Offset (o, Offset(c));
          mutate_quantised (o, qVal);
          mutate_by_Increment(o, count);
        } 
      
      
      void
      mutate_by_Value(TimeValue original, Time newStart)
        {
          TestValues t(original);
          
          CHECK (t.span.start() == original);
          t.span.accept (Mutation::changeTime (newStart));
          CHECK (t.span.start() != original);
          CHECK (t.span.start() == newStart);
          
          // instead of invoking directly, we can store and copy mutation messages 
          EncapsulatedMutation change_back(Mutation::changeTime (Time(original)));
          t.span.accept (change_back);
          CHECK (t.span.start() == original);
          
          CHECK (t.quant == original);
          t.quant.accept (Mutation::changeTime (newStart));
          CHECK (t.quant != original);
          CHECK (t.quant == newStart);
          
          // Durations have no start time...
          VERIFY_ERROR (INVALID_MUTATION, t.dur.accept(change_back));
          VERIFY_ERROR (INVALID_MUTATION, t.span.duration().accept(change_back));
          
          CHECK (t.dur == original);
          t.dur.accept (Mutation::changeDuration (Duration(2*t.var)));
          CHECK (t.dur != original);
          CHECK (t.dur == t.var*2);
          
          CHECK (t.span.start() == original);
          CHECK (t.span.duration() == original);
          t.span.accept (Mutation::changeDuration(Duration(3*t.var)));
          CHECK (t.span.duration() != original);
          CHECK (t.span.duration() == t.var*3);  // affects the duration,
          CHECK (t.span.start() == original);   //  while the start time remains unaltered
          
          // can't change the 'duration' of a quantised time point...
          VERIFY_ERROR (INVALID_MUTATION, t.quant.accept(Mutation::changeDuration (Duration(t.var))));
        }
      
      
      void
      mutate_by_Offset (TimeValue original, Offset change)
        {
          TestValues t(original);
          TimeValue& should_be(t.var+=change); // use as ref for verification
          
          CHECK (t.span == original);
          CHECK (t.span != should_be);
          t.span.accept (Mutation::adjust (change));
          CHECK (t.span == should_be);
          
          t.dur.accept (Mutation::adjust (change));
          CHECK (t.dur  == should_be);
          
          t.quant.accept (Mutation::adjust (change));
          CHECK (t.quant == should_be);
          
          // adjustment is cumulative
          EncapsulatedMutation back_off = Mutation::adjust (-change);
          t.span.accept (back_off);
          CHECK (t.span == original);
          t.span.accept (back_off);
          t.span.accept (back_off);
          t.span.accept (back_off);
          CHECK (t.span == Time(original) - 3*change);
        }
      
      
      void
      mutate_quantised (TimeValue o, QuTime change)
        {
          TestValues t(o);
        }
      
      
      void
      mutate_by_Increment (TimeValue o, int change)
        {
          TestValues t(o);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimeMutation_test, "unit common");
  
  
  
}}} // namespace lib::time::test
