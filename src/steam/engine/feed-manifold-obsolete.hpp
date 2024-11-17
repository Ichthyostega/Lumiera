/*
  FEED-MANIFOLD.hpp  -  data feed connection system for render nodes

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file feed-manifold-obsolete.hpp
 ** @todo staled since 2009, picked up in 2024 in an attempt to finish the node invocation.
 ** deprecated 2024 this file is a dead-end! It is retained in tree to keep other obsolete code buildable
 ** @see nodeinvocation.hpp
 */


#ifndef ENGINE_FEED_MANIFOLD_OBSOLETE_H
#define ENGINE_FEED_MANIFOLD_OBSOLETE_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "steam/engine/channel-descriptor.hpp"
//#include "steam/engine/proc-node.hpp"    ///////////////////////////////TODO this is a dead end
#include "steam/engine/connectivity-obsolete.hpp"

#include <vector>
#include <utility>


////////////////////////////////TICKET   #826  will be reworked alltogether

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
  struct FeedManifold                 ///////////////////////////////////OOO rename into FeedManifold
    {
      typedef BuffHandle        * PHa;
      typedef BuffHandle::PBuff * PBu;
      typedef pair<PHa const,PBu const> Chunk;
      
      PHa outHandle;
      PHa inHandle;
      PBu outBuff;
      PBu inBuff;
    };
  
  class BuffDescr;
  
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
          static BuffDescr const& 
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
      FeedManifold::Chunk
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
      level_check (FeedManifold::Chunk& prev_level)
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
    : public FeedManifold,
      util::NonCopyable
    {
      const uint siz_;
      FeedManifold::Chunk tab_;
      BuffTableStorage& sto_;
      
    public:
      BuffTableChunk (Connectivity const& wd, BuffTableStorage& storage)
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
#endif /*ENGINE_FEED_MANIFOLD_OBSOLETE_H*/
