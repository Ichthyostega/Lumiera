/*
  CONTROL.hpp  -  a life time control for feedback and mutation  

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


/** @file control.hpp
 ** Manipulating and monitoring time entities with life changes.
 ** This is an control- and callback element to handle any kind of "running"
 ** time entities. This element is to be provided by the client and then attached
 ** to the target time entity as a time::Mutation. Internally, a life connection to
 ** the target is built, allowing both to 
 ** - to manipulate the target by invoking the function operator
 ** - to receive change notifications by installing a callback functor.
 ** 
 ** The actual type of the changes and modifications is specified as template parameter;
 ** when later attached to some time entity as a Mutation, the actual changes to be performed
 ** depend both on this change type and the type of the target time entity (double dispatch).
 ** The behaviour is similar to applying a static time::Mutation
 ** 
 ** \par relevance
 ** This control element is intended to be used for all kinds of editing and monitoring
 ** of time-like entities -- be it the running time display in a GUI widget, a ruler marker
 ** which can be dragged, a modifiable selection or the animated playhead cursor.
 ** 
 ** @todo WIP-WIP-WIP
 **
 */

#ifndef LIB_TIME_CONTROL_H
#define LIB_TIME_CONTROL_H

#include "lib/error.hpp"
#include "lib/time/mutation.hpp"
//#include "lib/symbol.hpp"

//#include <boost/noncopyable.hpp>
//#include <iostream>
//#include <boost/operators.hpp>
#include <tr1/functional>
//#include <string>


namespace lib {
namespace time {
  
//using lib::Symbol;
//using std::string;
//using lib::Literal;
  using std::tr1::function;
  
//LUMIERA_ERROR_DECLARE (INVALID_MUTATION); ///< Changing a time value in this way was not designated
  
  typedef function<void(TI)> timeSignal;
  
  /**
   * Interface: controller-element for retrieving and
   * changing running time values
   * 
   * @see time::Mutation
   * @see time::TimeSpan#accept(Mutation const&)
   * @todo WIP-WIP-WIP
   */
  template<class TI>
  class Control
    : Mutation
    , timeSignal
    {
    public:
      /** install a callback functor to be invoked
       *  to notify for any changes to the observed
       *  time entity */
      void connectChangeNotification (timeSignal);
      
      /** disconnect from observed entity and
       *  cease any change notification */
      void disconnnect();
      
    };
  
  
  
  
}} // lib::time
#endif
