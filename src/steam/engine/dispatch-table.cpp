/*
  DispatchTable  -  implementation of frame job creation

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


/** @file dispatch-table.cpp
 ** Implementation details of render job generation.
 ** @todo draft from 2011, stalled, relevance not yet clear
 */


#include "steam/engine/dispatch-table.hpp"
//#include "lib/frameid.hpp"
//#include "steam/state.hpp"



namespace steam {
namespace engine {
  
  /** */
  FrameCoord
  DispatchTable::locateRelative (FrameCoord const&, FrameCnt frameOffset)
  {
    UNIMPLEMENTED ("real implementation of the core dispatch operation");
  }
  
  bool
  DispatchTable::isEndOfChunk (FrameCnt, ModelPort port)
  {
    UNIMPLEMENTED ("determine when to finish a planning chunk");
  }
  
  JobTicket&
  DispatchTable::accessJobTicket (size_t, TimeValue nominalTime)
  {
    UNIMPLEMENTED ("hook into the real implementation of the model backbone / segmentation");
  }
  
  size_t
  DispatchTable::resolveModelPort (ModelPort modelPort)
  {
    UNIMPLEMENTED ("some Map lookup in a prepared table to find out the actual slot number");
  }

  
  
  
  
}} // namespace engine
