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
 ** @todo as of 4/2023 a complete rework of the Dispatcher is underway //////////////////////////////////////TICKET #1275
 */


#ifndef STEAM_ENGINE_DISPATCHER_H
#define STEAM_ENGINE_DISPATCHER_H

#include "steam/common.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/engine/time-anchor.hpp"
#include "steam/engine/frame-coord.hpp"
#include "steam/engine/job-ticket.hpp"
#include "steam/engine/job-planning.hpp"
#include "steam/play/output-slot.hpp"
#include "lib/iter-tree-explorer.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/nocopy.hpp"

#include <functional>
#include <utility>


namespace steam {
namespace engine {
  
  using std::move;
  using std::declval;
  using std::function;
  using mobject::ModelPort;
  using play::Timings;
  using play::DataSink;
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
   * # usage considerations
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
   * @todo 6/23 API is remoulded from ground up (»Playback Vertical Slice« integration effort)
   */
  class Dispatcher
    : public FrameLocator
    {
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1301 : obsolete      
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1301 : obsolete      
    public: //////////////////////////////////////////////////////////////////OOO TODO not public; need a way to pick up the result type
      struct PipeFrameTick;
      struct PipeSelector;
      struct PipeExpander;
      struct PipePlanner;
      
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1301 : obsolete      
      using FrameNrIter = lib::NumIter<FrameCnt>;
      
      struct PipelineBuilder
        : FrameNrIter
        {
          Dispatcher* dispatcher;
          Timings timings;
          ModelPort port;
          DataSink sink;
          
          auto
          timeRange (Time start, Time after)
            {
              auto frame = [&](Time t){ return timings.getBreakPointAfter(t); };
              auto reset = [&](auto i){ static_cast<FrameNrIter&>(*this) = i; };
              
              reset (lib::eachNum (frame(start), frame(after)));
              return lib::treeExplore (move(*this))
                         .transform ([&](FrameCnt frameNr) -> TimeVar          //////////////////////////////TICKET #1261 : transform-iterator unable to handle immutable time
                                     {
                                       return timings.getFrameStartAt (frameNr);
                                     });
            }
        };
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1301 : obsolete      
      
      
    public:
      virtual ~Dispatcher();  ///< this is an interface
      
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1301 : obsolete      
      JobBuilder onCalcStream (ModelPort modelPort, uint channel);
      
      PipelineBuilder forCalcStream(Timings timings, ModelPort port, DataSink sink)
        {
          return PipelineBuilder{FrameNrIter(), this, timings, port, sink};
        }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1301 : obsolete      
      PipeFrameTick forCalcStream (Timings timings);
      
      
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
      
      
      /** Builder Function: start frame sequence */
      auto
      timeRange (Time start, Time after)
        {
          stopPoint = after;
          frameNr = timings.getBreakPointAfter(start);
          currPoint = timings.getFrameStartAt (frameNr);
          
          // start iterator pipeline based on this as »state core«
          return lib::treeExplore (move(*this));
        }
      
    };

    
  inline Dispatcher::PipeFrameTick
  Dispatcher::forCalcStream(Timings timings)
  {
    return PipeFrameTick{this, timings};
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////OOO need better solution for type rebinding
  using Builder = decltype( declval<Dispatcher::PipeFrameTick>().timeRange (declval<Time>(), declval<Time>()));
  
  /**
   * Job-planning Step-2: select data feed connection between ModelPort
   * and DataSink; this entails to select the actually active Node pipeline
   */
  struct Dispatcher::PipeSelector
    : Builder
    {
      
    };
  
  
  /**
   * Job-planning Step-3: monadic depth-first exploration of Prerequisites
   */
  struct Dispatcher::PipeExpander
    {};
  
  
  /**
   * Job-planning Step-4: collect the complete planning context and determine time frame
   */
  struct Dispatcher::PipePlanner
    {};
  
  


  
}} // namespace steam::engine
#endif
