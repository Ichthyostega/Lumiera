/*
  EXTENT-FAMILY.hpp  -  maintain a sequence of memory extents used cyclically

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


/** @file extent-family.hpp
 ** Memory management scheme for cyclically used memory extents.
 ** In this context, _Extent_ denotes a larger block of memory to hold a
 ** cluster of smaller records, which are closely interrelated and will be managed
 ** and de-allocated together. The typical usage involves a constant demand for further
 ** memory, with is satisfied by putting further unused extents into use; older extents
 ** will fall out of use eventually, and can then be placed back into a buffer of free
 ** extents. In accordance to overall demand, this reserve buffer can be scaled up
 ** and down to avoid holding larger amounts of unused memory, while the availability
 ** of a baseline amount of memory can be enforced. 
 ** 
 ** @see ////TODO_test usage example
 ** @see gear::BlockFlow usage example
 ** 
 ** @todo WIP-WIP-WIP 7/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_MEM_EXTENT_FAMILY_H_
#define SRC_VAULT_MEM_EXTENT_FAMILY_H_


#include "vault/common.hpp"
//#include "lib/symbol.hpp"
#include  "lib/nocopy.hpp"
//#include "lib/util.hpp"

//#include <string>
#include <memory>
#include <vector>
#include <array>


namespace vault{
namespace mem {
  
//  using util::isnil;
//  using std::string;
  
  
  /**
   * Memory manager to provide a sequence of Extents for cyclic usage.
   * @todo WIP-WIP 7/2023
   * @see NA_test
   */
  template<typename T, size_t siz>
  class ExtentFamily
    : util::NonCopyable
    {
      using Storage = std::array<T,siz>;
      
      struct Extent
        : std::unique_ptr<Storage>
        {
          /**
           * @note default ctor immediately allocates the full storage,
           *       but uses default initialisation rsp. no initialisation
           *       in case the payload type T is a POD
           */
          Extent()
            : std::unique_ptr<Storage>{new Storage}
            { }
        };
      using Extents = std::vector<Extent>;
      
      Extents extents_;
      
      size_t start_,after_;
      
    public:
      explicit
      ExtentFamily(size_t initialCnt =0)
        : extents_{initialCnt}
        , start_{0}
        , after_{initialCnt}
        { }
      
      void
      reserve (size_t expectedMaxExtents)
        {
          extents_.reserve (expectedMaxExtents);
        }
    };
  
  
  
}} // namespace vault::mem
#endif /*SRC_VAULT_MEM_EXTENT_FAMILY_H_*/
