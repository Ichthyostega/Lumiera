/*
  Segmentation  -  Partitioning of a timeline for organising the render graph.

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


/** @file segmentation.cpp
 ** @todo stalled effort towards a session implementation from 2008
 ** @todo 2016 likely to stay, but expect some extensive rework
 */

#include "lib/error.hpp"
#include "steam/fixture/segmentation.hpp"
//#include "steam/mobject/builder/fixture-change-detector.hpp"  ///////////TODO
#include "lib/time/timevalue.hpp"

#include <array>


namespace steam {
namespace fixture {
  
  namespace error = lumiera::error;
  
  
//  typedef ModelPortRegistry::ModelPortDescriptor const& MPDescriptor;
  
  
  /** storage for the link to the global
      Registry instance currently in charge  */
//  lib::OptionalRef<ModelPortRegistry> ModelPortRegistry::theGlobalRegistry;
  
  
  Segmentation::~Segmentation() { }   // emit VTable here...
  
  namespace {// Implementation of Split-Splice algorithm
    
    using lib::time::Time;
    using lib::time::TimeVar;
    using OptTime = std::optional<Time>;
    using Iter = typename list<Segment>::iterator;
    
    
    /**
     * Descriptor and working context to split/splice in a new Interval.
     * The »Split-Splice« algorithm works on a seamless segmentation of
     * an ordered working axis, represented as sequence of intervals.
     * The purpose is to integrate a new Segment / interval, thereby
     * truncating / splitting / filling adjacent intervals to fit
     */
    class SplitSpliceAlgo
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
        
        Iter pred_,  succ_;
        Time start_, after_;
        
        /* ======= elementary operations ======= */
        
        Time getStart (Iter elm) { return elm->start(); }
        Time getAfter (Iter elm) { return elm->after(); }
        
        Iter
        createSeg (Iter pos, Time start, Time after)
          {
            UNIMPLEMENTED ("create new Segment");
          }
        
        Iter
        emptySeg (Iter pos, Time start, Time after)
          {
            UNIMPLEMENTED ("create new Segment");
          }
        
        Iter
        cloneSeg (Iter pos, Time start, Time after, Iter src)
          {
            UNIMPLEMENTED ("clone Segment and modify time");
          }
        
        Iter
        discard (Iter start, Iter after)
          {
            UNIMPLEMENTED ("discard Segments");
          }
        
        
      public:
        /**
         * @param startAll (forward) iterator pointing at the overall Segmentation begin
         * @param afterAll (forward) iterator indicating point-after-end of Segmentation
         * @param start (optional) specification of new segment's start point
         * @param after (optional) specification of new segment's end point
         */
        SplitSpliceAlgo (Iter startAll, Iter afterAll
                        ,OptTime start, OptTime after)
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
        std::pair<Time,Time>
        establishSplitPoint (Iter startAll, Iter afterAll
                            ,OptTime start, OptTime after)
          { // nominal break point  
            Time sep = start? *start
                            : after? *after
                                   : Time::NEVER;
            
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
            
            Time startSeg = start? *start
                                 : getAfter(pred_) < sep? getAfter(pred_)
                                                        : getStart(pred_);
            Time afterSeg = after? *after
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
            Time startPred = getStart (pred_),
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
            
            TimeVar startSucc = getStart (succ_),
                    afterSucc = getAfter (succ_);
            
            if (startSucc <  after_)
              {
                while (afterSucc < after_)
                  {
                    ++succ_;
                    startSucc = getStart (succ_);
                    afterSucc = getAfter (succ_);
                  }
                ASSERT (startSucc < after_          // in case we dropped a successor completely spanned,
                       ,"seamless segmentation");  //  even the next one must start within the new segment
                
                if (after_ == afterSucc) opSucc_ = DROP;
                else
                if (after_ <  afterSucc) opSucc_ = TRUNC;
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
        std::array<Iter, 3>
        performSplitSplice()
          {
            Iter refPred = pred_, refSucc = succ_;
            REQUIRE (opPred_ != NIL and opSucc_ != NIL);
            
            // deletions are done by skipping the complete range around the insertion point;
            // thus to retain a predecessor or successor, this range has to be reduced
            if (opPred_ == INS_NOP or opPred_ == SEAMLESS)
              ++pred_;
            if (opSucc_ == DROP or opSucc_ == TRUNC)
              ++succ_;
            
            // insert the new elements /before/ the range to be dropped, i.e. at pred_
            Iter n = createSeg (pred_, start_, after_);
            Iter s = n;
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
            Iter e = discard (pred_, succ_);
            
            // indicate the range where changes happened
            return {s,n,e};
          }
      };
    
  }//(End)SlitSplice impl
  
  
  /**
   * @param start (optional) definition of the new Segment's start point (inclusive)
   * @param after (optional) definition of the end point (exclusive)
   * @param jobTicket specification of provided render functionality for the new Segment
   * @remarks missing definitions will be derived or interpolated according to context
   *   - if start point is omitted, the new Segment will start seamlessly after
   *     any preceding Segment's end, in case this preceding Segment ends earlier
   *   - otherwise the preceding Segment's start point will be used, thereby effectively
   *     replacing and expanding or trimming or inserting into the preceding Segment
   *   - similar for the end point: if the definition is omitted, the new Segment
   *     will cover the time range until the next Segmen's start
   *   - if upper/lower boundaries can not be established, the covered range will be
   *     expanded from Time::ANYTIME up to Time::ANYTIME in as fitting current context
   *   - after start and end point have been established by the above rules, the actual
   *     splicing operation will be determined; either an existing Segment is replaced
   *     altogether, or it is trimmed to fit, or the new Segment is inserted, thereby
   *     creating a second (copied) part of the encompassing old Segment.
   *   - in case the JobTicket is omitted, the new Segment will be marked as _passive_
   *     and any job created from such a Segment will then be a »NOP-job«
   */
  Segment const&
  Segmentation::splitSplice (OptTime start, OptTime after, const engine::JobTicket* jobTicket)
  {
    SplitSpliceAlgo splicr{segments_.begin(),segments_.end(), start,after};
    splicr.determineRelations();
    auto [s,n,e] = splicr.performSplitSplice();
    return *n;
  }
  
  
  
//LUMIERA_ERROR_DEFINE (DUPLICATE_MODEL_PORT, "Attempt to define a new model port with an pipe-ID already denoting an existing port");
  
  
  
}} // namespace steam::fixture
