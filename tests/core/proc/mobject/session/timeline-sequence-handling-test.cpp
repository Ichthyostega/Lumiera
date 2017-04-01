/*
  TimelineSequenceHandling(Test)  -  managing the top level session facade objects

  Copyright (C)         Lumiera.org
    2008, 2011,         Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/

/** @file timeline-sequence-handling-test.cpp
 ** unit test \ref TimelineSequenceHandling_test
 */


#include "lib/test/run.hpp"
#include "proc/mobject/session.hpp"
#include "proc/mobject/mobject-ref.hpp"
#include "proc/mobject/session/binding.hpp"
//#include "proc/mobject/session/fixture.hpp"             // TODO only temporarily needed
#include "proc/assetmanager.hpp"
#include "proc/asset/timeline.hpp"
#include "proc/asset/sequence.hpp"
#include "proc/asset/pipe.hpp"
#include "common/query.hpp"
#include "lib/util.hpp"


using util::isSameObject;
using util::contains;


namespace proc    {
namespace mobject {
namespace session {
namespace test    {
  
  using proc_interface::AssetManager;
  using proc_interface::PAsset;
  
  using asset::Timeline;
  using asset::PTimeline;
  using asset::Sequence;
  using asset::PSequence;
  using asset::Pipe;
  
  using lumiera::Query;
  
  
  /****************************************************************************//**
   * @test verify retrieval and instance management of the top level facade objects
   *       as integrated with the session and high-level model. Both sequences and
   *       timelines are at the same time structural assets and act as facades
   *       on the session API. Thus we can query specific instances from the
   *       struct factory or alternatively access them through the session.
   *       Moreover we can create new top level elements in the session
   *       just by querying the respective asset.
   *       
   * @todo specify how deletion is handled      
   * @todo specify how to \em move objects by placement
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
          CHECK (Session::current.isUp());
          
          verify_retrieval();
          verify_creation();
          verify_removalTimeline();
          verify_removalBinding();
          verify_removalSequence();
          
          indirect_SequenceHandling();
        }
      
      
      void
      verify_retrieval()
        {
          PSess sess = Session::current;
          CHECK (sess->isValid());
          CHECK (0 < sess->timelines.size());
          
          PTimeline defaultTimeline = sess->defaults (Query<Timeline> ());           //////////////////////TICKET #549
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
          Query<Timeline> query1 = "id("+defaultTimeline->getNameID()+").";
          
          PTimeline queriedTimeline = asset::Struct::retrieve (query1);
          CHECK (queriedTimeline);
          CHECK (queriedTimeline == defaultTimeline);  // retrieved the existing timeline asset again
          CHECK (queriedTimeline == sess->timelines[0]);
          
          Query<Sequence> query2 = "id("+defaultTimeline->getSequence()->getNameID()+").";
          PSequence queriedSequence = asset::Struct::retrieve (query2);
          CHECK (queriedSequence);
          CHECK (queriedSequence == sess->sequences[0]);
          CHECK (queriedSequence == sess->timelines[0]->getSequence());
          CHECK (queriedSequence == defaultTimeline->getSequence());
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
        }
      
      
      void
      verify_creation()
        {
          PSess sess = Session::current;
          CHECK (sess->isValid());
          
          uint num_timelines = sess->timelines.size();
          CHECK (0 < num_timelines);
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
          Query<Timeline> special = "id(aSillyName), sequence("
                                  + sess->sequences[0]->getNameID()
                                  + "), pipe(ambiance).";
          
          PTimeline specialTimeline (asset::Struct::retrieve (special));
          CHECK (specialTimeline);
          CHECK (num_timelines + 1 == sess->timelines.size());
          CHECK (specialTimeline == session->timelines[num_timelines]);  // new one got appended at the end
          
          // verify the properties
          CHECK (specialTimeline->getSequence() == sess->sequences[0]);  // the already existing sequence got bound into that timeline too
          CHECK (contains (specialTimeline->pipes, Pipe::query("pipe(ambiance)")));
          
          CHECK (specialTimeline.use_count() == 3); // we, the AssetManager and the session
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
        }
      
      
      void
      verify_removalTimeline()
        {
          PSess sess  (Session::current);
          AssetManager& assetM (AssetManager::instance());
          
          CHECK (sess->isValid());
          uint num_timelines = sess->timelines.size();
          CHECK (2 <= num_timelines);
          
          PTimeline specialTimeline = sess->timelines[num_timelines-1];
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
          CHECK ("aSillyName" == specialTimeline->getNameID());
          
          RBinding binding = specialTimeline->getBinding();
          CHECK (binding);
          
          PSequence theSeq = binding->getSequence();
          CHECK (theSeq == sess->sequences[0]);
          CHECK (theSeq == specialTimeline->getSequence());
          CHECK (assetM.known (theSeq->getID()));
          
          // cause removal of the timeline
          assetM.remove (specialTimeline->getID());                   //////////////TICKET #550
          
          CHECK (!assetM.known (specialTimeline->getID()));
          CHECK (1 == specialTimeline.use_count());      // we hold the only remaining ref   
          CHECK ( assetM.known (theSeq->getID()));      // bound sequence isn't affected
          CHECK (theSeq == sess->sequences[0]);
          
          CHECK (num_timelines - 1 == sess->timelines.size());
          CHECK (!binding);                         //  got purged from the model
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
        }
      
      
      void
      verify_removalBinding()
        {
          PSess sess  (Session::current);
          AssetManager& assetM (AssetManager::instance());
          
          CHECK (sess->isValid());
          uint num_timelines = sess->timelines.size();
          CHECK (0 < num_timelines);
          
          // create a new Timeline to play with, using the default sequence...
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
          PTimeline aTimeline (asset::Struct::retrieve (Query<Timeline> ("sequence("+
                                                                      + sess->sequences[0]->getNameID()
                                                                      +        ").")));
          CHECK (num_timelines + 1 == sess->timelines.size());
          RBinding binding = aTimeline->getBinding();
          CHECK (binding);
          
          PSequence theSeq = binding->getSequence();
          CHECK (theSeq == sess->sequences[0]);
          CHECK (theSeq == aTimeline->getSequence());
          CHECK (assetM.known (aTimeline->getID()));
          CHECK (assetM.known (theSeq->getID()));
          
          // indirectly cause removal of the timeline by dropping the binding
          sess->remove(binding);
          CHECK (!binding);
          
          CHECK (!assetM.known (aTimeline->getID()));
          CHECK (1 == aTimeline.use_count());   
          CHECK ( assetM.known (theSeq->getID()));
          
          CHECK (num_timelines == sess->timelines.size());
          CHECK (!contains (sess->timelines, aTimeline));
          CHECK ( contains (sess->sequences, theSeq));   // unaffected
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
        }
      
      
      void
      verify_removalSequence()
        {
          PSess sess  (Session::current);
          AssetManager& assetM (AssetManager::instance());
          
          CHECK (sess->isValid());
          uint num_timelines = sess->timelines.size();
          uint num_sequences = sess->sequences.size();
          
          // create a new timeline, bound to a new sequence...
          PTimeline aTimeline (asset::Struct::retrieve (Query<Timeline> ()));
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
          PSequence aSequence (aTimeline->getSequence());
          CHECK (num_timelines + 1 == sess->timelines.size());
          CHECK (num_sequences + 1 == sess->sequences.size());
          
          RBinding binding = aTimeline->getBinding();
          RTrack rootTrack = aSequence->getTracks();
          CHECK (rootTrack);
          CHECK (binding);
          
          CHECK (aSequence == binding->getSequence());
          CHECK (assetM.known (aTimeline->getID()));
          CHECK (assetM.known (aSequence->getID()));
          
          // purging the sequence cascades to all linked entities
          assetM.remove (aSequence->getID());                   //////////////TICKET #550
          
          CHECK (!assetM.known (aTimeline->getID()));
          CHECK (!assetM.known (aSequence->getID()));
          CHECK (!rootTrack);
          CHECK (!binding);
          
          CHECK (num_timelines == sess->timelines.size());
          CHECK (num_sequences == sess->sequences.size());
          CHECK (!contains (sess->timelines, aTimeline));
          CHECK (!contains (sess->sequences, aSequence));
          
          CHECK (1 == aTimeline.use_count());   
          CHECK (1 == aSequence.use_count());   
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
        }
      
      
      void
      indirect_SequenceHandling()
        {
          PSess sess  (Session::current);
          AssetManager& assetM (AssetManager::instance());
          
          CHECK (sess->isValid());
          uint num_sequences = sess->sequences.size();
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
          RFork someFork = sess->sequences[0]->getForks();
          
          // indirectly cause a new sequence to come to life...
          RFork newFork = sess->getRoot().attach (someFork);           // attach new Placement<Fork> to root scope
          CHECK (newFork != someFork); // it's a new placement
          
          CHECK (num_sequences + 1 == sess->sequences.size());         // this root-attachment created a new sequence by sideeffect
          PSequence aSequence = sess->sequences[num_sequences];
          CHECK (newFork == aSequence->getForks());
          CHECK (newFork);
          CHECK (someFork);
          CHECK (assetM.known (aSequence->getID()));
          
          //TODO maybe even bind it into a new timeline. Then verify this new timeline gets removed alongside with the sequence below!
          
          // just moving the new fork away from root position
          // causes the sequence to disappear
          newFork.getPlacement().chain (someFork, Time(20));           /////////////////TICKET #555   does moving by placement really work this way??
          
          //TODO 3/2010 not finally decided *if* we want this behaviour
          //TODO how to verify the changed placement??
          
          CHECK (!assetM.known (aSequence->getID()));
          CHECK (num_sequences == sess->sequences.size());
          CHECK (!contains (sess->sequences, aSequence));
          
          CHECK (someFork);
          CHECK (newFork);
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimelineSequenceHandling_test, "unit session");
  
  
  
}}}} // namespace proc::mobject::session::test
