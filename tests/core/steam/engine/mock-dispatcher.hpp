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
 ** planning and frame dispatch. Together with dummy-job.hpp, this provides
 ** a specifically rigged test setup, allowing to investigate and verify
 ** designated functionality in isolation, without backing by the actual
 ** render engine implementation.
 ** - the MockDispatcherTable emulates the frame dispatch from the Fixture
 ** - MockSegmentation is a mocked variant of the »Segmentation« datastructure,
 **   which forms the backbone of the Fixture and is the top-level attachment
 **   point for the »low-level-Model« (the render nodes network)
 ** - MockJobTicket is a builder / adapter on top of the actual steam::engine::JobTicket,
 **   allowing to generate a complete rigged MockSegmentation setup from a generic
 **   test specification written as nested lib::diff::GenNode elements. From this
 **   setup, »mock jobs« can be generated, which use the DummyJob functor and
 **   just record any invocation without performing actual work.
 ** @remark in spring 2023, this setup was created as a means to define and
 **   then build the actual implementation of frame dispatch and scheduling.
 ** @see MockSupport_test
 */


#ifndef STEAM_ENGINE_TEST_MOCK_DISPATCHER_H
#define STEAM_ENGINE_TEST_MOCK_DISPATCHER_H


#include "lib/test/test-helper.hpp"
#include "steam/fixture/segmentation.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/engine/dispatcher.hpp"
#include "steam/engine/job-ticket.hpp"
#include "vault/engine/job.h"
#include "vault/engine/dummy-job.hpp"
#include "vault/real-clock.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/linked-elements.hpp"
#include "lib/itertools.hpp"
#include "lib/depend.hpp"

#include <tuple>
#include <list>


namespace steam  {
namespace engine {
namespace test   {
  
  using std::make_tuple;
  using lib::diff::GenNode;
  using lib::diff::MakeRec;
  using lib::time::TimeValue;
  using lib::time::Time;
  using lib::HashVal;
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1294 : organisation of namespaces / includes??
  using fixture::Segmentation;
  
  namespace { // used internally
    
//    using play::PlayTestFrames_Strategy;
//    using play::ModelPorts;
    using vault::engine::JobClosure;
    using vault::engine::JobParameter;
    using vault::engine::DummyJob;
    
//    typedef play::DummyPlayConnection<play::PlayTestFrames_Strategy> DummyPlaybackSetup;
    
    
    class MockDispatcherTable
      : public Dispatcher
      {
        
//        DummyPlaybackSetup dummySetup_;
        
        
        /* == mock Dispatcher implementation == */
        
        FrameCoord
        locateRelative (FrameCoord const&, FrameCnt frameOffset)
          {
            UNIMPLEMENTED ("dummy implementation of the core dispatch operation");
          }
        
        bool
        isEndOfChunk (FrameCnt, ModelPort port)
          {
            UNIMPLEMENTED ("determine when to finish a planning chunk");
          }

        JobTicket&
        accessJobTicket (ModelPort, TimeValue nominalTime)
          {
            UNIMPLEMENTED ("dummy implementation of the model backbone / segmentation");
          }
        
      public:
        
        ModelPort
        provideMockModelPort()
          {
//            ModelPorts mockModelPorts = dummySetup_.provide_testModelPorts();
//            return *mockModelPorts;  // using just the first dummy port
          }
      };
    
    /// @deprecated this setup is confusing and dangerous (instance identity is ambiguous)
    lib::Depend<MockDispatcherTable> mockDispatcher;
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1221
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1221
    
    
    
    
    /* ===== specify a mock JobTicket setup for tests ===== */
    
    template<class IT>
    inline auto
    defineSpec (HashVal seed, IT&& prereq)
    {
      using SpecTuple = std::tuple<JobFunctor&, HashVal, IT>;
      return lib::singleValIterator(                            /////////////////////////////////////////////TICKET #1297 : multiplicity per channel will be removed here
                 SpecTuple(DummyJob::getFunctor()
                          , seed
                          , std::forward<IT> (prereq)));
    }
    
    inline auto
    defineSimpleSpec (HashVal seed =0)
    {
      return defineSpec (seed, lib::nilIterator<JobTicket&>());
    }
    
  }//(End)internal test helpers....
    
  
  
  
  /**
   * Mock setup for a JobTicket to generate DummyJob invocations.
   * Implemented as subclass, it provides a specification DSL for tests,
   * and is able to probe some otherwise opaque internals of JobTicket.
   * Beyond that, MockJobTicket has the same storage size; and behaves
   * like the regular JobTicket after construction -- but any Job
   * created by JobTicket::createJobFor(FrameCoord) will be wired
   * with the DummyJob functor and can thus be related back to
   * the test specification setup.
   * @see JobPlanningSetup_test
   * @see DispatcherInterface_test
   */
  class MockJobTicket
    : public JobTicket
    {
    public:
      MockJobTicket()
        : JobTicket{defineSimpleSpec()}
      { }
      
      MockJobTicket (HashVal seed)
        : JobTicket{defineSimpleSpec (seed)}
      { }
      
      template<class IT>
      MockJobTicket (HashVal seed, IT&& prereq)
        : JobTicket{defineSpec (seed, std::forward<IT> (prereq))}
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
   * verified from the actual DummyJob invocation.
   * - the ctor accepts a sequence of GenNode elements,
   *   each corresponding to a segment to created
   * - optionally, attributes "start" and "after" can be defined
   *   to provide the lib::time::Time values of segment start/end
   * - in addition, optionally a "mark" attribute can be defined;
   *   the given integer number will be "hidden" in the job instance
   *   hash, and can be [verified](\ref DummyJob::invocationAdditionalKey)
   * - the _scope_ of each top-level GenNode may hold a sequence of
   *   nested nodes corresponding to _prerequisite_ JobTicket instances
   * - these can in turn hold further nested prerequisites, and so on
   * @see MockSetup_test::verify_MockSegmentation
   */
  class MockSegmentation
    : public Segmentation
    {
      // simulated allocator;
      // must be able to handle re-entrant allocations
      std::list<MockJobTicket> tickets_;
      
    public:
      MockSegmentation()
        : Segmentation{}
        , tickets_{}
      { }
      
      MockSegmentation (std::initializer_list<GenNode> specs)
        : MockSegmentation{}
        {
          for (auto& spec : specs)
            {
              JobTicket& newTicket = buildTicketFromSpec (spec);
              
              auto start = spec.retrieveAttribute<Time> ("start");
              auto after = spec.retrieveAttribute<Time> ("after");
              Segmentation::splitSplice (start, after, &newTicket);
            }
        }
      
      
    private: /* ======== Implementation: build mock JobTickes from test specification ==== */
      
      HashVal
      buildSeed (GenNode const& spec)
        {
          auto seed = spec.retrieveAttribute<int> ("mark");
          return seed? HashVal(*seed) : HashVal(rand() % 1000);
        }
      
      auto
      buildPrerequisites (GenNode const& spec)
        {
          return lib::transformIterator (spec.getChildren()
                                        ,[this](GenNode const& childSpec) -> JobTicket&
                                              {
                                                return buildTicketFromSpec (childSpec);
                                              });
        }
      
      JobTicket&
      buildTicketFromSpec (GenNode const& spec)
        {
          return tickets_.emplace_back (buildSeed(spec)
                                       ,buildPrerequisites(spec));
        }                              // Warning: re-entrant invocation of emplace_back
    };
  
  
  
  
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
  
  
}}} // namespace steam::engine::test
#endif /*STEAM_ENGINE_TEST_MOCK_DISPATCHER_H*/