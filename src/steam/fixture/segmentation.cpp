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

#include <tuple>


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
        
        Verb opPred = NIL,
             opSucc = NIL;
        
        Iter pred{};
        Iter succ{};
        
        Time start = Time::NEVER,
             after = Time::NEVER;
        
        /* ======= elementary operations ======= */
        
        Time getStart (Iter elm) { return elm->start(); }
        Time getAfter (Iter elm) { return elm->after(); }
        
        Iter
        createElm (Time start, Time after)
          {
            UNIMPLEMENTED ("create new Segment");
          }
        
        Iter
        cloneElm (Iter elm, Time start, Time after)
          {
            UNIMPLEMENTED ("clone Segment and modify time");
          }
        
      public:
        void
        establishSplitPoint (Iter startAll, Iter afterAll
                            ,OptTime start, OptTime after)
          {
            UNIMPLEMENTED ("Stage-1 and Stage-2");
          }
        
        void
        determineRelations()
          {
            UNIMPLEMENTED ("Stage-3");
          }
        
        std::pair<Iter, Iter>
        performSplitSplice()
          {
            UNIMPLEMENTED ("Stage-4 - doIT");
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
    SplitSpliceAlgo splicr;
    splicr.establishSplitPoint (segments_.begin(),segments_.end(), start,after);
    splicr.determineRelations();
    splicr.performSplitSplice();
  }
  
  
  
//LUMIERA_ERROR_DEFINE (DUPLICATE_MODEL_PORT, "Attempt to define a new model port with an pipe-ID already denoting an existing port");
  
  
  
}} // namespace steam::fixture
