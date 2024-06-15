/*
  TrackingAllocator  -  test dummy objects for tracking ctor/dtor calls

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

* *****************************************************/


/** @file tracking-allocator.cpp
 ** Implementation of the common storage backend for the tracking test allocator.
 ** 
 ** @see tracking-allocator.hpp
 ** @see TestTracking_test#demonstrate_checkAllocator()
 ** 
 */


//#include "lib/test/test-helper.hpp"
#include "lib/test/tracking-allocator.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"
//#include "lib/unique-malloc-owner.hpp"
#include "lib/depend.hpp"
#include "lib/uninitialised-storage.hpp"


//#include <string>

//using util::_Fmt;
//using std::string;

namespace lib {
namespace test{
  
  namespace { // implementation details for common memory management
    
    struct Allocation
      {
        UninitialisedDynBlock<byte> buff;
      };
    
    /**
     * @internal registration and tracking of memory allocations handed out. 
     */
    class MemoryPool
      {
        
      };
    
    Depend<MemoryPool> globalPool;
  }


  TrackingAllocator::TrackingAllocator()
    : mem_{}
    { }
  
  TrackingAllocator::TrackingAllocator (Literal id)
    : mem_{}
    { 
      UNIMPLEMENTED ("attach to specific pool");
    }
  
  
  void*
  TrackingAllocator::allocate (size_t n)
  {
    UNIMPLEMENTED ("allocate memory block of size n");
  }
  
  void
  TrackingAllocator::deallocate (void* loc) noexcept
  {
    UNIMPLEMENTED ("allocate memory block of size n");
  }
  
  
  
  /* ===== Diagnostics ===== */
  
  EventLog TrackingAllocator::log{"test::TrackingAllocator"};      
  
  
  HashVal
  TrackingAllocator::checksum (Literal pool)
  {
    UNIMPLEMENTED ("get Checksum for mem-pool");
  }
  
  size_t
  TrackingAllocator::numAlloc  (Literal pool)
  {
    UNIMPLEMENTED ("get allocation count for mem-pool");
  }
  
  size_t
  TrackingAllocator::numBytes  (Literal pool)
  {
    UNIMPLEMENTED ("calc allotted Bytes for mem-pool");
  }
  

  
  
}} // namespace lib::test
