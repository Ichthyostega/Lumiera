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


#ifndef PROC_ENGINE_DISPATCHER_H
#define PROC_ENGINE_DISPATCHER_H

#include "proc/common.hpp"
#include "proc/mobject/model-port.hpp"
#include "proc/engine/time-anchor.hpp"
#include "proc/engine/frame-coord.hpp"
#include "proc/engine/job-ticket.hpp"
#include "proc/engine/job-planning.hpp"
#include "lib/time/timevalue.hpp"

#include <boost/noncopyable.hpp>
#include <tr1/functional>


namespace proc {
namespace engine {
  
  using std::tr1::function;
  using mobject::ModelPort;
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
   * \par usage considerations
   * the asynchronous and ongoing nature of the render process mandates to avoid a central
   * instance for operating this planning process. Instead, each chunk of planned jobs
   * contains a continuation job, which -- on activation -- will pick up the planning
   * of the next chunk. The Dispatcher interface was shaped to support this process,
   * with a local JobBuilder to be used within the continuation job, and a TimeAnchor
   * to represent the continuation point. All the complexities of planning jobs are
   * hidden within the JobPlanningSequence, which, for the purpose of dispatching
   * a series of jobs just looks like a sequence of job descriptors
   * 
   * @todo 10/12 still WIP, but conceptually settled by now
   */
  class Dispatcher
    : public FrameLocator
    {
      struct JobBuilder
        {
          Dispatcher& dispatcher_;
          TimeAnchor refPoint_;
          ModelPort modelPort_;
          uint channel_;
          
          /////TODO need storage for the continuation
          
          /////////TODO somehow need to represent the dimensions of a "planning chunk"...
          
          FrameCoord relativeFrameLocation (TimeAnchor refPoint, uint frameCountOffset =0);
          
          JobBuilder& establishNextJobs (TimeAnchor refPoint);
          
          JobBuilder& prepareContinuation (function<void(TimeAnchor)> delayedAction);
          
          operator JobPlanningSequence()
            {
              TimeAnchor nextPlanningAnchor = refPoint_.buildNextAnchor();
              
              TODO ("build the continuation job if necessary, wrap the sequence");
              
              return JobPlanningSequence(
                  relativeFrameLocation(refPoint_),
                  dispatcher_); 
            }

        };
      
    public:
      virtual ~Dispatcher();  ///< this is an interface
      
      JobBuilder onCalcStream (ModelPort modelPort, uint channel);
      
      
    protected:
      virtual FrameCoord locateRelative (FrameCoord, uint frameCountOffset)   =0;
      virtual FrameCoord locateRelative (TimeAnchor, uint frameCountOffset)   =0;     //////////TODO is this really an interface operation, or just a convenience shortcut?
      
      virtual JobTicket& accessJobTicket (ModelPort, TimeValue nominalTime)   =0;
    };
  
  
  
}} // namespace proc::engine
#endif
