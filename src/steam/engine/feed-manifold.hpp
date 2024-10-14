/*
  FEED-MANIFOLD.hpp  -  data feed connection system for render nodes

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file feed-manifold.hpp
 ** @todo staled since 2009, picked up in 2024 in an attempt to finish the node invocation.
 ** @todo WIP-WIP 2024 rename and re-interpret as a connection system
 ** @see nodeinvocation.hpp
 */


#ifndef ENGINE_FEED_MANIFOLD_H
#define ENGINE_FEED_MANIFOLD_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
//#include "steam/engine/proc-node.hpp"
#include "steam/engine/buffhandle.hpp"
#include "lib/uninitialised-storage.hpp"
//#include "lib/several.hpp"

//#include <utility>
//#include <array>


////////////////////////////////TICKET   #826  will be reworked alltogether

namespace steam {
namespace engine {
  
//  using std::pair;
//  using std::vector;
  
  
    /**
     * Obsolete, to be rewritten  /////TICKET #826
     *  
     * Tables of buffer handles and corresponding dereferenced buffer pointers.
     * Used within the invocation of a processing node to calculate data.
     * The tables are further differentiated into input data buffers and output
     * data buffers. The tables are supposed to be implemented as bare "C" arrays,
     * thus the array of real buffer pointers can be fed directly to the
     * processing function of the respective node.
     * 
     * @todo WIP-WIP-WIP 7/24 now reworking the old design in the light of actual render engine requirements...
     */
  template<uint N>
  struct FeedManifold
    : util::NonCopyable
    {
      using BuffS = lib::UninitialisedStorage<BuffHandle,N>;
      enum{ STORAGE_SIZ = N };
      
      BuffS inBuff;
      BuffS outBuff;
    };
  
  
  
  
}} // namespace steam::engine
#endif /*ENGINE_FEED_MANIFOLD_H*/
