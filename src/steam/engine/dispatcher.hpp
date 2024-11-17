/*
  DISPATCHER.hpp  -  translating calculation streams into frame jobs

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file dispatcher.hpp
 ** Service abstraction within the render engine for generating render jobs.
 ** On interface level, the render engine uses the notion of a [calculation stream](\ref CalcStream)
 ** to represent an ongoing rendering process. Within the _implementation_ of such a process,
 ** the Dispatcher is responsible for transforming the generic setup of such a calculation stream
 ** into a sequence of concrete jobs, anchored at some distinct point in time.
 ** 
 ** @todo as of 4/2023 a complete rework of the Dispatcher is underway //////////////////////////////////////TICKET #1275
 */


#ifndef STEAM_ENGINE_DISPATCHER_H
#define STEAM_ENGINE_DISPATCHER_H

#include "steam/common.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/engine/job-ticket.hpp"
#include "steam/engine/job-planning.hpp"
#include "steam/play/timings.hpp"
#include "steam/play/output-slot.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/nocopy.hpp"

#include <functional>
#include <utility>
#include <tuple>


namespace steam {
namespace engine {
  
  using std::move;
  using std::make_pair;
  using mobject::ModelPort;
  using play::Timings;
  using play::DataSink;
  using lib::time::TimeValue;
  using lib::time::TimeSpan;
  using lib::time::FrameCnt;
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
   * # usage considerations
   * the asynchronous and ongoing nature of the render process mandates to avoid a central
   * instance for operating this planning process. Instead, together with each chunk of
   * planned jobs we generate a continuation job, which -- on activation -- will pick up
   * the planning of the next chunk. The Dispatcher interface was shaped especially to
   * support this process, with a local PlanningPipeline for use within the RenderDrive
   * incorporated into each CalcStream. All the complexities of actually planning the
   * jobs are hidden within this pipeline, which, for the purpose of dispatching a
   * series of jobs just looks like a sequence of job descriptors
   * 
   * @todo 6/23 API is remoulded from ground up (»Playback Vertical Slice« integration effort)
   */
  class Dispatcher
    : util::NonCopyable
    {
      
      struct PipeFrameTick;
      
      template<class IT>
      struct PipelineBuilder;
      
      
    public:
      virtual ~Dispatcher();  ///< this is an interface
      
      
      /**
       * Start a builder sequence to assemble a job-planning pipeline, backed by this Dispatcher.
       * @param timings the frame-grid and further specs to use for a specific CalcStream
       * @remark the given #PipelineBuilder object shall be used to supply the further parameters,
       *         thereby qualifying the actual planning steps necessary to arrive at a sequence
       *         of Jobs; these can be retrieved from the resulting iterator, ready for dispatch.
       */
      PipelineBuilder<PipeFrameTick> forCalcStream (Timings timings);
      
      /**
       * A complete job-planning pipeline: this »Lumiera Forward Iterator« drives the actual
       * job-planning process on-demand. At the end of the pipeline, a sequence of render Jobs
       * appears, ready for hand-over to the Scheduler. The PlanningPipeline itself wraps a
       * »state-core« holding the current planning state; for operation it should be placed
       * at a fixed location (typically in the CalcStream) and not duplicated, since this
       * internal state ensures the generation of a distinct and unique sequence of Jobs
       * for one specific data feed. During the lifetime of this iterator, the backing
       * data structures in the Fixture must be kept alive and fixed in memory.
       */
      template<class IT>
      class PlanningPipeline;
      
      /** translate a generic ModelPort spec into the specific index number
       *  applicable at the Timeline referred-to by this Dispatcher
       * @throws error::Logic if the given ModelPort is not associated
       * @remark assuming that any Play-Process about to be started is always
       *         built on top of an established OutputConnection, and thus relies
       *         on a model link predetermined by the Builder. In other words,
       *         when a Timeline can be performed to this output, then a suitable
       *         ModelPort was derived on the Builder run triggered by preparing
       *         this specific output possibility. Thus it's an application logic
       *         error if attempting to dispatch on a unknown ModelPort.
       */
      virtual size_t resolveModelPort (ModelPort)                                  =0;
      
      /**
       * Core Dispatcher operation: locate the appropriate Segment and
       * retrieve/derive  a »blueprint« for render job generation. 
       * @param portIDX index-number for a ModelPort [as resolved](\ref #resolveModelPort)
       * @param nominalTime time of the frame to calculate, relative to Timeline origin
       */
      virtual JobTicket& getJobTicketFor (size_t portIDX, TimeValue nominalTime)   =0;
      
      /** Convenience shortcut for tests: JobTicket ⟼ Job */
      Job createJobFor (size_t portIDX, TimeValue nominalTime);
    };
  
  
  
  
  /* ======== Steps of the Job-planning Pipeline ======== */
  
  /**
   * Job-planning Step-1: establish a sequence of frame start times
   */
  struct Dispatcher::PipeFrameTick
    {
      Dispatcher * const dispatcher;
      const Timings      timings;
      
      TimeVar currPoint{Time::NEVER};
      TimeVar stopPoint{Time::NEVER};
      FrameCnt frameNr{0};
      
      
      
      /* === state protocol API for IterStateWrapper === */
      bool
      checkPoint()  const
        {
          return currPoint < stopPoint;
        }
      
      TimeVar const&
      yield()  const
        {
          return currPoint;
        }
      
      void
      iterNext()
        {
          ++frameNr;
          currPoint = timings.getFrameStartAt (frameNr);
        }
      
    protected:
      void
      activate (Time start, Time after)
        {
          stopPoint = after;
          frameNr = timings.getBreakPointAfter(start);
          currPoint = timings.getFrameStartAt (frameNr);
        }
    };
  
  
  
  
  /**
   * A Builder wrapper, allowing to build a Job-planning pipeline
   * step by step, while supplying contextual information from the CalcStream.
   * @remark This builder is created from \ref Dispatcher::forCalcStream, and thus
   *         internally wired back to the `Dispatcher` implementation, to access the
   *         Fixture and low-level-Model data-structures to back generated render Jobs.
   *         Client code is expected to invoke all builder functions consecutively,
   *         and then place the result into the CalcStream for generating render Jobs.
   */
  template<class SRC>
  struct Dispatcher::PipelineBuilder
    : SRC
    {
      
      /**
       * Builder: start frame sequence
       */
      auto
      timeRange (Time start, Time after)
        {
          SRC::activate (start,after);
          return buildPipeline (lib::explore (move(*this)));
        }                      // expected next to invoke pullFrom(port,sink)
      

      
      /**
       *  Builder: connect to the JobTicket defining the actual processing
       *  for the nominal time of this frame and the given ModelPort
       */
      auto
      pullFrom (mobject::ModelPort port)
        {
          size_t portIDX = SRC::dispatcher->resolveModelPort(port);
          return buildPipeline (
                   SRC::transform(
                     [portIDX](PipeFrameTick& core)
                      {
                        return JobPlanning{core.dispatcher->getJobTicketFor(portIDX, core.currPoint)
                                          ,core.currPoint
                                          ,core.frameNr};
                      }));
        }
      
      
      /**
       * Builder: cause a exhaustive depth-first search
       * to recursively discover all prerequisites of each
       * top-level JobTicket
       */
      auto
      expandPrerequisites()
        {
          return buildPipeline (
                   SRC::expandAll(
                     [](JobPlanning& currentLevel)
                      {
                        return currentLevel.buildDependencyPlanning();
                      }));
        }
      
      /**
       * Terminal builder: setup processing feed to the given DataSink.
       * @return Iterator to pull a sequence of render jobs, ready for processing
       * @todo 6/2023 more akin to a place holder, since it is not clear what must
       *       be done with the sink handle(s). Moreover, a Transformer returning
       *       a reference to the JobPlanning (deeper down in the pipeline) causes
       *       yet another unnecessary indirection; if the Transformer wasn't there,
       *       the resulting iterator would directly expose the result of the expander,
       *       which is the actual JobPlanning (but then we'd need another way how to
       *       wire in the DataSink...)
       */
      auto
      feedTo (play::DataSink& sink)
        {
          return terminatePipeline (
                   SRC::transform(
                     [sink](JobPlanning& currentLevel) -> JobPlanning&
                      {
                                                /////////////////////////////////////////////////////////////TICKET #1308 : something must be done with SinkHandle here, but it is not clear yet, what
                        return currentLevel;
                      })
                   .asIterator());
        }
      
      
    protected:
      /** @internal type rebinding helper to move the given tree-Explorer pipeline
       *            and layer a new PipelineBuilder subclass on top.
       *  @note IterExplorer itself is defined in a way to always strip away any existing
       *            top-level IterExplorer, then add a new processing layer and finally
       *            place a new IterExplorer layer on top. Taken together, this setup will
       *            *slice away* the actual PipelineBuilder layer, move the resulting pipeline
       *            into the next building step and finally produce a cleanly linked processing
       *            pipeline without any interspersed builders. Yet still, partially constructed
       *            pipelines are valid iterators and can be unit tested in isolation.
       */
      template<class PIP>
      PipelineBuilder<PIP>
      buildPipeline(PIP&& treeExplorer)
        {
          return PipelineBuilder<PIP> {move (treeExplorer)};
        }
      
      template<class PIP>
      PlanningPipeline<PIP>
      terminatePipeline(PIP&& pipelineIterator)
        {
          return PlanningPipeline<PIP> {move (pipelineIterator)};
        }
    };
  
  
    /**
     * @remark this is a convenience front-end layered on top of the job-planning pipeline
     *         - is a »Lumiera Forward Iterator« (bool-check, increment by `++`)
     *         - exposes the JobPlanning for the current Job when dereferenced
     */
  template<class PIP>
  struct Dispatcher::PlanningPipeline
    : PIP
    {
      FrameCnt
      currFrameNr()  const
        {
          return PIP::frameNr;
        }
      
      bool
      isBefore (Time breakPoint)  const
        {
          return currFrameNr() < PIP::timings.getBreakPointAfter(breakPoint);
        }
      
      Job
      buildJob()
        {
          return PIP::operator->()->buildJob();
        }
      
      Time
      determineDeadline()
        {
          return PIP::operator->()->determineDeadline (PIP::timings);
        }
    };
  
  
  
  inline Dispatcher::PipelineBuilder<Dispatcher::PipeFrameTick>
  Dispatcher::forCalcStream (Timings timings)
  {
    return PipelineBuilder<PipeFrameTick> {this, timings};
  }
  
  inline Job
  Dispatcher::createJobFor (size_t portIDX, TimeValue nominalTime)
  {
    return getJobTicketFor (portIDX, nominalTime)
             .createJobFor (Time{nominalTime});
  }


  
}} // namespace steam::engine
#endif
