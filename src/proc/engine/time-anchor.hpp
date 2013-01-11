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

#include <boost/rational.hpp>



namespace proc {
namespace engine {
  
  using backend::RealClock;
  using lib::time::Offset;
  using lib::time::TimeVar;
  using lib::time::Time;
  
  
  /**
   * The process of playback or rendering is a continued series of exploration and evaluation.
   * The outline of what needs to be calculated is determined continuously, proceeding in
   * chunks of evaluation. Each of these continued partial evaluations establishes a distinct
   * anchor or breaking point in time: everything before this point can be considered settled
   * and planned thus far. Effectively, this time point acts as a <i>evaluation closure</i>,
   * to be picked up on the next partial evaluation. More specifically, the TimeAnchor closure
   * is the definitive binding between the abstract logical time of the session timeline, and
   * the real wall-clock time forming the deadline for rendering.
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
   *   system's real time clock scale (typically milliseconds since 1970)
   * 
   * @todo 1/12 WIP-WIP-WIP just emerging as a concept
   *          /////////////////////////////////////////////////////////////////TODO: WIP needs to act as proxy for the grid, using the Timings
   */
  class TimeAnchor
    {
      play::Timings timings_;
      int64_t anchorPoint_;
      Time relatedRealTime_;
      
      Time
      expectedTimeofArival (play::Timings const& timings, Offset engineLatency)
        {
          TimeVar deadline;
          switch (timings.playbackUrgency)
            {
            case play::ASAP:
            case play::NICE:
              deadline = RealClock::now() + engineLatency;
              break;
            
            case play::TIMEBOUND:
              deadline = timings.getTimeDue() - engineLatency; 
              break;
            }
          return deadline - timings.outputLatency;
        }
      
      
      TimeAnchor (play::Timings timings, Offset engineLatency, int64_t startFrame)
        : timings_(timings)
        , anchorPoint_(startFrame)
        , relatedRealTime_(expectedTimeofArival(timings, engineLatency))
        { }
      
    public:
      // using default copy operations
      
      
      /** create a TimeAnchor for playback/rendering start
       *  at the given startFrame. Since no information is given
       *  regarding the reaction latency required to get the engine
       *  to deliver at a given time, this "engine latency" is guessed
       *  to be 1/3 of the frame duration.
       * @note using this function in case of "background" rendering
       *       doesn't make much sense; you should indeed retrieve the
       *       start delay from internals of the engine in this case.
       */
      static TimeAnchor
      build (play::Timings timings, int64_t startFrame)
        {
          const boost::rational<uint> DEFAULT_LATENCY_FACTOR (1,3);
          Offset startDelay(timings.outputLatency
                          + timings.getFrameDurationAt(startFrame) * DEFAULT_LATENCY_FACTOR
                           );
          
          return TimeAnchor (timings,startDelay,startFrame);
        }
      
      //////////////////////////////////////////////////////////////////////////////////////////TODO: second builder function, relying on Engine timings
      
      
      /** @internal for debugging and diagnostics:
       * explicitly cast this TimeAnchor onto the underlying
       * nominal time scale (as defined by the Timings of this
       * playback or render process). */
      operator lib::time::TimeValue()  const
        {
          UNIMPLEMENTED ("representation of the Time Anchor closure");
        }
    };
  
  
  
}} // namespace proc::engine
#endif
