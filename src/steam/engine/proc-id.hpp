/*
  PROC-ID.hpp  -  symbolic and hash identification of processing steps

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file proc-id.hpp
 ** Metadata interface to generate symbolic and hash ID tags for media processing steps.
 ** Functionality is provided to identify a point in the processing chain for sake of
 ** error reporting and unit testing; moreover, identifying information can be chained
 ** and combined into a systematic hash key, to serve as foundation for a stable cache key.
 ** @todo WIP-WIP as of 10/2024 this is a draft created as an aside while working towards
 **       the first integration of render engine functionality //////////////////////////////////////////////TICKET #1377 : establish a systematic processing identification
 ** @remark the essential requirement for a systematic and stable cache key is
 **       - to be be re-generated directly from the render node network
 **       - to be differing if and only if the underlying processing structure changes
 ** 
 ** @see turnout.hpp
 ** @see engine::ProcNodeDiagnostic
 */

#ifndef ENGINE_PROC_ID_H
#define ENGINE_PROC_ID_H


#include "lib/error.hpp"
#include "lib/hash-value.h"
//#include "steam/streamtype.hpp"


namespace steam {
namespace engine {
  namespace err = lumiera::error;
  
  using lib::HashVal;
  
  class ProcID
    {
    public:
      /** build and register a processing ID descriptor */
      static ProcID& describe();
    };
  
  
}} // namespace steam::engine
#endif /*ENGINE_PROC_ID_H*/
