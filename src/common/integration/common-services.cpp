/*
  CommonServices  -  integrate some library facilities with common system services

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file common-serivces.cpp 
 ** Wire library facilities directly into application core services 
 ** This translation unit serves to complete the definition of some parts of the Lumiera library.
 ** While library facilities usually are written to be self-contained, at places we want "magic"
 ** integration with central services, without incurring tight coupling to the application core.
 ** In these special cases, the library just \em declares some function or constructor variant,
 ** which is then \em defined here, causing the corresponding code to be emitted into the
 ** Lumiera application library. Obviously, to use such integrated "magic" short-cuts, client code
 ** needs to link against \c liblumieracore.so
 ** 
 ** The typical usage pattern is to allow for references by name-ID, which is then magically
 ** resolved behind the scenes, using the Advice system or more generic query facilities.
 ** This pattern allows even to tap into facilities known to exist within some very specific
 ** subsystem, like e.g. some session service. A typical example is Lumiera's time and time quantisation
 ** framework, which allows a \link QuTime grid aligned time \endlink to refer to a frame grid definition
 ** known to be provided by one of the session's timelines.
 ** 
 ** @see advice.hpp
 ** @see timequant.hpp
 ** @see quantiser.hpp
 **
 */


#include "lib/error.hpp"
#include "lib/format-string.hpp"

namespace error = lumiera::error;



/* =========== Advice System integrations ================== */

#include "lib/time/timequant.hpp"
#include "lib/time/quantiser.hpp"
#include "common/advice.hpp"


namespace lib {
namespace time {
  
  namespace { // implementation helper to access the Advice system...
    
    using util::_Fmt;
    
    namespace advice = lumiera::advice;
    
    
    PQuant
    retrieveQuantiser (Symbol gridID)
    {
      advice::Request<PQuant> query(gridID);
      PQuant grid_found = query.getAdvice();
      if (!grid_found)
        throw error::Logic (_Fmt("unable to resolve the time quantisation grid with ID=\"%s\" -- was it already defined?")
                                % gridID
                           , LUMIERA_ERROR_UNKNOWN_GRID);
      return grid_found;
    }
  }//(End) implementation helpers
  
  
  /** 
   * build a quantised time value, referring the time grid by-name.
   * This is the preferred standard way of establishing a quantisation,
   * but it requires an existing time scale defined in the Lumiera Session,
   * as TimeGrid (meta asset). Usually, such a time scale gets built based
   * on the format and parameters of an output bus.
   */
  QuTime::QuTime (TimeValue raw, Symbol gridID)
    : Time(raw)
    , quantiser_(retrieveQuantiser (gridID))
    { }
  
  
  /** Access an existing grid definition or quantiser, known by the given symbolic ID.
   *  Typically this fetches a meta::TimeGrid (asset) from the session.
   * @throw error::Logic if the given gridID wasn't registered
   * @return smart-ptr to the quantiser instance */
  PQuant
  Quantiser::retrieve (Symbol gridID)
  {
    return retrieveQuantiser (gridID);
  }
  
  
}} // namespace lib::time
