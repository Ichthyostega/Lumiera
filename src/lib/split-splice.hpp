/*
  SPLIT-SPLICE.hpp  -  Algorithm to integrate a new interval into an axis-segmentation.

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file splite-splice.hpp
 ** Generic algorithm to splice a new segment into a seamless segmentation of intervals.
 ** Here _"segmentation"_ denotes a partitioning of an ordered axis into a seamless sequence
 ** of intervals (here called "segment"). The axis is based on some _ordering type,_ like e.g.
 ** `int` (natural numbers) or lib::time::Time values, and the axis is assumed to cover a complete
 ** domain. The intervals / segments are defined by start and end point, where the start point
 ** is inclusive, yet the end point is exclusive (the next ordered point after the interval).
 ** 
 ** The purpose of the algorithm is to splice in a new segment (interval) at the proper location,
 ** based on the ordering, such as to retain the ordering and the seamless coverage of the complete
 ** axis domain. Since there are a lot of possible arrangements of intervals relative to one another,
 ** this splicing operation may necessitate to adjust _predecessor_ or _successor_ segments, and
 ** possibly even to insert additional segments to fill a gap, thereby effectively _splitting_
 ** some segment existing on the axis.
 ** 
 ** # Specification
 ** 
 ** The implementation relies upon the following assumptions:
 ** \par assumptions
 **   - there is an ordering type `ORD`, which is totally ordered,
 **     has value semantics and can be clone-initialised (can be immutable)
 **   - Segments are represented as custom data type and the Segmentation (axis)
 **     is implemented as a random-access list-like datatype with the ability to insert
 **     and delete ranges of elements, defined as pairs of _iterators_
 **   - the algorithm works solely on this _iterator type_ `POS`, which must be assignable,
 **     can be compared for equality (especially with the "end" iterator) and can be _incremented._
 ** 
 ** All further bindings to the actual implementation are abstracted as _binding functors_
 ** - `START`: when invoked on a POS (iterator), get the ORD value of the segment's start point
 ** - `AFTER`: likewise get the end point (exclusive) of the given segment
 ** - `CREATE`: create the representation of the desired new segment (which is the purpose
 **     of invoking this algorithm) and insert it _before_ the given insert POS,
 **     return an iterator (POS) pointing at the newly created segment
 ** - `EMPTY`: similarly create a new segment, which however is classified as _empty_
 **     and serves to fill a gap, notably towards the end of the axis.
 ** - `CLONE`: insert a _clone_ of an existing segment but adapt it's start and after POS;
 **     this operation effectively allows to _adapt_ adjacent segments, by also deleting
 **     the original version after the new elements have been inserted. Return POS of clone
 ** - `DELETE`: delete all segments between the given start (incl) and end (excl) POS
 ** 
 ** Moreover, POS (iterators) to indicate the complete domain are required, and a specification
 ** of the ORD values of the new segmen's _start_ and _after_ points -- which however can also
 ** be defined only partially (as optional values), to use contextual information...
 ** - if only the start point is given, then the existing segment containing this point
 **   will be shortened, and the new segment will cover the span until the existing segment's end
 ** - likewise an omitted start point will cause the new segment to be expanded towards lower ORD
 **   values until matching the end of the preceding segment
 ** - if both values are omitted, the new segment will replace the last segment of the given axis.
 ** The new segment may span an arbitrary range within the domain and may thus possibly supersede
 ** several existing segments, which are then removed by the `DELETE` operation.
 ** 
 ** @see SplitSplice_test
 ** @see steam::fixture::Segmentation::splitSplice
 */


#ifndef LIB_SPLIT_SPLICE_H
#define LIB_SPLIT_SPLICE_H

#include "lib/error.hpp"
#include "lib/meta/function.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/nocopy.hpp"

#include <array>


namespace lib {
  
  namespace error = lumiera::error;
  
  
  namespace splitsplice {/// Implementation of [»SplitSplice« algorithm](\ref splite-splice.hpp)
    
    
    /**
     * Descriptor and working context to split/splice in a new Interval.
     * The »Split-Splice« algorithm works on a seamless segmentation of
     * an ordered working axis, represented as sequence of intervals.
     * The purpose is to integrate a new Segment / interval, thereby
     * truncating / splitting / filling adjacent intervals to fit
     */
    template<class ORD        ///< order value for the segmentation
            ,class POS        ///< iterator to work with elements of the segmentation
            ,class START      ///< function to access the start value for a segment
            ,class AFTER      ///< function to access the after-end value for a segment
            ,class CREATE     ///< function to create a new segment: `createSeg(pos, start,after)`
            ,class EMPTY      ///< function to create empty segment: `emptySeg (pos, start,after)`
            ,class CLONE      ///< function to clone/modify segment: `cloneSeg (pos, start,after, src)`
            ,class DELETE     ///< function to discard a segment:    `discard  (start,after)`
            >
    class Algo
      : util::NonCopyable
      {
        /* ======= elementary operations ======= */
        
        ASSERT_VALID_SIGNATURE (START,  ORD(POS))
        ASSERT_VALID_SIGNATURE (AFTER,  ORD(POS))
        ASSERT_VALID_SIGNATURE (CREATE, POS(POS,ORD,ORD))
        ASSERT_VALID_SIGNATURE (EMPTY,  POS(POS,ORD,ORD))
        ASSERT_VALID_SIGNATURE (CLONE,  POS(POS,ORD,ORD,POS))
        ASSERT_VALID_SIGNATURE (DELETE, POS(POS,POS))
        
        START  getStart;
        AFTER  getAfter;
        CREATE createSeg;
        EMPTY  emptySeg;
        CLONE  cloneSeg;
        DELETE discard;
        
        using OptORD = std::optional<ORD>;
        const ORD AXIS_END;
        
        
        /* ======= working state ======= */
        
        POS pred_, succ_;
        
        struct SegBounds
          {                            // can be assigned in one step (ORD may be immutable)
            ORD start,
                after;
          } b_;
        
        enum Verb { NIL
                  , DROP
                  , TRUNC
                  , INS_NOP
                  , SEAMLESS
                  };
        
        Verb opPred_ = NIL,
             opSucc_ = NIL;
        
      public:
        /**
         * Setup for a single SplitSplice-operation to insert a new segment \a start to \a after.
         * @param startAll (forward) iterator pointing at the overall Segmentation begin
         * @param afterAll (forward) iterator indicating point-after-end of Segmentation
         * @param start (optional) specification of new segment's start point
         * @param after (optional) specification of new segment's end point
         */
        Algo (START  fun_getStart
             ,AFTER  fun_getAfter
             ,CREATE fun_createSeg
             ,EMPTY  fun_emptySeg
             ,CLONE  fun_cloneSeg
             ,DELETE fun_discard
             ,const ORD axisEnd
             ,POS startAll, POS afterAll
             ,OptORD start, OptORD after)
          : getStart{fun_getStart}
          , getAfter{fun_getAfter}
          , createSeg{fun_createSeg}
          , emptySeg{fun_emptySeg}
          , cloneSeg{fun_cloneSeg}
          , discard {fun_discard}
          , AXIS_END{axisEnd}
          , pred_{}
          , succ_{}
          , b_{establishSplitPoint (startAll,afterAll, start,after)}
          {
            // Postcondition: ordered start and end times
            ENSURE (pred_ != afterAll);
            ENSURE (succ_ != afterAll);
            ENSURE (b_.start < b_.after);
            ENSURE (getStart(pred_) <= b_.start);
            ENSURE (b_.start <= getStart(succ_) or pred_ == succ_);
          }
        
        /**
         * Stage-1 and Stage-2 of the algorithm determine the insert point
         * and establish the actual start and end point of the new segment
         * @return the definitive start and after ORD values, based on context
         */
        SegBounds
        establishSplitPoint (POS startAll, POS afterAll
                            ,OptORD start, OptORD after)
          { // nominal break point
            ORD sep = start? *start
                           : after? *after
                                  : AXIS_END;
            
            // find largest Predecessor with start before separator
            for (succ_ = startAll, pred_ = afterAll
                ;succ_ != afterAll and getStart(succ_) < sep
                ;++succ_)
              {
                pred_ = succ_;
              }
            REQUIRE (pred_ != succ_, "non-empty segmentation required");
            if (succ_ == afterAll) succ_=pred_;
            if (pred_ == afterAll) pred_=succ_; // separator touches bounds
            
            // Stage-2 : establish start and end point of new segment
            
            ORD startSeg = start? *start
                                : getAfter(pred_) < sep? getAfter(pred_)
                                                       : getStart(pred_);
            ORD afterSeg = after? *after
                                : getStart(succ_) > sep? getStart(succ_)
                                                       : getAfter(succ_);
            ENSURE (startSeg != afterSeg);
            if (startSeg < afterSeg)
              return {startSeg,afterSeg};
            else
              return {afterSeg,startSeg};
          }
        
        
        /**
         * Stage-3 of the algorithm works out the precise relation of the
         * predecessor and successor segments to determine necessary adjustments
         * @remark results in definition of operation verbs #opPred_ and #opSucc_.
         */
        void
        determineRelations()
          {
            ORD startPred = getStart (pred_),
                afterPred = getAfter (pred_);
            
            if (startPred < b_.start)
              {
                if (afterPred <  b_.start) opPred_ = INS_NOP;
                else
                if (afterPred == b_.start) opPred_ = SEAMLESS;
                else
                  {
                    opPred_ = TRUNC;
                    if (afterPred  > b_.after)
                      { // predecessor actually spans the new segment
                        // thus use it also as successor and truncate both (=SPLIT)
                        succ_ = pred_;
                        opSucc_ = TRUNC;
                        return;
              }   }   }
            else
              {
                REQUIRE (startPred == b_.start, "predecessor does not precede start point");
                opPred_ = DROP;
                if (b_.after < afterPred )
                  { // predecessor coincides with start of new segment
                    // thus use it rather as successor and truncate at start
                    succ_ = pred_;
                    opSucc_ = TRUNC;
                    return;
              }   }
            
            ORD startSucc = getStart (succ_);
            if (startSucc <  b_.after)
              {
                while (getAfter(succ_) < b_.after)
                  ++succ_;
                ASSERT (getStart(succ_) < b_.after  // in case we dropped a successor completely spanned,
                       ,"seamless segmentation");  //  even the next one must start within the new segment
                
                if (b_.after == getAfter(succ_)) opSucc_ = DROP;
                else
                if (b_.after <  getAfter(succ_)) opSucc_ = TRUNC;
              }
            else
              {
                if (b_.after == startSucc) opSucc_ = SEAMLESS;
                else                       opSucc_ = INS_NOP;
              }
          }
        
        
        /**
         * Stage-4 of the algorithm performs the actual insert and deleting of segments
         * @return `(s,n,e)` to indicate where changes happened
         *   - s the first changed element
         *   - n the new main segment (may be identical to s)
         *   - e the first unaltered element after the changed range (may be `end()`)
         */
        std::array<POS, 3>
        performSplitSplice()
          {
            POS refPred = pred_, refSucc = succ_;
            REQUIRE (opPred_ != NIL and opSucc_ != NIL);
            
            // deletions are done by skipping the complete range around the insertion point;
            // thus to retain a predecessor or successor, this range has to be reduced
            if (opPred_ == INS_NOP or opPred_ == SEAMLESS)
              ++pred_;
            if (opSucc_ == DROP or opSucc_ == TRUNC)
              ++succ_;
            
            // insert the new elements /before/ the range to be dropped, i.e. at pred_
            POS insPos = pred_;
            POS n = createSeg (insPos, b_.start, b_.after);
            POS s = n;
            //
            // possibly adapt the predecessor
            if (opPred_ == INS_NOP)
              s = emptySeg (n, getAfter(refPred), b_.start);
            else
            if (opPred_ == TRUNC)
              s = cloneSeg (n, getStart(refPred), b_.start, refPred);
            //
            // possibly adapt the successor
            if (opSucc_ == INS_NOP)
              emptySeg (insPos, b_.after, getStart(refSucc));
            else
            if (opSucc_ == TRUNC)
              cloneSeg (insPos, b_.after, getAfter(refSucc), refSucc);
            
            // finally discard superseded segments
            POS e = discard (insPos, succ_);
            
            // indicate the range where changes happened
            return {s,n,e};
          }
      };
    
  }//namespace splitsplace
} // namespace lib
#endif /*LIB_SPLIT_SPLICE_H*/
