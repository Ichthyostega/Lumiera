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
 ** @see BlockFlow_test
 ** @see SchedulerUsage_test
 ** @see extent-family.hpp underlying allocation scheme
 ** 
 ** @todo WIP-WIP-WIP 6/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_BLOCK_FLOW_H_
#define SRC_VAULT_GEAR_BLOCK_FLOW_H_


#include "vault/common.hpp"
#include "vault/gear/activity.hpp"
#include "vault/mem/extent-family.hpp"
//#include "lib/symbol.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/nocopy.hpp"
//#include "lib/util.hpp"

//#include <string>
#include <utility>


namespace vault{
namespace gear {
  
//  using util::isnil;
//  using std::string;
  using lib::time::Time;
  
  namespace {// hard-wired parametrisation
    const size_t EPOCH_SIZ = 100;
    const size_t ACTIVITIES_PER_FRAME = 10;
    const size_t INITIAL_FRAMES = 50;
    const size_t INITIAL_ALLOC  = 1 + (INITIAL_FRAMES * ACTIVITIES_PER_FRAME) / EPOCH_SIZ;
    
    /** raw allocator to provide a sequence of Extents to place Activity records */
    using Allocator = mem::ExtentFamily<Activity, EPOCH_SIZ>;
  }
  
  
  
  /**
   * Allocation Extent holding _scheduler Activities_ to be performed altogether
   * before a common _deadline._ Other than the underlying raw Extent, the Epoch
   * maintains a deadline time and keeps track of storage slots already claimed.
   * This is achieved by using the Activity record in the first slot as a GATE term
   * to maintain those administrative information.
   * @remark rationale is to discard the Extent as a whole, once the deadline passed.
   */
  class Epoch
    : public Allocator::Extent
    {
      
      /// @warning will be faked, not constructed
      Epoch()    = delete;
      
    public:
      /**
       * specifically rigged GATE Activity,
       * used for managing Epoch metadata
       * - the Condition::rest tracks pending async IO operations
       * - the Condition::deadline is the nominal deadline of this Epoch
       * - the field `next` points to the next free allocation Slot to use
       */
      struct EpochGate
        : Activity
        {
          /** @note initially by default there is...
           *      - effectively no deadline
           *      - no IO operations pending (i.e. we can just discard the Epoch)
           *      - the `next` usable Slot is the last Storage slot, and will be
           *        decremented until there is only one slot left (EpochGate itself)
           *  @warning EpochGate is assumed to sit in the Epoch's first slot
           */
          EpochGate()
            : Activity{int(0), Time::ANYTIME}
            {
              // initialise allocation usage marker: start at last usable slot
              next = this + (Epoch::SIZ() - 1);
              ENSURE (next != this);
            }
          // default copyable
          
          bool
          hasFreeSlot()
            { // see C++ § 5.9 : comparison of pointers within same array
              return next > this;
            }
          
          Activity*
          claimNextSlot()
            {
              REQUIRE (hasFreeSlot());
              return next--;
            }
        };
      
      static Epoch&
      implantInto (Allocator::Extent& rawStorage)
        {
          Epoch& target = static_cast<Epoch&> (rawStorage);
          new(&target[0]) EpochGate{};
          return target;
        }
      
      EpochGate&
      gate()
        {
          return static_cast<EpochGate&> ((*this)[0]);
        }
    };
  
  
  
  /**
   * Allocation scheme for the Scheduler, based on Epoch(s).
   * Scheduling entails to provide a chain of Activity definitions,
   * which will then »flow« through the priority queue until invocation.
   * 
   * @see SchedulerCommutator
   * @see BlockFlow_test
   */
  class BlockFlow
    : util::NonCopyable
    {
      Allocator alloc_;
      
    public:
      BlockFlow()
        : alloc_{INITIAL_ALLOC}
        { }
      
      /**
       * Local handle to allow allocating a collection of Activities,
       * all sharing a common deadline. Internally, these records are
       * maintained in fixed-sized _extents_ and thus allocations may
       * _overflow_ — leading to allocation of further extents. However,
       * this extension is handled transparently by the embedded iterator.
       */
      class AllocatorHandle
        {
          Allocator::iterator extent;
          
        public:
          template<typename...ARGS>
          Activity&
          create (ARGS&& ...args)
            {
              return *new(claimSlot()) Activity {std::forward<ARGS> (args)...};
            }
          
        private:
          Epoch&
          currEpoch()
            {
              REQUIRE (bool(extent));
              return static_cast<Epoch&> (*extent);
            }
          
          void*
          claimSlot() ///< EX_SANE
            {
              if (currEpoch().gate().hasFreeSlot())
                {
                  return currEpoch().gate().claimNextSlot();
                }
              else // Epoch overflow
                { //  use following Epoch; possibly allocate
                  UNIMPLEMENTED ("advance to next epoch");
                }
            }
        };
      
      AllocatorHandle
      until (Time deadline)
        {
          UNIMPLEMENTED ("search through existing Epochs to locate the latest one to support given deadline");
        }
      
      void
      discardBefore (Time deadline)
        {
          UNIMPLEMENTED ("traverse oldest Epochs and discard obsoleted");
        }
    };
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_BLOCK_FLOW_H_*/
