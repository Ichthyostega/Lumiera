/*
  TimeMutation(Test)  -  explicitly changing time specifications

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file time-mutation-test.cpp
 ** unit test \ref TimeMutation_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/timequant.hpp"
#include "lib/time/mutation.hpp"
#include "steam/asset/meta/time-grid.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
#include <string>

using boost::lexical_cast;
using util::isnil;
using std::string;


namespace lib {
namespace time{
namespace test{
  
  using steam::asset::meta::TimeGrid;
  
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
  
  
  
  
  /************************************************************//**
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
            return gavl_time_t(1 + rani (100000)) * TimeValue::SCALE;
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
          if (isnil(arg))
            seedRand();
          TimeValue o (random_or_get (pop(arg)));
          TimeValue c (random_or_get (pop(arg)));
          CHECK (o != c, "unsuitable testdata");
          
          // using a 25fps-grid, but with an time origin offset by 1/50sec
          TimeGrid::build("test_grid", FrameRate::PAL, Time(FSecs(1,50)));
          
          QuTime qChange (c, "test_grid");
          FrameNr count(qChange);
          
          mutate_by_Value (o, Time(c));
          mutate_by_Offset (o, Offset(c));
          mutate_quantised (o, qChange);
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
      mutate_quantised (TimeValue original, QuTime change)
        {
          TestValues t(original);
          t.var = change;
          CHECK (Time(change) == t.var); // the underlying raw time value
          
          CHECK (t.span == original);
          t.span.accept (Mutation::materialise (change));
          CHECK (t.span != original);
          CHECK (t.span != t.var);    // really materialised (grid-aligned)
          
          // simulate what happened by explicit operations...
          Secs seconds  = change.formatAs<format::Seconds>();
          PQuant quantiser(change);
          Time materialised (quantiser->materialise(change));
          CHECK (t.span == materialised);
          
          CHECK (t.span.duration() == original); // not affected by mutation as usual
          VERIFY_ERROR (INVALID_MUTATION, t.dur.accept (Mutation::materialise (change)));
                                               //   not surprising, a time point has no duration!!
          
          CHECK (t.quant == original);
          t.quant.accept (Mutation::materialise (change));
          CHECK (t.quant != original);
          CHECK (t.quant == materialised);
          // but note, here we checked the underlying raw value.
          // because t.quant is itself quantised, this might
          // result in a second, chained quantisation finally
          
          // Here accidentally both the change and t.quant use the same grid.
          // For a more contrived example, we try to use a different grid...
          TimeGrid::build("special_funny_grid", 1, Time(0,-10)); // (1 frame per second, zero point at -10s)
          QuTime funny (original, "special_funny_grid");
          funny.accept (Mutation::materialise (change));
          CHECK (funny == t.quant);                      // leading to the same raw value this far
          
          Time doublyQuantised (PQuant(funny)->materialise(funny));
          CHECK (doublyQuantised != materialised);
        }
      
      
      void
      mutate_by_Increment (TimeValue original, int change)
        {
          TestValues t(original);
          
          // without any additional specification,
          // the nudge-Mutation uses a 'natural grid'
          t.span.accept (Mutation::nudge (change));
          t.dur.accept (Mutation::nudge (change));
          
          t.var += Time(FSecs(change));      // natural grid is in seconds
          CHECK (t.span.start() == t.var);
          CHECK (t.dur          == t.var);
          
          // any other grid can be specified explicitly
          t.dur.accept (Mutation::nudge (change, "test_grid"));
          CHECK (t.dur != t.var);
          CHECK (t.dur == t.var + change * FrameRate::PAL.duration());
          // ....this time the change was measured in grid units,
          // taken relative to the origin of the specified grid
          PQuant testGrid = Quantiser::retrieve("test_grid");
          Offset distance (testGrid->timeOf(0), testGrid->timeOf(change));
          CHECK (distance == change * FrameRate::PAL.duration());
          CHECK (t.dur - t.var == distance);
          
          
          
          // To the contrary, *quantised* values behave quite differently...
          long frameNr = t.quant.formatAs<format::Frames>();
          
          t.quant.accept (Mutation::nudge (change));
          CHECK (t.quant != original);
          long frameNr_after = t.quant.formatAs<format::Frames>();
          CHECK (frameNr_after == frameNr + change);
          //i.e. the quantised time's own grid is used
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimeMutation_test, "unit common");
  
  
  
}}} // namespace lib::time::test
