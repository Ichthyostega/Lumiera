/*
  TIME-ANCHOR.hpp  -  current render evaluation time point closure

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file time-anchor.hpp
 ** Representation of a _continuation point_ for planning the render process.
 ** In the Lumiera engine, render and playback processes are modelled as infinite streams,
 ** which are evaluated chunk wise. The TimeAnchor is used to mark a point, where the
 ** _planning_ of further render jobs will be picked up and continued later on
 ** 
 ** @todo this is part of an implementation draft from 2013,
 **       to create a complete outline of player and render job generation.
 ** @todo as of 2016 this effort is stalled, but remains valid
 */


#ifndef STEAM_ENGINE_TIME_ANCHOR_H
#define STEAM_ENGINE_TIME_ANCHOR_H

#include "steam/common.hpp"
#include "vault/real-clock.hpp"
#include "lib/time/timevalue.hpp"
#include "steam/play/timings.hpp"
#include "steam/engine/frame-coord.hpp"



namespace steam {
namespace engine {
  
  using vault::RealClock;
  using lib::time::Offset;
  using lib::time::Duration;
  using lib::time::FrameCnt;
  using lib::time::TimeVar;
  using lib::time::Time;
  
  
  /**
   * The process of playback or rendering is a continued series of exploration and evaluation.
   * The outline of what needs to be calculated is determined continuously, proceeding in
   * chunks of evaluation. Each of these continued partial evaluations establishes a distinct
   * anchor or breaking point in time: everything before this point can be considered settled
   * and planned thus far. Effectively, this time point acts as a <i>evaluation closure</i>,
   * to be picked up for the next partial evaluation. Each time anchor defines a span of the
   * timeline, which will be covered with the next round of job planning; the successive next
   * TimeAnchor will be located at the first frame \em after this time span, resulting in
   * seamless coverage of the whole timeline. Whenever a TimeAnchor is created, a relation
   * between nominal time, current engine latency and wall clock time is established, This way,
   * the TimeAnchor closure is the definitive binding between the abstract logical time of the
   * session timeline, and the real wall-clock time forming the deadline for rendering.
   * 
   * # internals
   * The time anchor associates a nominal time, defined on the implicit time grid
   * of some given Timings, with an actual wall clock time. Due to the usage situation,
   * the TimeAnchor takes on the secondary meaning of a breaking point; everything \em before
   * this anchor point has been handled during the preceding invocations of an ongoing chunk wise
   * partial evaluation of the timeline to be "performed" within this play process.
   * - the #timings_ serve as an abstracted grid (actually, the implementation
   *   does refer to a grid defined somewhere within the session)
   * - the actual #anchorPoint_ is defined as frame number relative to this grid
   * - this anchor point is scheduled to happen at a #relatedRealTime_, based on
   *   system's real time clock scale (typically milliseconds since 1970).
   *   This schedule contains a compensation for engine and output latency.
   * 
   * @remarks please note that time anchors are set per CalcStream.
   *          Since different streams might use different frame grids, the rhythm
   *          of these planning operations is likely to be specific for a given stream.
   *          The relation to real time is established anew at each time anchor, so any
   *          adjustments to the engine latency will be reflected in the planned job's
   *          deadlines. Actually, the embedded Timings record is responsible for this
   *          timing calculation and for fetching the current EngineConfig.
   * 
   * @see Dispatcher
   * @see DispatcherInterface_test
   * @see Timings
   */
  class TimeAnchor
    {
      play::Timings timings_;
      FrameCnt anchorPoint_;
      Time relatedRealTime_;
      
      static Time
      expectedTimeofArival (play::Timings const& timings, FrameCnt startFrame, Offset startDelay)
        {
          Duration totalLatency = startDelay
                                + timings.currentEngineLatency()
                                + timings.outputLatency;
          TimeVar deadline;
          switch (timings.playbackUrgency)
            {
            case play::ASAP:
            case play::NICE:
              deadline = RealClock::now() + totalLatency;
              break;
            
            case play::TIMEBOUND:
              deadline = timings.getTimeDue(startFrame) - totalLatency;
              break;
            }
          return deadline;
        }
      
      
    public:
      TimeAnchor (play::Timings timings, FrameCnt startFrame, Offset startDelay =Offset::ZERO)
        : timings_(timings)
        , anchorPoint_(startFrame)
        , relatedRealTime_(expectedTimeofArival(timings,startFrame,startDelay))
        { }
      
      // using default copy operations
      
      
      
      /** set a follow-up TimeAnchor point.
       *  After planning a chunk of jobs, the dispatcher uses
       *  this function to set up a new breaking point (TimeAnchor)
       *  and places a continuation job to resume the planning activity.
       * @note precisely satisfies the <i>planning chunk duration</i>
       * @return a frame number suitable to build the next TimeAnchor
       *       based on the current play::Timings. This new start point
       *       will be anchored at the grid point following the end of
       *       the previous planning chunk, resulting in a seamless
       *       coverage of the timeline 
       */
      FrameCnt
      getNextAnchorPoint()  const
        {
          return timings_.establishNextPlanningChunkStart (this->anchorPoint_);
        }
      
      
      /** @internal for debugging and diagnostics:
       * explicitly cast this TimeAnchor onto the underlying
       * nominal time scale (as defined by the Timings of this
       * playback or render process). */
      operator lib::time::TimeValue()  const
        {
          return timings_.getFrameStartAt (anchorPoint_);
        }
      
      
      /** @return the frame at which any job planning
       *          for this planning chunk will start */
      FrameCnt getStartFrame()  const
        {
          return anchorPoint_;
        }
      
      
      /** define the deadline for a grid point relative to this reference point.
       *  Since a TimeAnchor represents the definitive link between nominal time
       *  and ongoing wall clock time, and since all of the current output stream
       *  related timing information is available -- including the engine and the
       *  output latency -- this is the place to do the final decision.
       * @param frameOffset frame count offset relative to this TimeAnchor point
       * @return the latest real absolute wall clock time at which this frame
       *         has to be delivered to the OutputSlot. This deadline is exclusive,
       *         i.e. time < deadline is required.
       */
      Time
      establishDeadlineFor (FrameCnt frameOffset)
        {
          return this->relatedRealTime_
               + timings_.getRealOffset(frameOffset);
        }
      
      
      /** convenience shortcut, employing the deadline calculation in relation
       *  to current wall clock time */
      Offset
      remainingRealTimeFor (FrameCoord plannedFrame)
        {
          FrameCnt frameOffset = plannedFrame.absoluteFrameNumber - anchorPoint_;
          return Offset(RealClock::now()
                       ,establishDeadlineFor(frameOffset));
        }
    };
  
  
  
}} // namespace steam::engine
#endif
