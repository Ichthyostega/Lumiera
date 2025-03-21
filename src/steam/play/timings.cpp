/*
  Timings  -  timing specifications for a frame quantised data stream

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file timings.cpp
 ** Implementation of a data tuple for timing specification.
 ** A Timings record represents a constraint on playback, and will be
 ** used to organise and the calculations necessary to generate the data.
 ** Especially, a Timings record serves to establish a relation to an externally
 ** fixed time grid or time source.
 */


#include "steam/play/timings.hpp"
#include "lib/time/formats.hpp"
#include "lib/time/timequant.hpp"
#include "lib/rational.hpp"


namespace steam {
namespace play {
  
  using lib::time::PQuant;
  using lib::time::Time;
  using lib::time::TimeVar;
  using lib::time::FrameCnt;
  using lib::time::FSecs;
  
  namespace { // Hard wired placeholder settings...
    
    const Duration DEFAULT_ENGINE_LATENCY  = Duration{Time{10,0}};    ///////////////////////////////////////TICKET #802 : shouldn't be hard wired
    const Duration DEFAULT_JOB_PLANNING_TURNOVER(FSecs(3,2));
    
  }//(End)hard wired settings
  
  
  namespace { // hidden local details of the service implementation....
    
    inline PQuant
    buildStandardGridForFramerate (FrameRate fps)
    {
      return PQuant (new lib::time::FixedFrameQuantiser (fps));
    }                         //////TODO maybe caching these quantisers? they are immutable and threadsafe
    
  } // (End) hidden service impl details
  
  
  
  /** Create a default initialised Timing constraint record.
   * Using the standard optimistic settings for most values,
   * no latency, no special requirements. The frame grid is
   * rooted at the "natural" time origin; it is not related
   * in any way to the current session.
   * @remarks this ctor is intended rather for testing purposes!
   *          Usually, when creating a play/render process,
   *          the actual timings _are related to the timeline_
   *          and the latency/speed requirements of the output.
   */
  Timings::Timings (FrameRate fps)
    : grid_{buildStandardGridForFramerate(fps)}
    , playbackUrgency {ASAP}
    , playbackSpeed {1}
    , scheduledDelivery{Time::NEVER}
    , outputLatency{Duration::NIL}
    , engineLatency{DEFAULT_ENGINE_LATENCY}    //////////////////////////////////////////////////////////////TICKET #802 : derive from engine state -- but make it adjustable for unit tests!!!
    { 
      ENSURE (grid_);
    }
  
  Timings::Timings (FrameRate fps, Time realTimeAnchor)
    : grid_{buildStandardGridForFramerate(fps)}
    , playbackUrgency {TIMEBOUND}
    , playbackSpeed {1}
    , scheduledDelivery{realTimeAnchor}
    , outputLatency {Duration::NIL}
    , engineLatency{DEFAULT_ENGINE_LATENCY}
    {
      ENSURE (grid_);
    }
  
  //////////////////////////////////////////////////////////////////TODO ctors for use in the real player/engine?
  
  
  /** a special marker Timings record,
   * indicating disabled or halted output */
  Timings Timings::DISABLED(FrameRate::HALTED);
  
  
  
  /** @internal typically invoked from assertions */
  bool
  Timings::isValid()  const
  {
    return bool(grid_)
        && (( (ASAP == playbackUrgency || NICE == playbackUrgency)
            && Time::NEVER == scheduledDelivery)
           ||
            (TIMEBOUND == playbackUrgency
            && Time::MIN < scheduledDelivery && scheduledDelivery < Time::MAX)
           );
  }
  
  Time
  Timings::getOrigin()  const
  {
    return Time(grid_->timeOf(0));
  }
  
  
  Time
  Timings::getFrameStartAt (FrameCnt frameNr)  const
  {
    return Time(grid_->timeOf(frameNr));
  }
  
  
  Duration
  Timings::getFrameDurationAt (TimeValue refPoint)  const
  {
    FrameCnt frameNr = grid_->gridPoint (refPoint);
    return getFrameDurationAt(frameNr);
  }
  
  
  Duration
  Timings::getFrameDurationAt (FrameCnt refFrameNr)  const
  {
    return Offset (grid_->timeOf(refFrameNr), grid_->timeOf(refFrameNr + 1));
  }
  
  
  FrameCnt
  Timings::getBreakPointAfter (TimeValue refPoint)  const
  {
    FrameCnt frameNr = grid_->gridPoint (refPoint);
    return grid_->timeOf(frameNr) == refPoint? frameNr
                                             : frameNr+1;
  }
  
  
  
  /** @remarks the purpose of this function is to support scheduling
   *           and frame handling even in case the frame rate isn't constant.
   *           To indicate the case the frame rate is changing right now,
   *           this function might return Duration::NIL
   *  @todo implement real support for variable frame rates
   */                                  ////////////////////////////////////////////////////////TICKET #236
  Duration
  Timings::constantFrameTimingsInterval (TimeValue)  const
  {
    return Duration (Time::MAX);
  }
  
  
  Time
  Timings::getTimeDue(FrameCnt frameOffset)  const
  {
    if (TIMEBOUND == playbackUrgency)
      {
        REQUIRE (scheduledDelivery != Time::NEVER);
        return scheduledDelivery
             + getRealOffset (frameOffset);
      }
    else
      return Time::NEVER;
  }
  
  
  Offset
  Timings::getRealOffset (FrameCnt frameOffset)  const
  {
    Offset nominalOffset (grid_->timeOf(0), grid_->timeOf(frameOffset));
    return isOriginalSpeed()? nominalOffset
                            : nominalOffset * playbackSpeed;
                                             ////////////////////////TICKET #902  for full-featured variable speed playback, we need to integrate (sum up step wise) instead of just using a fixed factor 
  }
  
  
  Duration
  Timings::getPlanningChunkDuration()  const
  {
    UNIMPLEMENTED ("controlling the job planning rhythm");
  }
  
  
  FrameCnt
  Timings::establishNextPlanningChunkStart(FrameCnt anchorFrame)  const
  {
    TimeVar breakingPoint = grid_->timeOf(anchorFrame);
    breakingPoint += getPlanningChunkDuration();
    FrameCnt nextFrame = grid_->gridPoint (breakingPoint);
    
    ASSERT (breakingPoint <= grid_->timeOf(nextFrame));
    ASSERT (breakingPoint >  grid_->timeOf(nextFrame-1));
    
    if (grid_->timeOf(nextFrame) == breakingPoint)
      return nextFrame;
    else
      return nextFrame+1;
  }
  
  
  
  Timings
  Timings::constrainedBy (Timings additionalConditions)
  {
    UNIMPLEMENTED ("how to combine timing constraints");
  }
  
  
  
}} // namespace steam::play
