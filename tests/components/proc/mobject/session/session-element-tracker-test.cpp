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
//#include "proc/mobject/session/fixture.hpp"             // TODO only temporarily needed
//#include "proc/assetmanager.hpp"
#include "proc/asset/timeline.hpp"
#include "proc/asset/sequence.hpp"
//#include "proc/asset/pipe.hpp"
//#include "lib/lumitime.hpp"
//#include "lib/query.hpp"
//#include "lib/util.hpp"

//#include <iostream>

//using util::isSameObject;
//using util::contains;
//using std::string;
//using std::cout;


namespace mobject {
namespace session {
namespace test    {
  
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
          UNIMPLEMENTED ("use a test dummy class and do a dry run of the ElementTracker");
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
