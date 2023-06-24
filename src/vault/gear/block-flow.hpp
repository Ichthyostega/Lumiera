/*
  BLOCK-FLOW.hpp  -  specialised custom allocator to manage scheduler data

  Copyright (C)         Lumiera.org
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


/** @file block-flow.hpp
 ** Memory management scheme for activities and parameter data passed through
 ** the Scheduler within the Lumiera render engine. While — conceptually — the
 ** intended render operations are described as connected activity terms, sent
 ** as messages through the scheduler, the actual implementation requires a fixed
 ** descriptor record sitting at a stable memory location while the computation
 ** is underway. Moreover, activities can spawn further activities, implying that
 ** activity descriptor records can emanate from multiple threads concurrently,
 ** and the duration to keep those descriptors in valid state is contingent.
 ** On the other hand, ongoing rendering produces a constant flow of further
 ** activities, necessitating timely clean-up of obsolete descriptors.
 ** Used memory should be recycled, calling for an arrangement of
 ** pooled allocation tiles, extending the underlying block
 ** allocation on increased throughput.  
 ** 
 ** @note currently this rather marks the intended memory management pattern,
 **       while the actual allocations are still performed on the heap.
 ** @see ////TODO_test usage example
 ** @see scheduler.cpp implementation
 ** 
 ** @todo WIP-WIP-WIP 6/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_BLOCK_FLOW_H_
#define SRC_VAULT_GEAR_BLOCK_FLOW_H_


#include "vault/common.hpp"
//#include "lib/symbol.hpp"
#include "lib/nocopy.hpp"
//#include "lib/util.hpp"

//#include <string>


namespace vault{
namespace gear {
  
//  using util::isnil;
//  using std::string;
  
  
  /**
   * Basic (abstracted) view of...
   * 
   * @see SomeSystem
   * @see NA_test
   */
  class BlockFlow
    : util::NonCopyable
    {
      
    public:
      explicit
      BlockFlow (int woof)
        { }
    };
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_BLOCK_FLOW_H_*/
