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
#include "lib/time/mutation.hpp"
//#include "lib/time.h"
//#include "lib/util.hpp"

//#include <tr1/functional>

//using std::string;
//using util::unConst;
//using util::floorwrap;

namespace error = lumiera::error;

namespace lib {
namespace time {
  
  
  LUMIERA_ERROR_DEFINE (INVALID_MUTATION, "Changing a time value in this way was not designated");
  
  
  Mutation::~Mutation()   { }  // emit VTable here....
  
  
  /** @internal actually force a change
   *  into a target time entity to mutate.
   *  Mutation is declared fried to TimeValue
   *  and thus is allowed to influence the basic
   *  value stored in each time entity
   */
  void
  Mutation::imposeChange (TimeValue& target, TimeValue const& valueToSet)
  {
    target = valueToSet; 
  }
  
  
  
  
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
          imposeChange (target, TimeVar(target)+=adjustment_);
        }
      
      
      virtual void
      change (TimeSpan& target) const
        {
          imposeChange (target, TimeVar(target)+=adjustment_);
        }
      
      
      /** @note the re-quantisation happens automatically
       *  when the (changed) QuTime is materialised */
      virtual void
      change (QuTime& target)   const
        {
          imposeChange (target, TimeVar(target)+=adjustment_);
        }
      
      
    public:
      explicit
      ImposeOffsetMutation (Offset adj)
        : adjustment_(adj)
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
  
  
  EncapsulatedMutation
  Mutation::nudge (int adjustment)
  {
    UNIMPLEMENTED ("Nudge by a predefined nudge amount");
  }
  
  
  
  
}} // lib::time
  
