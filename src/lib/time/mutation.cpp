/*
  Mutation  -  changing and adjusting time values

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file mutation.cpp 
 ** A collection of concrete forms of time mutation.
 ** This compilation unit defines some "mutators" based on explicitly given time values,
 ** to be applied to time durations, time spans and quantised time values. The intended
 ** usage is to build these "mutators" inline through use of the corresponding static
 ** builder functions defined in class Mutation. All the mutators defined here are
 ** "polymorphic value objects", i.e. they can be treated like simple time values, while
 ** actually hiding the concrete implementation and just exposing the #Mutation interface.
 ** 
 ** @remarks these mutators are for casual use at places where you'd intuitively just want
 **          set a simple more or less hard wired value. The benefit of this seemingly complicated
 **          solution is that all the tricky corner cases for grid aligned time values will be
 **          treated automatically. Incidentally, there is another, quite different use case
 **          for time mutations, when it comes to receive continuous and ongoing changes to
 **          a time value (e.g. a cursor). This separate use case is handled by time::Control
 ** 
 ** @see time::Control
 ** @see lib::PolymorphicValue
 ** @see TimeMutation_test
 ** @see TimeControl_test
 **
 */


#include "lib/error.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/timequant.hpp"
#include "lib/time/timecode.hpp"
#include "lib/time/mutation.hpp"
#include "lib/util-quant.hpp"


namespace error = lumiera::error;

namespace lib {
namespace time {
  
  
  LUMIERA_ERROR_DEFINE (INVALID_MUTATION, "Changing a time value in this way was not designated");
  
  
  Mutation::~Mutation()   { }  // emit VTable here....
  
  
  
  
  
  /* Warning: all the classes defined here must be of size below MUTATION_IMPL_SIZE */ 
  
  /**
   * concrete time value mutation:
   * impose fixed new start time.
   */
  class SetNewStartTimeMutation
    : public ClonableMutation
    {
      TimeValue newTime_;
      
      virtual void
      change (Duration&)  const
        {
          throw error::Logic ("mutating the start point of a pure Duration doesn't make sense"
                             , LUMIERA_ERROR_INVALID_MUTATION);
        }
      
      
      virtual void
      change (TimeSpan& target)  const
        {
          imposeChange (target, newTime_);
        }
      
      
      /** @note the re-quantisation happens automatically
       *  when the (changed) QuTime is materialised */
      virtual void
      change (QuTime& target)    const
        {
          imposeChange (target, newTime_);
        }
      
      
    public:
      explicit
      SetNewStartTimeMutation (TimeValue t)
        : newTime_(t)
        { }
    };
  
  
  /**
   * concrete time value mutation:
   * set a new overall duration for an extended timespan.
   */
  class SetNewDuration
    : public ClonableMutation
    {
      Duration changedDuration_;
      
      virtual void
      change (Duration& target)  const
        {
          imposeChange (target, changedDuration_);
        }
      
      
      virtual void
      change (TimeSpan& target)  const
        {
          imposeChange (target.duration(), changedDuration_);
        }
      
      
      /** @note the re-quantisation happens automatically
       *  when the (changed) QuTime is materialised */
      virtual void
      change (QuTime&)    const
        {
          throw error::Logic ("mutating the duration of a (quantised) time point doesn't make sense"
                             , LUMIERA_ERROR_INVALID_MUTATION);
        }
      
      
    public:
      explicit
      SetNewDuration (Duration dur)
        : changedDuration_(dur)
        { }
    };
  
  
  /**
   * concrete time value mutation:
   * adjust the given time entity by an offset amount.
   */
  class ImposeOffsetMutation
    : public ClonableMutation
    {
      Offset adjustment_;
      
      virtual void
      change (Duration& target) const
        {
          imposeChange (target, adjustment_);
        }
      
      
      virtual void
      change (TimeSpan& target) const
        {
          imposeChange (target, adjustment_);
        }
      
      
      /** @note the re-quantisation happens automatically
       *  when the (changed) QuTime is materialised */
      virtual void
      change (QuTime& target)   const
        {
          imposeChange (target, adjustment_);
        }
      
      
    public:
      explicit
      ImposeOffsetMutation (Offset adj)
        : adjustment_(adj)
        { }
    };
  
  
  /**
   * concrete time value mutation:
   * make the grid aligned time value explicit,
   * and impose the resulting value to the given
   * time points (or start points).
   */
  class MaterialiseIntoTarget
    : public SetNewStartTimeMutation
    {
    public:
      explicit
      MaterialiseIntoTarget (QuTime const& quant)
        : SetNewStartTimeMutation (PQuant(quant)->materialise(quant))
        { }
    };
  
  
  /**
   * concrete time value mutation:
   * nudge target value by the given number of 'steps',
   * relative to the given grid.
   */
  class NudgeMutation
    : public ImposeOffsetMutation
    {
      
      static Offset
      materialiseGridPoint (PQuant const& grid, int steps)
        {
          REQUIRE (grid);
          return Offset(grid->timeOf(0), grid->timeOf(steps));
        }
      
    public:
      NudgeMutation (int relativeSteps, PQuant const& grid)
        : ImposeOffsetMutation(materialiseGridPoint (grid,relativeSteps))
        { }
    };
  
  
  /**
   * concrete time value mutation:
   * nudge based on a implicit grid, which is
   * either a quantised target value's own grid,
   * or a \em natural grid.
   * @note currently the natural grid is hard wired,
   *       just interpreting the step parameter as
   *       offset in seconds.
   * @see mutation#imposeChange (TimeValue, int)
   * @see mutation#imposeChange (QuTime, int)
   */
  class NaturalNudgeMutation
    : public ClonableMutation
    {
      int steps_;
      
      virtual void
      change (Duration& target) const
        {
          imposeChange (target, steps_);
        }
      
      
      virtual void
      change (TimeSpan& target) const
        {
          imposeChange (target, steps_);
        }
      
      
      /** @note special treatment: use the quantised time's own grid */
      virtual void
      change (QuTime& target)   const
        {
          imposeChange (target, steps_);
        }
      
   
    public:
      explicit
      NaturalNudgeMutation (int relativeSteps)
        : steps_(relativeSteps)
        { }
    };
  
  
  
  
  
  
  /** Convenience factory to yield a simple Mutation changing the absolute start time.
   *  This whole procedure might look quite inefficient, but actually most of the
   *  abstractions are removed at runtime, leaving only a single indirection 
   *  through the VTable of the Mutation Interface.
   * @throw  error::Logic when attempting to change the (non existent) start time of a Duration
   * @return EncapsulatedMutation, which is an "polymorphic value object",
   *         actually carrying an embedded Mutation subclass with the new start time
   */
  EncapsulatedMutation
  Mutation::changeTime (Time newStartTime)
  {
    return EncapsulatedMutation::build<SetNewStartTimeMutation> (newStartTime);
  }
  
  
  /** Convenience factory: simple Mutation to adjust the duration or length of a timespan
   * @throw error::Logic when attempting to change the "duration" of a quantised time point
   * @return EncapsulatedMutation, carrying the new duration value to impose
   */
  EncapsulatedMutation
  Mutation::changeDuration (Duration changedDur)
  {
    return EncapsulatedMutation::build<SetNewDuration> (changedDur);
  }
  
  
  /** Convenience factory: simple Mutation to adjust the duration or length of a timespan
   * @throw error::Logic when attempting to change the "duration" of a quantised time point
   * @return EncapsulatedMutation, carrying the new duration value to impose
   */
  EncapsulatedMutation
  Mutation::adjust (Offset change)
  {
    return EncapsulatedMutation::build<ImposeOffsetMutation> (change);
  }
  
  
  /** Convenience factory: materialise the given quantised time into an explicit fixed
   *  internal time value, according to the underlying time grid; impose the resulting
   *  value then as new time point or start point to the target
   * @note same as materialising and then invoking #changeTime
   */
  EncapsulatedMutation
  Mutation::materialise (QuTime const& gridAlignedTime)
  {
    return EncapsulatedMutation::build<MaterialiseIntoTarget> (gridAlignedTime);
  }
  
  
  /** build a time mutation to \em nudge the target time value by an offset,
   *  defined as number of steps on an implicit nudge grid. If the target is an
   *  continuous (not quantised) time value or duration, an internal 'default nudge grid'
   *  will be used to calculate the offset value. Typically, this grid counts in seconds.
   *  To the contrary, when the target is a quantised value, it will be aligned to the
   *  grid point relative to the current value's next grid point, measured in number
   *  of steps. This includes \em materialising the internal time to the exact grid
   *  position. If especially the adjustment is zero, the internal value will
   *  be changed to literally equal the current value's next grid point.
   */
  EncapsulatedMutation
  Mutation::nudge (int adjustment)
  {
    return EncapsulatedMutation::build<NaturalNudgeMutation> (adjustment);
  }
  
  
  /** build a time mutation to \em nudge the target time value; the nudge time grid
   *  is specified explicitly here, instead of using a global or 'natural' nudge grid.
   *  In case the target itself is a quantised time value, a chaining of the two
   *  grids will happen: first, the nudge grid is used to get an offset value,
   *  according to the number of steps, then this offset is applied to the
   *  raw value underlying the quantised target. If this resulting target
   *  value later will be cast into any kind of time code or materialised
   *  otherwise, the quantised value's own grid will apply as well,
   *  resulting in the net result of two quantisation operations
   *  being applied in sequence.  
   * @param adjustment number of grid steps to apply as offset
   * @param grid reference to a concrete grid instance
   * @note there is a variant of this function, using just a
   *       symbolic name to refer to the grid, allowing to nudge
   *       based on a grid known to exist somewhere in the session.
   *       Using this approach involves the Advice system and thus
   *       requires linking against \c liblumieracommon.so
   * @see #nudge(int,Symbol)
   */
  EncapsulatedMutation
  Mutation::nudge (int adjustment, PQuant const& grid)
  {
    return EncapsulatedMutation::build<NudgeMutation> (adjustment, grid);
  }
  
  
  
  
}} // lib::time

