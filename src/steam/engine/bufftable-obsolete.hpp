/*
  BUFFTABLE-OBSOLTE.hpp  -  Old dead code to be removed when rewriting ProcNode!!!!!

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

*/


/** @file bufftable-obsolete.hpp
 ** @deprecated obsolete since 2009, left in tree to keep some likewise unfinished code alive.
 ** @see nodeinvocation.hpp
 */


#ifndef ENGINE_BUFFHTABLE_OBSOLETE_H
#define ENGINE_BUFFHTABLE_OBSOLETE_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "steam/engine/channel-descriptor.hpp"
#include "steam/engine/procnode.hpp"

#include <vector>
#include <utility>


////////////////////////////////WARNING: obsolete code
////////////////////////////////WARNING: ...just left in tree to keep it compiling
////////////////////////////////TICKET   #826  need to be reworked entirely

namespace steam {
namespace engine {
  
  using std::pair;
  using std::vector;
  
  
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
     * @todo this whole design is a first attempt and rather clumsy. It should be reworked
     *       to use a single contiguous memory area and just layer the object structure on top
     *       (by using placement new). Yet the idea of an stack-like organisation should be retained
     */
  struct BuffTable
    {
      typedef BuffHandle        * PHa;
      typedef BuffHandle::PBuff * PBu;
      typedef pair<PHa const,PBu const> Chunk;
      
      PHa outHandle;
      PHa inHandle;
      PBu outBuff;
      PBu inBuff;
    };
  
  class BufferDescriptor;
  
    /** Obsolete, to be rewritten  /////TICKET #826 */
  class BuffTableStorage
    {
      /////////////////////////////////////////////////////////////////////////TICKET #826  need to be reworked entirely
      /** just a placeholder to decouple the existing code
       *  from the reworked BuffHandle logic. The existing
       *  code in turn will be reworked rather fundamentally
       */
      struct BuffHaXXXX
        : BuffHandle
        {
          BuffHaXXXX() : BuffHandle(just_satisfy_the_compiler()) { /* wont work ever */ }
          static BufferDescriptor const& 
          just_satisfy_the_compiler() { }
        };
        
                                           ////////////////////////////////////TICKET #825  should be backed by mpool and integrated with node invocation
      vector<BuffHaXXXX>        hTab_;
      vector<BuffHandle::PBuff> pTab_;
      size_t level_;
      
    public:
      BuffTableStorage (const size_t maxSiz)
        : hTab_(maxSiz),
          pTab_(maxSiz),
          level_(0)
        { }
      
      ~BuffTableStorage() { ASSERT (0==level_, "buffer management logic broken."); }
      
    protected:
      
      friend class BuffTableChunk;
      
      /** allocate the given number of slots
       *  starting at current level to be used
       *  by the newly created BuffTableChunk
       */
      BuffTable::Chunk
      claim (uint slots)
        {
          ASSERT (pTab_.size() == hTab_.size());
          REQUIRE (level_+slots <= hTab_.size());
          
          size_t prev_level (level_);
          level_ += slots;
          return std::make_pair (&hTab_[prev_level],
                                 &pTab_[prev_level]);
        }
      
      void
      release (uint slots)
        {
          ASSERT (slots <= level_);
          REQUIRE (level_ <= hTab_.size());
          REQUIRE (level_ <= pTab_.size());
          
          level_ -= slots;
        }
      
      bool
      level_check (BuffTable::Chunk& prev_level)
        {
          return prev_level.first  == &hTab_[level_]
              && prev_level.second == &pTab_[level_];
        }
    };
  
  
  /** Obsolete, to be rewritten  /////TICKET #826 
   * to be allocated on the stack while evaluating a ProcNode#pull() call.
   * The "current" State (StateProxy) maintains a BuffTableStorage (=pool),
   * which can be used to crate such chunks. The claiming and releasing of
   * slots in the BuffTableStorage is automatically tied to BuffTableChunk
   * object's lifecycle.
   */
  class BuffTableChunk
    : public BuffTable,
      util::NonCopyable
    {
      const uint siz_;
      BuffTable::Chunk tab_;
      BuffTableStorage& sto_;
      
    public:
      BuffTableChunk (WiringDescriptor const& wd, BuffTableStorage& storage)
        : siz_(wd.nrI + wd.nrO),
          tab_(storage.claim (siz_)),
          sto_(storage)
        {
          const uint nrO(wd.nrO);
          
          // Setup the publicly visible table locations
          this->outHandle = &tab_.first[ 0 ];
          this->inHandle  = &tab_.first[nrO];
          this->outBuff   = &tab_.second[ 0 ];
          this->inBuff    = &tab_.second[nrO];
        }
      
      ~BuffTableChunk ()
        {
          sto_.release (siz_);
          ASSERT ( sto_.level_check (tab_),
                  "buffer management logic broken.");
        }
    };
  
  
  
  
  
}} // namespace steam::engine
#endif
