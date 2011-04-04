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
  Mutation::imposeChange (TimeValue& target, TimeValue valueToSet)
  {
    target = valueToSet; 
  }
  
  
  
  
  /** 
   * concrete time value mutation:
   * impose fixed new start time.
   */
  class SetNewStartTime
    : public Mutation
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
      SetNewStartTime (TimeValue t)
        : newTime_(t)
        { }
    };
  
  
  
  /** */
  void
  Mutation::changeTime (Time newStartTime)
  {
    UNIMPLEMENTED ("perform a trivial mutation to set a new fixed time value");
  }
  
  
  void
  Mutation::changeDuration (Duration)
  {
    UNIMPLEMENTED ("perform a trivial mutation to set a new Duration");
  }
  
  
  void
  Mutation::nudge (int adjustment)
  {
    UNIMPLEMENTED ("Nudge by a predefined nudge amount");
  }
  
  
  
  
}} // lib::time
  
