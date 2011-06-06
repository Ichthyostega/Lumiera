/*
  Mutation  -  changing and adjusting time values

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


#include "lib/error.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/timequant.hpp"
#include "lib/time/timecode.hpp"
#include "lib/time/mutation.hpp"
//#include "lib/time.h"
#include "lib/util.hpp"

//#include <tr1/functional>

//using std::string;
//using util::unConst;
//using util::floorwrap;
using util::isnil;

namespace error = lumiera::error;

namespace lib {
namespace time {
  
  
  LUMIERA_ERROR_DEFINE (INVALID_MUTATION, "Changing a time value in this way was not designated");
  
  
  Mutation::~Mutation()   { }  // emit VTable here....
  
  
  
  
  
  
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
      materialiseGridPoint (Symbol gridID, int steps)
        {
          REQUIRE (!isnil (gridID));
          PQuant grid = Quantiser::retrieve(gridID);
          return Offset(grid->timeOf(0), grid->timeOf(steps));
        }
      
    public:
      NudgeMutation (int relativeSteps, Symbol gridID)
        : ImposeOffsetMutation(materialiseGridPoint (gridID,relativeSteps))
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
   * @param gridID symbolic name used to register or define a
   *        suitable nudge grid, typically somewhere globally
   *        in the session (as meta asset)
   */
  EncapsulatedMutation
  Mutation::nudge (int adjustment, Symbol gridID)
  {
    return EncapsulatedMutation::build<NudgeMutation> (adjustment, gridID);
  }
  
  
  
  
}} // lib::time
  
