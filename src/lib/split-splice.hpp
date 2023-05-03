/*
  SPLIT-SPLICE.hpp  -  Algorithm to integrate a new interval into an axis-segmentation.

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


/** @file splite-splice.cpp
 ** Generic algorithm to splice a new segment into a seamless segmentation of intervals.
 ** 
 ** @todo 2023 WIP-WIP
 */


#ifndef LIB_SPLIT_SPLICE_H
#define LIB_SPLIT_SPLICE_H

#include "lib/error.hpp"
//#include "steam/mobject/builder/fixture-change-detector.hpp"  ///////////TODO
#include "lib/time/timevalue.hpp"
#include "lib/meta/function.hpp"
#include "lib/nocopy.hpp"

#include <array>


namespace lib {
  
  namespace error = lumiera::error;
  
  
  namespace splitsplice {// Implementation of Split-Splice algorithm
    
    using lib::meta::_Fun;
    
    template<typename FUN, typename SIG>
    struct has_Sig
      : std::is_same<SIG, typename _Fun<FUN>::Sig>
      { };
    
    /** verify the installed functors or lambdas expose the expected signature */
#define ASSERT_VALID_SIGNATURE(_FUN_, _SIG_) \
        static_assert (has_Sig<_FUN_, _SIG_>::value, "Function " STRINGIFY(_FUN_) " unsuitable, expected signature: " STRINGIFY(_SIG_));
    
    
    
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
        enum Verb { NIL
                  , DROP
                  , TRUNC
                  , INS_NOP
                  , SEAMLESS
                  };
        
        Verb opPred_ = NIL,
             opSucc_ = NIL;
        
        POS pred_,  succ_;
        ORD start_, after_;

        using OptORD = std::optional<ORD>;
        
        
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
        
        const ORD AXIS_END;
        
      public:
        /**
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
          {
            auto [start_,after_] = establishSplitPoint (startAll,afterAll, start,after);
            
            // Postcondition: ordered start and end times
            ENSURE (pred_ != afterAll);
            ENSURE (succ_ != afterAll);
            ENSURE (start_ < after_);
            ENSURE (getStart(pred_) <= start_);
            ENSURE (start_ <= getStart(succ_) or pred_ == succ_);
          }
        
        /**
         * Stage-1 and Stage-2 of the algorithm determine the insert point
         * and establish the actual start and end point of the new segment
         * @return
         */
        std::pair<ORD,ORD>
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
         */
        void
        determineRelations()
          {
            ORD startPred = getStart (pred_),
                afterPred = getAfter (pred_);
            
            if (startPred < start_)
              {
                if (afterPred <  start_) opPred_ = INS_NOP;
                else
                if (afterPred == start_) opPred_ = SEAMLESS;
                else
                  {
                    opPred_ = TRUNC;
                    if (afterPred  > after_)
                      { // predecessor actually spans the new segment
                        // thus use it also as successor and truncate both (=SPLIT)
                        succ_ = pred_;
                        opSucc_ = TRUNC;
                        return;
              }   }   }
            else
              {
                REQUIRE (startPred == start_, "predecessor does not precede start point");
                opPred_ = DROP;
                if (after_ < afterPred )
                  { // predecessor coincides with start of new segment
                    // thus use it rather as successor and truncate at start
                    succ_ = pred_;
                    opSucc_ = TRUNC;
                    return;
              }   }
            
            ORD startSucc = getStart (succ_);
            if (startSucc <  after_)
              {
                while (getAfter(succ_) < after_)
                  ++succ_;
                ASSERT (getStart(succ_) < after_    // in case we dropped a successor completely spanned,
                       ,"seamless segmentation");  //  even the next one must start within the new segment
                
                if (after_ == getAfter(succ_)) opSucc_ = DROP;
                else
                if (after_ <  getAfter(succ_)) opSucc_ = TRUNC;
              }
            else
              {
                if (after_ == startSucc) opSucc_ = SEAMLESS;
                else                     opSucc_ = INS_NOP;
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
            POS n = createSeg (pred_, start_, after_);
            POS s = n;
            //
            // possibly adapt the predecessor
            if (opPred_ == INS_NOP)
              s = emptySeg (n, getAfter(refPred), start_);
            else
            if (opPred_ == TRUNC)
              s = cloneSeg (n, getStart(refPred), start_, refPred);
            //
            // possibly adapt the successor
            if (opSucc_ == INS_NOP)
              emptySeg (pred_, after_, getStart(refSucc));
            else
            if (opPred_ == TRUNC)
              cloneSeg (pred_, after_, getAfter(refSucc), refSucc);
            
            // finally discard superseded segments
            POS e = discard (pred_, succ_);
            
            // indicate the range where changes happened
            return {s,n,e};
          }
      };
    
  }//(End) namespace splitsplace
  
  
  
} // namespace lib
#endif /*LIB_SPLIT_SPLICE_H*/
