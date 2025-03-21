/*
  TIMINGS.hpp  -  timing specifications for a frame quantised data stream

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file timings.hpp
 ** How to define a timing specification or constraint.
 ** A timing spec is used to anchor a data stream with relation to a time axis or frame grid.
 ** There are two kinds of timing specs:
 ** - nominal timing specifications relate to the nominal time values
 **   of the frames in a data stream, i.e. the "should be" time values.
 **   These might be values derived from a timecode or just values in
 **   in relation to the timeline axis, but without any connection to
 **   the real wall clock time
 ** - actual timing specifications are always connected or related to
 **   an external time source, typically just wall clock time. For example,
 **   actual timing specs dictate the constraints for real time frame
 **   delivery to an external output connection.
 **   
 ** @todo WIP-WIP-WIP 8/2011
 ** @see output-slot.hpp  ////TODO
 */


#ifndef STEAM_PLAY_TIMINGS_H
#define STEAM_PLAY_TIMINGS_H


#include "lib/error.hpp"
//#include "lib/handle.hpp"
#include "lib/time/timevalue.hpp"
//#include "steam/engine/buffer-provider.hpp"
//#include "lib/iter-source.hpp"
//#include "lib/sync.hpp"

//#include <string>
//#include <vector>
#include <boost/rational.hpp>
#include <memory>

namespace lib {
namespace time{
  class Quantiser;
  typedef std::shared_ptr<const Quantiser> PQuant;
}}

namespace steam {
namespace play {

  using lib::time::FrameCnt;
  using lib::time::FrameRate;
  using lib::time::TimeValue;
  using lib::time::Duration;
  using lib::time::Offset;
  using lib::time::Time;
//using std::string;

//using std::vector;
//using std::shared_ptr;
  
  enum PlaybackUrgency {
    ASAP,
    NICE,
    TIMEBOUND
  };
  
  
  
  /*************************************************************************//**
   * Generic frame timing specification. Defines the expected delivery interval,
   * optionally also the expected quality-of-service (urgency).
   * 
   * @note copyable value class
   * 
   * @todo write type comment
   *          /////////////////////////////////////////////////////////////////TODO: WIP seems to become some kind of abstracted Grid representation!
   */
  class Timings
    {
      lib::time::PQuant grid_;
      
    public:
      PlaybackUrgency playbackUrgency;
      boost::rational<FrameCnt> playbackSpeed;                     /////////////TICKET #902 we need a more generic representation for variable speed playback
      Time scheduledDelivery;                           ///< a wall clock time corresponding to the Grid's origin. Can be Time::Never (=not time bound)
      Duration outputLatency;                                    ///////////////TICKET #802 this information is necessary, but it is not clear who maintains and updates the latency
      Duration engineLatency;                           ///< reasonable guess at the scheduling and dispatch-delay of the render engine
      
      explicit
      Timings (FrameRate fps);
      
      Timings (FrameRate fps, Time realTimeAnchor);
      
      // default copy acceptable
      
      
      /** marker for halted output */
      static Timings DISABLED;
      
      Time     getOrigin()  const;
      
      Time     getFrameStartAt    (FrameCnt frameNr)    const;
      Duration getFrameDurationAt (TimeValue refPoint)  const;
      Duration getFrameDurationAt (FrameCnt refFrameNr) const;
      
      /** the next grid point at or after the given reference time */
      FrameCnt getBreakPointAfter (TimeValue refPoint)  const;
      
      /** the frame spacing and duration remains constant for some time...
       * @param startPoint looking from that time point into future
       * @return duration after this starting point, where it's safe
       *         to assume unaltered frame dimensions */
      Duration constantFrameTimingsInterval (TimeValue startPoint)  const;
      
      /** calculate the given frame's distance from origin,
       *  but do so using the real time scale, including any
       *  playback speed factor and similar corrections.
       * @param frameOffset frame number relative to the implicit grid
       * @return real time value relative to the implicit grid's zero point
       * @note since the Timings don't contain any information relating the
       *       nominal time scale to wall clock time, this result is just
       *       a relative offset, but expressed in real time scale values
       * @see steam::engine::TimeAnchor for an absolutely anchored conversion
       */
      Offset getRealOffset (FrameCnt frameOffset)  const;
      
      /** real time deadline for the given frame, without any latency.
       *  This value is provided in case of scheduled time of delivery,
       *  which is signalled  by `playbackUrgency == TIMEBOUND`
       * @return wall clock time to expect delivery of data
       *         corresponding to a frame specified relative
       *         to [time axis origin](\ref #getOrigin)
       * @note for other playback urgencies `Time::NEVER` is returned
       * 
       * @warning not clear as of 1/13 if it is even possible to have such a function
       *          on the Timings record. 
       */
      Time getTimeDue(FrameCnt frameOffset)  const;
      
      /** the minimum time span to be covered by frame calculation jobs
       *  planned in one sway. The ongoing planning of additional jobs
       *  proceeds in chunks of jobs added at once to the schedule.
       *  This setting defines the minimum time to plan ahead; after
       *  covering at least this time span with new jobs, the
       *  frame dispatcher concludes "enough for now" and emits
       *  a continuation job for the next planning chunk.
       */
      Duration getPlanningChunkDuration() const;
      
      /** establish the time point to anchor the next planning chunk,
       *  in accordance with #getPlanningChunkDuration
       * @param anchorFrame frame number where the current planning chunk started
       * @return number of the first frame, which is located strictly more than
       *         the planning chunk duration into the future
       * @remarks this value is used by the frame dispatcher to create a
       *          follow-up planning job */
      FrameCnt establishNextPlanningChunkStart(FrameCnt anchorFrame)  const;
      
      
      bool isOriginalSpeed()  const;
      bool isTimebound()      const;
      
      
      /** Consistency self-check */
      bool isValid()  const;
      
      
      //////////////TODO further accessor functions here
      
      Timings constrainedBy (Timings additionalConditions);
      
    };
  
  
  
  
  
  inline bool
  Timings::isOriginalSpeed()  const
  {
    return 1 == playbackSpeed;
  }
  
  inline bool
  Timings::isTimebound()  const
  {
    return play::TIMEBOUND == playbackUrgency;
  }
  
  
  
}} // namespace steam::play
#endif
