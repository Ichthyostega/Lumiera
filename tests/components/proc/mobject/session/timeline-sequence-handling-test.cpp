/*
  TimelineSequenceHandling(Test)  -  managing the top level session facade objects
 
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
#include "proc/mobject/session.hpp"
//#include "proc/mobject/session/fixture.hpp"             // TODO only temporarily needed
#include "proc/assetmanager.hpp" ///??
#include "proc/asset/timeline.hpp"
#include "proc/asset/sequence.hpp"
#include "proc/asset/pipe.hpp"
#include "lib/lumitime.hpp"
#include "lib/query.hpp"
#include "lib/util.hpp"

#include <iostream>

using util::isSameObject;
using std::string;
using std::cout;


namespace mobject {
namespace session {
namespace test    {
  
  using proc_interface::AssetManager;
  using proc_interface::PAsset;
  
  using asset::PTimeline;
  using asset::PSequence;
  using asset::Pipe;
  
  using lumiera::Query;
  using lumiera::Time;
  
  
  /********************************************************************************
   * @test verify retrieval and instance management of the top level facade objects
   *       as integrated with the session and high-level model. Both sequences and
   *       timelines are at the same time structural assets and act as facades
   *       on the session API. Thus we can query specific instances from the
   *       struct factory or alternatively access them through the session.
   *       Moreover we can create new top level elements in the session
   *       just by querying the respective asset.
   *       
   * @todo specify how deletion is handled      
   *       
   * @see  session-structure-test.cpp
   * @see  Timeline
   * @see  Sequence
   * @see  Session
   */
  class TimelineSequenceHandling_test : public Test
    {
      virtual void
      run (Arg) 
        {
          Session::current.reset();
          ASSERT (Session::current.isUp());
          
          verify_retrieval();
          verify_creation();
        }
      
      
      void
      verify_retrieval()
        {
          PSess sess = Session::current;
          ASSERT (sess->isValid());
          ASSERT (0 < sess->timelines.size());
          
          PTimeline defaultTimeline = sess->defaults (Query<Timeline> ());           //////////////////////TICKET #549
          Query<Timeline> query1 = "id("+defaultTimeline->getNameID()+").";
          
          PTimeline queriedTimeline = asset::Struct::create (query1);
          ASSERT (queriedTimeline);
          ASSERT (queriedTimeline == defaultTimeline);  // retrieved the existing timeline asset again
          ASSERT (queriedTimeline == sess->timelines[0]);
          
          Query<Sequence> query2 = "id("+defaultTimeline->getSequence()->getNameID()+").";
          PSequence queriedSequence = asset::Struct::create (query2);
          ASSERT (queriedSequence);
          ASSERT (queriedSequence == sess->sequences[0]);
          ASSERT (queriedSequence == sess->timelines[0]->getSequence());
          ASSERT (queriedSequence == defaultTimeline->getSequence());
        }
      
      
      void
      verify_creation()
        {
          PSess sess = Session::current;
          ASSERT (sess->isValid());
          
          uint num_timelines = sess->timelines.size();
          ASSERT (0 < num_timelines);
          
          Query<Timeline> special = "id(aSillyName), sequence("
                                  + sess->sequences[0]->getNameID()
                                  + "), pipe(ambiance).";
          
          PTimeline specialTimeline (asset::Struct::create (special));
          ASSERT (specialTimeline);
          ASSERT (num_timelines + 1 == sess->timelines.size());
          ASSERT (specialTimeline == session->timelines[num_timelines]);  // new one got appended at the end
          
          // verify the properties
          ASSERT (specialTimeline->getSequence() == sess->sequences[0]);  // the already existing sequence got bound into that timeline too
          ASSERT (contains (specialTimeline->pipes, Pipe::query("pipe(ambiance)")));
          
          ASSERT (specialTimeline.use_count() == 3); // we, the AssetManager and the session
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimelineSequenceHandling_test, "unit session");
  
  
  
}}} // namespace mobject::session::test
