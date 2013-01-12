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


#ifndef PROC_ENGINE_TIME_ANCHOR_H
#define PROC_ENGINE_TIME_ANCHOR_H

#include "proc/common.hpp"
#include "backend/real-clock.hpp"
#include "lib/time/timevalue.hpp"
#include "proc/play/timings.hpp"
#include "proc/engine/frame-coord.hpp"



namespace proc {
namespace engine {
  
  using backend::RealClock;
  using lib::time::Offset;
  using lib::time::Duration;
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
   * \par internals
   * The time anchor associates a nominal time, defined on the implicit time grid
   * of some given Timings, with an actual wall clock time. Due to the usage situation,
   * the TimeAnchor takes on the secondary meaning of a breaking point; everything \em before
   * this anchor point has been handled during the preceding invocations of an ongoing chunk wise
   * partial evaluation of the timeline to play back.
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
   *          deadlines. Actually, the embedded Timings record is responsible for these
   *          timing calculation and for fetching the current EngineConfig.
   * 
   * @see Dispatcher
   * @see DispatcherInterface_test
   * @see Timings
   */
  class TimeAnchor
    {
      play::Timings timings_;
      int64_t anchorPoint_;
      Time relatedRealTime_;
      
      Time
      expectedTimeofArival (play::Timings const& timings, int64_t startFrame, Offset startDelay)
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
      
      
      TimeAnchor (play::Timings timings, int64_t startFrame, Offset startDelay =Offset::ZERO)
        : timings_(timings)
        , anchorPoint_(startFrame)
        , relatedRealTime_(expectedTimeofArival(timings,startFrame,startDelay))
        { }
      
    public:
      // using default copy operations
      
      
      /** create a TimeAnchor for playback/rendering start at the given startFrame.
       *  For latency calculations, the EngineConfig will be queried behind the scenes.
       *  regarding the reaction latency required to get the engine
       * @note this builder function adds an additional, hard wired start margin
       *       of one frame duration, to compensate for first time effects.
       */
      static TimeAnchor
      build (play::Timings timings, int64_t startFrame)
        {
          Offset startDelay(timings.getFrameDurationAt(startFrame));
          return TimeAnchor (timings,startFrame,startDelay);
        }
      
      
      /** create a follow-up TimeAnchor.
       *  After planning a chunk of jobs, the dispatcher uses
       *  this function to set up a new breaking point (TimeAnchor)
       *  and places a continuation job to resume the planning activity.
       * @return new TimeAchor which precisely satisfies the <i>planning
       *         chunk duration</i>: it will be anchored at the following
       *         grid point, resulting in seamless coverage of the timeline 
       */
      TimeAnchor
      buildNextAnchor() const
        {
          int64_t nextStart = timings_.establishNextPlanningChunkStart (this->anchorPoint_);
          return TimeAnchor(this->timings_, nextStart);
        }
      
      
      /** @internal for debugging and diagnostics:
       * explicitly cast this TimeAnchor onto the underlying
       * nominal time scale (as defined by the Timings of this
       * playback or render process). */
      operator lib::time::TimeValue()  const
        {
          return timings_.getFrameStartAt (anchorPoint_);
        }
      
      
      
      Offset
      remainingRealTimeFor (FrameCoord plannedFrame)
        {
          int64_t frameOffset = plannedFrame.absoluteFrameNumber - anchorPoint_;
          return Offset(this->relatedRealTime_
                      + timings_.getRealOffset(frameOffset)
                      - RealClock::now());
        }
    };
  
  
  
}} // namespace proc::engine
#endif
