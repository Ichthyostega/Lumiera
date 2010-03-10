/*
  SessionElementTracker(Test)  -  check the facility to track and expose selected model elements
 
  Copyright (C)         Lumiera.org
    2008-2010,          Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


#include "lib/test/run.hpp"
//#include "proc/mobject/session.hpp"
//#include "proc/mobject/mobject-ref.hpp"
//#include "proc/mobject/session/binding.hpp"
//#include "proc/mobject/session/fixture.hpp"
//#include "proc/assetmanager.hpp"
#include "proc/mobject/session/session-interface-modules.hpp"
#include "proc/asset/timeline.hpp"
#include "proc/asset/sequence.hpp"
#include "lib/p.hpp"
//#include "proc/asset/pipe.hpp"
//#include "lib/lumitime.hpp"
//#include "lib/query.hpp"
//#include "lib/util.hpp"

//#include <iostream>

//using util::isSameObject;
//using util::contains;
//using util::isnil;
//using std::string;
//using std::cout;


namespace mobject {
namespace session {
namespace test    {
  
  namespace { // yet another accounting dummy
    
    int checksum = 0;
    
    class AutoRegisteringDummy;
    
    typedef lumiera::P<AutoRegisteringDummy> PDummy;
    
    class AutoRegisteringDummy
      {
        const uint id_;
        
      public:
        AutoRegisteringDummy()
          : id_(++checksum)
          {
            TODO ("cause the registration");
          }
        
        void
        unlink()
          {
            TODO ("cause the deregistration");
            checksum -= id_;
          }
        
        static PDummy create();
      };
    
    inline PDummy
    AutoRegisteringDummy::create()
    {
      return PDummy (new AutoRegisteringDummy);
    }
    
  }
//  using proc_interface::AssetManager;
//  using proc_interface::PAsset;
  
  using asset::PTimeline;
  using asset::PSequence;
//  using asset::RBinding;
//  using asset::RTrack;
//  using asset::Pipe;
  
//  using lumiera::Query;
//  using lumiera::Time;
  
  
  /********************************************************************************
   * @test verify the tracking of special session/model elements, to be exposed
   *       through an self-contained interface module on the session API.
   *       
   * @todo WIP-WIP-WIP
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
          checksum = 0;
          {
            impl::ElementTracker<AutoRegisteringDummy> trackedDummies;
            
            CHECK (0 == checksum);
            CHECK (0 == trackedDummies.size());
            
            PDummy dummy1 = AutoRegisteringDummy::create();
            PDummy dummy2 = AutoRegisteringDummy::create();
            
            CHECK (2 == trackedDummies.size());
            CHECK (dummy1 == trackedDummies[0]);
            CHECK (dummy2 == trackedDummies[1]);
            
            PDummy dummy3 = AutoRegisteringDummy::create();
            CHECK (3 == trackedDummies.size());
            CHECK (dummy3 == trackedDummies[2]);
            
            CHECK (1+2+3 == checksum);
            
            dummy2.unlink();
            CHECK (1 + 3 == checksum);
            CHECK (2 == trackedDummies.size());
            CHECK (dummy1 == trackedDummies[0]);
            CHECK (dummy3 == trackedDummies[1]);
            
            CHECK (1 == dummy2.use_count());
            CHECK (2 == dummy1.use_count());
            CHECK (2 == dummy3.use_count());
            
            // deliberately discard our reference, 
            // so the only remaining ref is in the tracker
            dummy1.reset();
            dummy2.reset();
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
          
          PTimeline specialTimeline (asset::Struct::create (Query<Timeline> ("id(testical)")));
          CHECK (specialTimeline);
          CHECK (num_timelines + 1 == sess->timelines.size());
          CHECK (specialTimeline == session->timelines[num_timelines]);
          CHECK (specialTimeline.use_count() == 3);                        // we, the AssetManager and the session
          
          PTimeline anotherTimeline (asset::Struct::create (Query<Timeline> ()));
          CHECK (num_timelines + 2 == sess->timelines.size());
          CHECK (specialTimeline == session->timelines[num_timelines]);
          CHECK (anotherTimeline == session->timelines[num_timelines+1]);  // new one got appended at the end
          
          AssetManager& assetM (AssetManager::instance());
          CHECK (assetM.known (specialTimeline->getID()));
          assetM.remove (specialTimeline->getID());                   //////////////TICKET #550
          CHECK (!assetM.known (specialTimeline->getID()));
          
          CHECK (num_timelines + 1 == sess->timelines.size());
          CHECK (anotherTimeline == session->timelines[num_timelines]);    // moved to the previous slot
          CHECK (specialTimeline.use_count() == 1);                        // we're holding the last reference
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SessionElementTracker_test, "unit session");
  
  
  
}}} // namespace mobject::session::test
