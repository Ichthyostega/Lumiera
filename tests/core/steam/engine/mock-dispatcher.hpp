/*
  MOCK-DISPATCHER.hpp  -  test scaffolding to verify render job planning and dispatch

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

/** @file mock-dispatcher.hpp
 ** Mock data structures to support implementation testing of render job
 ** planning and frame dispatch. This specifically rigged test setup allows to
 ** investigate and verify designated functionality in isolation, without backing
 ** by the actual render engine and low-level-Model implementation.
 ** - a MockJob is a render Job, wired to a DummyFunctor, which does nothing,
 **   but records any invocation into an internal diagnostics Map.
 ** - MockJobTicket is a builder / adapter on top of the actual steam::engine::JobTicket,
 **   allowing to generate simple JobTicket instances with an embedded ExitNode and
 **   a (configurable) pipelineID. From this setup, »mock jobs« can be generated,
 **   which use the MockJob functor and thus record any invocation without performing
 **   actual work. The internal connection to the MockJobTicket can then be verified.
 ** - MockSegmentation is a mocked variant of the »Segmentation« datastructure,
 **   which forms the backbone of the Fixture and is the top-level attachment
 **   point for the »low-level-Model« (the render nodes network). It can be
 **   configured with a test specification of ExitNode(s) defined by a
 **   GenNode tree, and defining Segments of the timeline and prerequisites.
 ** - finally, the MockDispatcher combines all these facilities to emulate
 **   frame dispatch from the Fixture without actually using any data model.
 **   Similar to MockSegmentation, a GenNode-based specification is used.
 **
 ** @remark in spring 2023, this setup was created as a means to define and
 **   then build the actual implementation of frame dispatch and scheduling.
 ** @see MockSupport_test
 */


#ifndef STEAM_ENGINE_TEST_MOCK_DISPATCHER_H
#define STEAM_ENGINE_TEST_MOCK_DISPATCHER_H


#include "lib/test/test-helper.hpp"
#include "steam/play/dummy-play-connection.hpp"
#include "steam/fixture/node-graph-attachment.hpp"
#include "steam/fixture/segmentation.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/engine/dispatcher.hpp"
#include "steam/engine/job-ticket.hpp"
#include "vault/engine/job.h"
#include "vault/real-clock.hpp"
#include "lib/allocator-handle.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/linked-elements.hpp"
#include "lib/itertools.hpp"
#include "lib/depend.hpp"
#include "lib/util.hpp"

#include <tuple>
#include <map>


namespace steam  {
namespace engine {
namespace test   {
  
  using std::make_tuple;
  using lib::diff::GenNode;
  using lib::diff::MakeRec;
  using lib::time::TimeValue;
  using lib::time::Time;
  using lib::HashVal;
  using util::isnil;
  using util::isSameObject;
  using fixture::Segmentation;
  using vault::engine::Job;
  using vault::engine::JobClosure;
  
  
  
  
  
  /**
   * Mock setup for a render Job with NO action but built-in diagnostics.
   * Each invocation of such a MockJob will be logged internally
   * and can be investigated and verified afterwards.
   */
  class MockJob
    : public Job
    {
      static Job build(); ///< uses random job definition values
      static Job build (Time nominalTime, int additionalKey);
      
    public:
      MockJob()
        : Job{build()}
        { }
      
      MockJob (Time nominalTime, int additionalKey)
        : Job{build (nominalTime,additionalKey)}
        { }
      
      
      static bool was_invoked (Job const& job);
      static Time invocationTime (Job const& job);
      static Time invocationNominalTime (Job const& job);
      static int  invocationAdditionalKey (Job const& job);
      
      static bool isNopJob (Job const&);
      static JobClosure& getFunctor();
    };
  

  
  
  
  
  /**
   * Mock setup for a JobTicket to generate dummy render Job invocations.
   * Implemented as subclass, it provides a specification DSL for tests,
   * and is able to probe some otherwise opaque internals of JobTicket.
   * Beyond that, MockJobTicket has the same storage size; and behaves
   * like the regular JobTicket after construction -- but any Job
   * created by JobTicket::createJobFor(FrameCoord) will be wired
   * with the MockJob functor and can thus be related back to
   * the test specification setup.
   * @see JobPlanningSetup_test
   * @see DispatcherInterface_test
   */
  class MockJobTicket
    : private lib::AllocatorHandle<JobTicket>
    , public JobTicket
    {
      auto&
      allocator()
        {
          return static_cast<lib::AllocatorHandle<JobTicket>&> (*this);
        }
      
      /** provide a test specification wired to MockJob */
      static ExitNode
      defineSimpleSpec (HashVal seed = 1+rand())
        {
          return ExitNode{seed
                         ,ExitNodes{}
                         ,& MockJob::getFunctor()};
        }
      
    public:
      MockJobTicket()
        : JobTicket{defineSimpleSpec(), allocator()}
      { }
      
      MockJobTicket (HashVal seed)
        : JobTicket{defineSimpleSpec (seed), allocator()}
      { }
      
      
      /* ===== Diagnostics ===== */
      
      bool verify_associated (Job const&) const;
      static bool isAssociated (Job const&, JobTicket const&);
    };
  
  
  
  /**
   * Mock setup for a complete Segmentation to emulate the structure
   * of the actual fixture, without the need of building a low-level Model.
   * MockSegmentation instances can be instantiated directly within the
   * test, by passing a test specification in »GenNode« notation to the
   * constructor. This specification defines the segments to create
   * and allows to associate a marker number, which can later be
   * verified from the actual DummyClosure invocation.
   * - the ctor accepts a sequence of GenNode elements,
   *   each corresponding to a segment to created
   * - optionally, attributes "start" and "after" can be defined
   *   to provide the lib::time::Time values of segment start/end
   * - in addition, optionally a "mark" attribute can be defined;
   *   the given integer number will be "hidden" in the job instance
   *   hash, and can be [verified](\ref MockJob::invocationAdditionalKey)
   * - the _scope_ of each top-level GenNode may hold a sequence of
   *   nested nodes corresponding to _prerequisite_ JobTicket instances
   * - these can in turn hold further nested prerequisites, and so on
   * @see MockSetup_test::verify_MockSegmentation
   */
  class MockSegmentation
    : public Segmentation
    {
      
    public:
      MockSegmentation()
        : Segmentation{}
      { }
      
      MockSegmentation (std::initializer_list<GenNode> specs)
        : MockSegmentation{}
        {
          for (auto& spec : specs)
            {
              auto start = spec.retrieveAttribute<Time> ("start");
              auto after = spec.retrieveAttribute<Time> ("after");
              Segmentation::splitSplice (start, after
                                        ,ExitNodes{buildExitNodeFromSpec (spec)}
                                        );
            }
        }
      
      
      ExitNode
      buildExitNodeFromSpec (GenNode const& spec)
        {
          return ExitNode{buildSeed (spec)
                         ,buildPrerequisites (spec)
                         ,& MockJob::getFunctor()};
        }
      
      /** @internal helper for MockDispatcher */
      void duplicateExitNodeSpec (uint times);
      
      
    private: /* ======== Implementation: build fake ExitNodes from test specification ==== */
      
      HashVal
      buildSeed (GenNode const& spec)
        {
          auto seed = spec.retrieveAttribute<int> ("mark");
          return seed? HashVal(*seed) : HashVal(rand() % 1000);
        }
      
      ExitNodes
      buildPrerequisites (GenNode const& spec)
        {
          ExitNodes prerequisites;
          for (auto& child : spec.getChildren())
            prerequisites.emplace_back (
              buildExitNodeFromSpec (child));
          return prerequisites;
        }
    };
  
  
  
  
  /**
   * This is some trickery to allow handling of multiple ModelPort(s) in MockDispatcher;
   * actually the code using this mock setup does not need any elaborate differentiation
   * of the ExitNodes structure per port, thus the first entry of the existing configuration
   * is just duplicated for the given number of further ModelPorts.
   * @warning this manipulation must be done prior to generating any JobTicket
   */
  inline void
  MockSegmentation::duplicateExitNodeSpec (uint times)
    {
      using Spec = fixture::NodeGraphAttachment;
      
      Segmentation::adaptSpecification ([times](Spec const& spec)
                                                {
                                                  return Spec{ExitNodes{times, spec[0]}};
                                                });
    }
  
  
  /**
   * verify the given job instance was actually generated from this JobTicket.
   * @remark this test support function relies on some specific rigging,
   *         which typically is prepared by setup of a MockJobTicket.
   */
  inline bool
  MockJobTicket::verify_associated (Job const& job)  const
  {
    JobFunctor& functor = dynamic_cast<JobFunctor&> (static_cast<JobClosure&> (*job.jobClosure));
    Time nominalTime {TimeValue{job.parameter.nominalTime}};
    InvocationInstanceID const& invoKey = job.parameter.invoKey;
    return this->isValid()
       and this->verifyInstance(functor, invoKey, nominalTime);
  }
  
  /**
   * convenience shortcut to perform [this test](\ref MockJobTicket::verify_associated)
   * on arbitrary JobTicket and Job instances.
   * @warning a positive test result however relies on some casting trickery and there is no
   *    guarantee this test works if the JobTicket was not created from this mock framework.
   */
  inline bool
  MockJobTicket::isAssociated (Job const& job, JobTicket const& ticket)
  {                                    // should work always, since storage is the same
    MockJobTicket const& backdoor = static_cast<MockJobTicket const&> (ticket);
    return backdoor.verify_associated (job);
  }
  
  
  
  namespace { // used internally by MockDispatcher....
    using play::test::ModelPorts;
    using play::test::PlayTestFrames_Strategy;
    
    using DummyPlaybackSetup = play::test::DummyPlayConnection<PlayTestFrames_Strategy>;
  }
  
  
  
  /**
   * A mocked frame Dispatcher setup without any backing model.
   * Instantiating such a MockDispatcher will automatically create some fake
   * model structures and some ModelPort and DisplaySink handles (and thereby
   * push aside and shadow any existing ModelPort registry).
   * 
   * The configuration is similar to MockSegmentation, using a test spec
   * given as GenNode-tree to define Segments of the timeline and possibly
   * pipeline-IDs and prerequisites. One notable difference is that here
   * the default ctor always creates a single Segment covering the whole
   * time axis, and that the ExitNode specification is automatically
   * duplicated for all faked ModelPort(s).
   */
  class MockDispatcher
    : public Dispatcher
    {
      
      DummyPlaybackSetup dummySetup_;
      MockSegmentation mockSeg_;
      
      using PortIdxMap = std::map<ModelPort, size_t>;
      
      const PortIdxMap portIdx_;
      
    public:
      /* == mock implementation of the Dispatcher interface == */
      
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 : likely to become obsolete
      FrameCoord
      locateRelative (FrameCoord const&, FrameCnt frameOffset)  override
        {
          UNIMPLEMENTED ("dummy implementation of the core dispatch operation");
        }
      
      bool
      isEndOfChunk (FrameCnt, ModelPort port)  override
        {
          UNIMPLEMENTED ("determine when to finish a planning chunk");
        }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 : likely to become obsolete
      
      size_t
      resolveModelPort (ModelPort modelPort)  override
        {
          auto entry = portIdx_.find(modelPort);
          if (entry == portIdx_.end())
            throw error::Logic{"Invalid ModelPort for this Dispatcher"};
          else
            return entry->second;
        }
      
      
      JobTicket&
      accessJobTicket (size_t portIDX, TimeValue nominalTime)  override
        {
          auto& seg = mockSeg_[nominalTime];
          return seg.jobTicket(portIDX);
        }
      
      
    public:
      MockDispatcher()
        : dummySetup_{}
        , mockSeg_{MakeRec().genNode()}                 // Node: generate a single active Segment to cover all
        , portIdx_{buildPortIndex()}
        {
          mockSeg_.duplicateExitNodeSpec(portIdx_.size());
        }
      
      MockDispatcher (std::initializer_list<GenNode> specs)
        : dummySetup_{}
        , mockSeg_(specs)
        , portIdx_{buildPortIndex()}
        {
          mockSeg_.duplicateExitNodeSpec(portIdx_.size());
        }
      
      
      ModelPort
      provideMockModelPort()
        {
          ModelPorts mockModelPorts = dummySetup_.getAllModelPorts();
          return *mockModelPorts;  // using just the first dummy port
        }
      
      /**
       * The faked builder/playback setup provides some preconfigured ModelPort and
       * corresponding DataSink handles. These are stored into a dummy registry and only available
       * during the lifetime of the DummyPlaybackSetup instance.
       * @param index number of the distinct port / connection
       * @return a `std::pair<ModelPort,DataSink>`
       * @warning as of 5/2023, there are two preconfigured "slots",
       *          and they are not usable in any way other then referring to their identity
       */
      play::test::DummyOutputLink
      getDummyConnection(uint index)
        {
          return dummySetup_.getModelPort (index);
        }
      
      /**
       * Test support: verify the given Job is consistent with this Dispatcher.
       */
      bool verify(Job const& job, ModelPort const& port, play::DataSink const& sink)
        {
          if (not dummySetup_.isSupported (port, sink)) return false;
          
          TimeValue nominalTime{job.parameter.nominalTime};
          size_t portIDX = resolveModelPort (port);
          JobTicket& ticket = accessJobTicket (portIDX, nominalTime);
          return isnil (ticket)? MockJob::isNopJob (job)
                               : MockJobTicket::isAssociated (job, ticket);
        }
      
    private:
      PortIdxMap
      buildPortIndex()
        {
          PortIdxMap newIndex;
          uint i{0};
          for (auto it=dummySetup_.getAllModelPorts()
              ; bool{it}
              ; ++it, ++i
              )
            newIndex[*it] = i;
          
          return newIndex;
        }
    };
  
  
}}} // namespace steam::engine::test
#endif /*STEAM_ENGINE_TEST_MOCK_DISPATCHER_H*/
