/*
  LIST-DIFF-DETECTOR.hpp  -  language to describe differences in linearised form

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file list-diff-detector.hpp
 ** Compare two data sequences to find or describe differences.
 ** The DiffDetector defined here takes snapshot(s) from a monitored generic
 ** data structure and generates a description of differences in a linearised
 ** list diff language. Once initiated, the investigation of the old and new
 ** sequence snapshot, combined with generation of a sequence of diff description
 ** verbs, proceeds demand driven. The client "takes" a DiffFrame, which acts
 ** as iterator to extract the diff progressively; when initiating such a
 ** diff generation process, a new baseline snapshot from the underlying
 ** data is taken to replace the old baseline.
 ** 
 ** \par List Diff Algorithm
 ** A fundamental decision taken here is to process the differences in a stream
 ** processing fashion. This renders the usage of data index numbers undesirable.
 ** Moreover, we do not want to assume anything about the consumer; the diff might
 ** be transformed into a textual representation, or it may be applied to quite
 ** another target data structure.
 ** 
 ** The implementation is built using a simplistic method and is certainly far from
 ** optimal. For one, we're taking snapshots, and we're building an index table
 ** for each snapshot, in order to distinguish inserted and deleted elements from
 ** mismatches due to sequence re-ordering. And for the description of permutations,
 ** we use a processing pattern similar to insertion sort. This allows for a very
 ** simple generation mechanism, but requires the receiver of the diff to scan
 ** down into the remainder of the data to find and fetch elements out-of-order.
 ** 
 ** @see diff-list-generation-test.cpp
 ** @see DiffApplicationStrategy
 ** @see ListDiffLanguage
 ** 
 */


#ifndef LIB_DIFF_LIST_DIFF_DETECTOR_H
#define LIB_DIFF_LIST_DIFF_DETECTOR_H


#include "lib/diff/list-diff.hpp"
#include "lib/diff/index-table.hpp"
#include "lib/iter-adapter.hpp"

#include <boost/noncopyable.hpp>
#include <utility>


namespace lib {
namespace diff{
  
  using util::unConst;
  using std::move;
  using std::swap;
  
  
  
  template<class SEQ>
  class DiffDetector
    : boost::noncopyable
    {
      using Val = typename SEQ::value_type;
      using Idx = IndexTable<Val>;
      
      Idx refIdx_;
      SEQ const& currentData_;
      
      
      using DiffStep = typename ListDiffLanguage<Val>::DiffStep;
      
      /** @internal state frame for diff detection and generation. */
      class DiffFrame;
      
      
      
      
    public:
      explicit
      DiffDetector(SEQ const& refSeq)
        : refIdx_(refSeq)
        , currentData_(refSeq)
        { }
      
      
      /** does the current state of the underlying sequence differ
       * from the state embodied into the last reference snapshot taken?
       * @remarks will possibly evaluate and iterate the whole sequence
       */
      bool
      isChanged()  const
        {
          UNIMPLEMENTED("change detection");
        }
      
      
      /** Diff is a iterator to yield a sequence of DiffStep elements */
      using Diff = lib::IterStateWrapper<DiffStep, DiffFrame>;
      
      /** Diff generation core operation.
       * Take a snapshot of the \em current state of the underlying sequence
       * and establish a frame to find the differences to the previously captured
       * \em old state. This possible difference evaluation is embodied into a #Diff
       * iterator and handed over to the client, while the snapshot of the current state
       * becomes the new reference point from now on.
       * @return iterator to yield a sequence of DiffStep tokens, which describe the changes
       *         between the previous reference state and the current state of the sequence.
       * @note takes a new snapshot to supersede the old one, i.e. updates the DiffDetector.
       * @warning the returned iterator retains a reference to the current (new) snapshot.
       *         Any concurrent modification leads to undefined behaviour. You must not
       *         invoke #pullUpdate while another client still explores the result
       *         of an old evaluation.
       */
      Diff
      pullUpdate()
        {
          Idx mark (currentData_);
          swap (mark, refIdx_);  // mark now refers to old reference point
          return Diff(DiffFrame(refIdx_, move(mark)));
        }
    };
  
  
  
  
  /**
   * A diff generation process is built on top of an "old" reference point
   * and a "new" state of the underlying sequence. Within this reference frame,
   * an demand-driven evaluation of the differences is handed out to the client
   * as an iterator. While consuming this evaluation process, both the old and
   * the new version of the sequence will be traversed once. In case of re-orderings,
   * a nested forward lookup similar to insertion sort will look for matches in the
   * old sequence, rendering the whole evaluation quadratic in worst-case.
   */
  template<class SEQ>
  class DiffDetector<SEQ>::DiffFrame
    {
      Idx old_;
      Idx* new_;
      size_t oldHead_=0,
             newHead_=0;
      
      static ListDiffLanguage<Val> token;
      
      DiffStep currentStep_;
      
      
    public:
      DiffFrame(Idx& current, Idx&& refPoint)
        : old_(refPoint)
        , new_(&current)
        , currentStep_(establishNextState())
        { }
      
      
      /* === Iteration control API for IterStateWrapper== */
      
      friend bool
      checkPoint (DiffFrame const& frame)
      {
        return token.NIL != frame.currentStep_;
      }
      
      friend DiffStep&
      yield (DiffFrame const& frame)
      {
        REQUIRE (checkPoint (frame));
        return unConst(frame).currentStep_;
      }
      
      friend void
      iterNext (DiffFrame & frame)
      {
        frame.establishNextState();
      }
      
    private:
      DiffStep
      establishNextState()
        {
          if (canPick())
            {
              consumeOld();
              return token.pick (consumeNew());
            }
          if (canDelete())
            return token.del (consumeOld());
          if (canInsert())
            return token.ins (consumeNew());
          if (needFetch())
            return token.find (consumeNew());
          if (obsoleted())
            return token.skip (consumeOld());
          
          return token.NIL;
        }
      
      bool hasOld()    const { return oldHead_ < old_.size(); }
      bool hasNew()    const { return newHead_ < new_->size(); }
      bool canPick()   const { return hasOld() && hasNew() && oldElm()==newElm(); }
      bool canDelete() const { return hasOld() && !new_->contains(oldElm());      }
      bool canInsert() const { return hasNew() && !old_.contains(newElm());       }
      bool needFetch() const { return hasNew() && oldHead_ < old_.pos(newElm());  }
      bool obsoleted() const { return hasOld() && newHead_ > new_->pos(oldElm()); }
      
      Val const& oldElm()     const { return old_.getElement (oldHead_); }
      Val const& newElm()     const { return new_->getElement (newHead_); }
      Val const& consumeOld()       { return old_.getElement (oldHead_++); }
      Val const& consumeNew()       { return new_->getElement (newHead_++); }
    };
  
  
  /** allocate static storage for the diff language token builder functions */
  template<class SEQ>
  ListDiffLanguage<typename DiffDetector<SEQ>::Val> DiffDetector<SEQ>::DiffFrame::token;
  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_LIST_DIFF_DETECTOR_H*/
