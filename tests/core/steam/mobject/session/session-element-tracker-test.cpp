/*
  SessionElementTracker(Test)  -  check the facility to track and expose selected model elements

   Copyright (C)
     2008, 2010,      Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file session-element-tracker-test.cpp
 ** unit test \ref SessionElementTracker_test
 */


#include "lib/test/run.hpp"

#include "lib/element-tracker.hpp"

#include "steam/assetmanager.hpp"
#include "steam/mobject/session.hpp"
#include "steam/asset/timeline.hpp"
#include "steam/asset/sequence.hpp"
#include "common/query.hpp"
#include "lib/p.hpp"



namespace steam    {
namespace mobject {
namespace session {
namespace test    {
  
  namespace { // yet another accounting dummy
    
    uint instance = 0;
    int  checksum = 0;
    
    using lib::AutoRegistered;
    
    /**
     * Test Dummy: to be created through the inherited static #create(),
     * managed by smart-ptr. Any Dummy instance gets automatically registered
     * for tracking, and will be deregistered by invoking #unlink().
     * The link to the actual registration service has to be established at
     * runtime once, by calling #establishRegistryLink or #setRegistryInstance
     */
    struct Dummy
      : AutoRegistered<Dummy>
      {
        const uint id_;
        
        Dummy()
          : id_(++instance)
          {
            checksum += id_;
          }
         // to be created by factory...
        friend class AutoRegistered<Dummy>;
        
      public:
        void
        detach() // demonstrates how to hook into the cleanup-operation
          {
            AutoRegistered<Dummy>::detach();
            checksum -= id_;
          }
      };
      
    bool
    operator== (Dummy const& d1, Dummy const& d2)
    {
      return util::isSameObject (d1, d2);
    }
    
  } // (End) test dummy and helper
  
  
  
  using lib::P;
  using lumiera::Query;
  using asset::Timeline;
  using asset::PTimeline;
  using asset::AssetManager;
  
  
  /****************************************************************************//**
   * @test verify the tracking of special session/model elements, to be exposed
   *       through an self-contained interface module on the session API.
   *       The basic element-tracking mechanism uses a simple (vector based)
   *       registry, which stores a smart-ptr. Thus the elements need to be
   *       created by a factory. In case of Timeline / Sequence, the
   *       asset::StructFactory will take on this role. The integration test
   *       creates a Timeline (facade asset) and verifies proper registration
   *       and deregistration.
   *       
   * @see  timeline-sequence-handling-test.cpp
   * @see  session-interface-modules.hpp
   * @see  ref-array-test.cpp
   */
  class SessionElementTracker_test : public Test
    {
      virtual void
      run (Arg) 
        {
          verify_trackingMechanism();
          verify_integration();
        }
      
      
      void
      verify_trackingMechanism()
        {
          instance = 0;
          checksum = 0;
          {
            typedef Dummy AutoRegisteringDummy;
            typedef P<AutoRegisteringDummy> PDummy;
            typedef lib::ElementTracker<Dummy> DummyRegistry;
            
            DummyRegistry trackedDummies;
            
            CHECK (0 == checksum);
            CHECK (0 == trackedDummies.size());
            
            AutoRegisteringDummy::setRegistryInstance (trackedDummies);
            PDummy dummy1 = AutoRegisteringDummy::create();
            PDummy dummy2 = AutoRegisteringDummy::create();
            
            CHECK (2 == trackedDummies.size());
            CHECK (dummy1 == trackedDummies[0]);
            CHECK (dummy2 == trackedDummies[1]);
            
            PDummy dummy3 = AutoRegisteringDummy::create();
            CHECK (3 == trackedDummies.size());
            CHECK (dummy3 == trackedDummies[2]);
            
            CHECK (1+2+3 == checksum);
            
            dummy2->detach();
            CHECK (1 + 3 == checksum);
            CHECK (2 == trackedDummies.size());
            CHECK (dummy1 == trackedDummies[0]);
            CHECK (dummy3 == trackedDummies[1]);
            
            CHECK (1 == dummy2.use_count());
            CHECK (2 == dummy1.use_count());
            CHECK (2 == dummy3.use_count());
            
            // deliberately discard our reference, 
            // so the only remaining ref is within the registry
            dummy1.reset();
            dummy3.reset();
            CHECK (!dummy1);
            CHECK ( dummy2);
            CHECK (!dummy3);
            CHECK (1 == trackedDummies[0].use_count());
            CHECK (1 == trackedDummies[1].use_count());
            CHECK (1 + 3 == checksum);
            
            // now the tracker goes out of scope...
          }
          CHECK (0 == checksum); // ...remaining elements have been unlinked 
        }
      
      
      void
      verify_integration()
        {
          Session::current.reset();
          CHECK (Session::current.isUp());
          
          PSess sess = Session::current;
          CHECK (sess->isValid());
          
          uint num_timelines = sess->timelines.size();
          CHECK (0 < num_timelines);
          
          PTimeline specialTimeline (asset::Struct::retrieve.newInstance<Timeline> ("testical"));
          CHECK (specialTimeline);
          CHECK (num_timelines + 1 == sess->timelines.size());
          CHECK (specialTimeline == sess->timelines[num_timelines]);    // got appended at the end of the tracking table
          CHECK (specialTimeline.use_count() == 3);                     // we, the AssetManager and the session
          
          PTimeline anotherTimeline (asset::Struct::retrieve.newInstance<Timeline>());
          CHECK (num_timelines + 2 == sess->timelines.size());
          CHECK (specialTimeline == sess->timelines[num_timelines]);
          CHECK (anotherTimeline == sess->timelines[num_timelines+1]);  // new one got appended at the end
          
          AssetManager& assetM (AssetManager::instance());
          CHECK (assetM.known (specialTimeline->getID()));
          assetM.remove (specialTimeline->getID());                   //////////////TICKET #550  modalities of Timeline/Sequence deletion
          CHECK (!assetM.known (specialTimeline->getID()));
          
          CHECK (num_timelines + 1 == sess->timelines.size());
          CHECK (anotherTimeline == sess->timelines[num_timelines]);    // moved to the previous slot
          CHECK (specialTimeline.use_count() == 1);                     // we're holding the last reference
          
          verify_cleanup (anotherTimeline);
        }
      
      
      /** @test ensure the asset cleanup doesn't interfere with session shutdown
       */
      void
      verify_cleanup (PTimeline const& aTimeline_in_session)
        {
          CHECK (1 < aTimeline_in_session.use_count(), "test object should still be attached to session");
          Session::current.reset();
          CHECK (1 == aTimeline_in_session.use_count(), "session reset should have de-registered the test object");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SessionElementTracker_test, "unit session");
  
  
  
}}}} // namespace steam::mobject::session::test
