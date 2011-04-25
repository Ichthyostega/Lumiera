/*
  MUTATION.hpp  -  changing and adjusting time values

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

*/


/** @file mutation.hpp
 ** Modifying time and timecode values.
 ** Basically, time values are designed to be immutable. Under some circumstances
 ** we need a means to change existing time values though. Moreover, actually performing
 ** these changes gets quite involved at the implementation level, due to time / timecode
 ** values being \em quantised (grid aligned). To keep this implementation logic separate
 ** from the simple time values, and to allow for later extensions and more elaborate
 ** change schemes, we choose the <b>visitor pattern</b> for implementing these
 ** time mutating operations.
 ** 
 ** \par usage situations
 ** 
 ** There are four basic usage situations causing an existing time value to be mutated:
 ** 
 ** - dragging an object or object boundary (resizing)
 ** - nudging
 ** - placing an object (e.g. a clip)
 ** - update of a timecode display in the GUI
 ** 
 ** All these include sending a change message to the original time spec, while the
 ** actual content of the change message determines the concrete behaviour. Especially,
 ** when the target time to be changed or the message content are quantised, an interference
 ** between multiple time grids may be happen, possibly resulting in a re-quantisation of
 ** the message into the target object's own time grid.
 ** 
 ** @todo WIP-WIP-WIP
 **
 */

#ifndef LIB_TIME_MUTATION_H
#define LIB_TIME_MUTATION_H

#include "lib/error.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/polymorphic-value.hpp"
//#include "lib/symbol.hpp"

#include <boost/noncopyable.hpp>
//#include <iostream>
//#include <boost/operators.hpp>
//#include <string>


namespace lib {
namespace time {
  
//using std::string;
//using lib::Literal;
  
  LUMIERA_ERROR_DECLARE (INVALID_MUTATION); ///< Changing a time value in this way was not designated
  
  class QuTime;
  class Mutation;
  
  
  /* The following typedefs allow to hand out
   * "unspecific" mutation value objects from 
   * factory functions, without disclosing any
   * implementation details here in this header.
   */
  enum{ MUTATION_IMPL_SIZE = sizeof(TimeValue) };
  
  typedef lib::polyvalue::CloneValueSupport<Mutation> ClonableMutation;
  typedef lib::PolymorphicValue<Mutation, MUTATION_IMPL_SIZE, ClonableMutation> EncapsulatedMutation;
  
  /**
   * Interface: an opaque change imposed onto some time value.
   * 
   * @see time::TimeSpan#accept(Mutation const&)
   * @todo WIP-WIP-WIP
   */
  class Mutation
    {
    public:
      virtual ~Mutation();
      
      virtual void change (Duration&)  const  =0;
      virtual void change (TimeSpan&)  const  =0;
      virtual void change (QuTime&)    const  =0;
      
      /* === convenience shortcuts for simple cases === */
      
      static EncapsulatedMutation changeTime (Time);
      static EncapsulatedMutation changeDuration (Duration);
      static EncapsulatedMutation adjust (Offset);
      static EncapsulatedMutation nudge (int adjustment);
      
    protected:
      static void imposeChange (TimeValue&, TimeValue const&);
    };
  
  
  /* === defining the visitation responses === */
    
  inline void Duration::accept (Mutation const& muta) { muta.change (*this); }
  inline void TimeSpan::accept (Mutation const& muta) { muta.change (*this); }
  
#ifdef LIB_TIME_TIMEQUQNT_H
  inline void QuTime::accept   (Mutation const& muta) { muta.change (*this); }
#endif
  
  
}} // lib::time
#endif
