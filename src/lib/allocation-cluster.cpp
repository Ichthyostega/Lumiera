/*
  AllocationCluster  -  allocating and owning a pile of objects

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file allocation-cluster.cpp
 ** Implementation of [memory management helper functions](\ref allocation-cluster.hpp)
 ** for the render engine model. Here, in the actual translation unit, the generic part
 ** of these functions is emitted, while the corresponding header provides a strictly
 ** typed front-end, based on templates, which forward to the implementation eventually.
 */


#include "lib/allocation-cluster.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"


using util::isnil;


namespace lib {
  
  
  
  /** creating a new AllocationCluster prepares a table capable
   *  of holding the individual object families to come. Each of those
   *  is managed by a separate instance of the low-level memory manager.
   */
  AllocationCluster::AllocationCluster()
  {
    TRACE (memory, "new AllocationCluster");
  }
  
  
  /** On shutdown of the AllocationCluster we need to assure a certain
   *  destruction order is maintained by explicitly invoking a cleanup
   *  operation on each of the low-level memory manager objects. 
   */
  AllocationCluster::~AllocationCluster()  noexcept
  {
    try
      {
        
        TRACE (memory, "shutting down AllocationCluster");
        
      }
    ERROR_LOG_AND_IGNORE (progress, "discarding AllocationCluster")
  }
  
  
  
  /* === diagnostics helpers === */
  
  
  
} // namespace lib
