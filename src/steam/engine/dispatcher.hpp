/*
  DISPATCHER.hpp  -  translating calculation streams into frame jobs

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file dispatcher.hpp
 ** Service abstraction within the render engine for generating render jobs.
 ** On interface level, the render engine uses the notion of a [calculation stream](\ref CalcStream)
 ** to represent an ongoing rendering process. Within the _implementation_ of such a process,
 ** the Dispatcher is responsible for transforming the generic setup of such a calculation stream
 ** into a sequence of concrete jobs, anchored at some distinct point in time.
 ** 
 ** @todo valid draft, unfortunately stalled in 2013
 */


#ifndef STEAM_ENGINE_DISPATCHER_H
#define STEAM_ENGINE_DISPATCHER_H

#include "steam/common.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/engine/time-anchor.hpp"
#include "steam/engine/frame-coord.hpp"
#include "steam/engine/job-ticket.hpp"
#include "steam/engine/job-planning.hpp"
#include "lib/time/timevalue.hpp"

#include <functional>


namespace steam {
namespace engine {
  
  using std::function;
  using mobject::ModelPort;
  using lib::time::FrameCnt;
  using lib::time::TimeSpan;
  using lib::time::FSecs;
  using lib::time::Time;
  
  
  /**
   * Internal abstraction: a service within the engine
   * for translating a logical calculation stream (corresponding to a PlayProcess)
   * into a sequence of individual RenderJob entries for calculations and data access.
   * The actual implementation of this service is tied to the low-level-model, i.e.
   * the render nodes network. The Dispatcher service is used to implement the CalcStreams
   * during playback and rendering; there will be a continuous, chunk-wise proceeding
   * evaluation and planning of new jobs, which can then be handed over to the Scheduler
   * for time-bound activation.
   * 
   * ## usage considerations
   * the asynchronous and ongoing nature of the render process mandates to avoid a central
   * instance for operating this planning process. Instead, together with each chunk of
   * planned jobs we generate a continuation job, which -- on activation -- will pick up
   * the planning of the next chunk. The Dispatcher interface was shaped especially to
   * support this process, with a local JobBuilder for use within the continuation job,
   * and a TimeAnchor to represent the continuation point. All the complexities of
   * actually planning the jobs are hidden within the JobPlanningSequence,
   * which, for the purpose of dispatching a series of jobs just looks
   * like a sequence of job descriptors
   * 
   * @todo 10/12 still WIP, but conceptually settled by now
   */
  class Dispatcher
    : public FrameLocator
    {
      struct JobBuilder
        {
          Dispatcher* dispatcher_;
          ModelPort modelPort_;
          uint channel_;
          
          FrameCoord relativeFrameLocation (TimeAnchor& refPoint, FrameCnt frameCountOffset =0);
          
          JobPlanningSequence
          establishNextJobs (TimeAnchor& refPoint)
            {
              return JobPlanningSequence(
                  relativeFrameLocation(refPoint),
                  *dispatcher_);
            }
        };
      
      
    public:
      virtual ~Dispatcher();  ///< this is an interface
      
      JobBuilder onCalcStream (ModelPort modelPort, uint channel);
      
      
    protected:
      /** core dispatcher operation: based on the coordinates of a reference point,
       *  establish binding frame number, nominal time and real (wall clock) deadline.
       * @return new FrameCoord record (copy), with the nominal time, frame number
       *         and deadline adjusted in accordance to the given frame offset.
       */
      virtual FrameCoord locateRelative (FrameCoord const&, FrameCnt frameOffset)  =0;
      
      virtual bool       isEndOfChunk   (FrameCnt, ModelPort port)                 =0;

      ////////TODO: API-1 = just get next frame, without limitations  .... CHECK
      ////////TODO: API-2 = query limitation of planning chunk        .... CHECK
      ////////TODO: API-3 = establish next chunk                      .... still WIP
      
      ////////TODO: Question: why not embedding the time anchor directly within the location generator??
      ////////      Answer: no this would lead to a huge blob called "the dispatcher"
      
      ////////TODO: immediate point to consider:  the time anchor is responsible for the real timing calculations. But how to introduce the play strategy *here* ?
      
      ////////////TODO: the solution is simple: get rid of the additional job placed magically into the chunk
      ////////////      instead, provide a dedicated API function to create exactly that job
      ////////////      and *enclosed* into a specialised JobClosure subclass, embody the code for the follow-up
      ////////////      As a corollary: the scheduling deadline should be defined right *within* the job!
      
      ////////////TODO: remaining issues
      ////////////    - the TimeAnchor needs to be created directly from the JobParameter. No mutable state!
      ////////////    - but this leads to a lot of duplicated Timings records, unless we rewrite the TimeAnchor to be noncopyable and use a Timings const&
      
      virtual JobTicket& accessJobTicket (ModelPort, TimeValue nominalTime)   =0;
    };
  
  
  
}} // namespace steam::engine
#endif
