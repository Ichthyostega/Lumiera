/*
  RENDER-DRIVE.hpp  -  repetitively advancing a render calculation stream

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


/** @file render-drive.hpp
 ** The active core within a CalcStream, causing the render mechanism to re-trigger repeatedly.
 ** Rendering is seen as an open-ended, ongoing process, and thus the management and planning
 ** of the render process itself is performed chunk wise and embedded into the other rendering
 ** calculations. The _"rendering-as-it-is-planned-right-now"_ can be represented as a closure
 ** to the jobs, which perform and update this plan on the go. And in fact, the head of the
 ** planning process, the CalcStream, maintains this closure instance, as parametrised
 ** with the appropriate configuration for the specific playback/render process underway.
 ** Enclosed into this instance lives the actual job planning pipeline, connected at the
 ** rear to the dispatcher and thus to the fixture and the low-level model
 ** 
 ** @todo 4/2023 »Playback Vertical Slice« -- effort towards first integration of render process ////////////TICKET #1221
 */


#ifndef STEAM_ENGINE_RENDER_DRIVE_H
#define STEAM_ENGINE_RENDER_DRIVE_H

#include "steam/common.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/engine/time-anchor.hpp"
#include "steam/engine/dispatcher.hpp"
#include "steam/play/timings.hpp"
#include "vault/engine/job.h"
//#include "lib/nocopy.hpp"


namespace steam {
namespace engine {
  
//  using std::function;
  using vault::engine::JobParameter;
  using vault::engine::JobClosure;
  using mobject::ModelPort;
//  using lib::time::TimeSpan;
//  using lib::time::FSecs;
//  using lib::time::Time;
  using lib::time::FrameCnt;
  using lib::HashVal;
  
  
  /**
   * Abstract definition of the environment
   * hosting a given render activity (CalcStream).
   * Exposes all the operations necessary to adjust the
   * runtime behaviour of the render activity, like e.g.
   * re-scheduling with modified playback speed. Since the
   * CalcStream is an conceptual representation of "the rendering",
   * the actual engine implementation is kept opaque this way.
   */
  class RenderEnvironment
    {
    public:
      virtual ~RenderEnvironment() { }   ///< this is an interface
      
      virtual play::Timings& effectiveTimings()   =0;
      virtual Dispatcher&    getDispatcher()      =0;
    };
  
  
  /**
   * The active drive to keep the render process going -- implemented as a
   * job planning job, that repeatedly triggers itself again for the next
   * planning chunk. The RenderDrive is created and owned by the corresponding
   * CalcStream, and operates the job planning pipeline, backed by the dispatcher.
   * 
   * @todo 4/23 early DRAFT -- find out what this does and write type comment
   */
  class RenderDrive
    : public JobClosure
    {
      
      RenderEnvironment& engine_;
      
//    const ModelPort modelPort_;
//    const uint channel_;
      
      
      /* === JobClosure Interface === */
      
      JobKind
      getJobKind()  const
        {
          return META_JOB;
        }
      
      bool verify (Time, InvocationInstanceID)      const override;
      size_t hashOfInstance (InvocationInstanceID)  const override;
      InvocationInstanceID buildInstanceID(HashVal) const override;
      
      void invokeJobOperation (JobParameter)              override;
      void signalFailure (JobParameter, JobFailureReason) override;
      
      
      
      
    public:
      /**
       * @todo
       */
      RenderDrive (RenderEnvironment& renderEnvironment
                  ,ModelPort port, uint chan)
        : engine_{renderEnvironment}
        { }
      
      
      play::Timings const&
      getTimings()  const
        {
          return engine_.effectiveTimings();
        }
      
      /** create the "start trigger job"
       *  Scheduling this job will effectively get a calculation stream
       *  into active processing, since it causes the first chunk of job planning
       *  plus the automated scheduling of follow-up planning jobs. The relation
       *  to real (wall clock) time will be established when the returned job
       *  is actually invoked
       * @param startFrame where to begin rendering, relative to the nominal
       *        time grid implicitly given by the ModelPort to be pulled
       */
      Job prepareRenderPlanningFrom (FrameCnt startFrame);
      
      
    private:
      void performJobPlanningChunk(FrameCnt nextStartFrame);
      Job buildFollowUpJobFrom (TimeAnchor const& refPoint);
    };
  
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_RENDER_DRIVE_H*/
