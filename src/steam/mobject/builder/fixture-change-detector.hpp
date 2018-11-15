/*
    FIXTURE-CHANGE-DETECTOR.hpp  -  isolating changed segments and tainted processes

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file fixture-change-detector.hpp
 ** Work out the part of the Fixture changed by a build process.
 ** This facility helps to deal with ongoing render/playback processes, which might be
 ** affected by the results of a build process. It's comprised of two distinct parts:
 ** - a comparison tool allowing to spot equal and changed segments when considering
 **   the old and the new version of the fixture before/after a build process.
 ** - a registration service to establish a relation between play/render processes
 **   and specific segments of the fixture.
 ** Together, these allow to identify those ongoing processes which need to be cancelled
 ** or restarted, because their results might be tainted by the changes induced by the
 ** build process. Typically, these detection process runs just before committing the
 ** newly built fixture datastructure.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2010
 ** 
 ** @see Fixture
 ** @see ModelPort
 */


#ifndef STEAM_MOBJECT_BUILDER_FIXTURE_CHANGE_DETECTOR_H
#define STEAM_MOBJECT_BUILDER_FIXTURE_CHANGE_DETECTOR_H

#include "lib/error.hpp"
//#include "lib/optional-ref.hpp"
#include "steam/asset/pipe.hpp"
//#include "steam/asset/struct.hpp"
//#include "steam/mobject/model-port.hpp"

//#include <map>

namespace steam    {
namespace mobject {
namespace builder {
  
  using asset::ID;
  using asset::Pipe;
//using asset::Struct;
  
//LUMIERA_ERROR_DECLARE (DUPLICATE_MODEL_PORT); ///< Attempt to define a new model port with an pipe-ID already denoting an existing port
  
  
  /**
   * TODO type comment
   */
  class FixtureChangeDetector
    : util::NonCopyable
    {
      
      typedef ID<Pipe>   PID;
//      typedef ID<Struct> StID;
      
    public:
      
    };
  
  
  
}}} // namespace steam::mobject::builder
#endif
