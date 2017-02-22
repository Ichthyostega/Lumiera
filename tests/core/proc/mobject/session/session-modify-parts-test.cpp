/*
  SessionModifyParts(Test)  -  adding and removing elements, changing structure

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file session-modify-parts-test.cpp
 ** unit test \ref SessionModifyParts_test
 */


#include "lib/test/run.hpp"
#include "proc/mobject/session.hpp"
#include "proc/mobject/session/fixture.hpp"             // TODO only temporarily needed
//#include "proc/assetmanager.hpp"          //////??
//#include "proc/asset/timeline.hpp"
#include "proc/asset/sequence.hpp"
#include "lib/format-cout.hpp"
#include "lib/util-foreach.hpp"
#include "proc/mobject/session/testclip.hpp"
#include "proc/mobject/mobject-ref.hpp"
#include "proc/mobject/placement.hpp"
#include "common/query.hpp"

#include <functional>
#include <set>

using std::ref;
using std::placeholders::_1;
using util::isSameObject;
using util::and_all;
using std::set;


namespace proc    {
namespace mobject {
namespace session {
namespace test    {
  
//  using proc_interface::AssetManager;
  using proc_interface::PAsset;
  
//  using asset::PTimeline;
  using asset::PSequence;
  using asset::Sequence;
  
  using lumiera::Query;
  
  typedef MORef<session::Clip> RClip;
  typedef PlacementMO::ID PID;
  
  
  
  /***************************************************************************//**
   * @test perform the most important structural modifications on a session and
   *       verify they're carried out properly. 
   *       - attaching forks ("tracks")
   *       - adding clips
   *       
   *       
   * @todo check more kinds of modifications, especially moving parts 
   * @todo define criteria to be checked more precisely
   * @todo verify the actually dispatched commands 
   */
  class SessionModifyParts_test : public Test
    {
      virtual void
      run (Arg) 
        {
          Session::current.reset();
          CHECK (Session::current.isUp());
          
          addForks();
          addObjects();
          removeParts();
          
          verify_dispatchedCommands();
        }
      
      
      void
      addForks()
        {
          PSess sess = Session::current;
          CHECK (sess->isValid());
          
          PSequence seq = sess->defaults (Query<Sequence> ());           //////////////////////TICKET #549
          CHECK (seq);
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
          RFork fork1 = seq->attachFork();
          RFork fork2 = seq->attachFork("fork-2");
          RFork fork21 = seq->attachFork("fork-2.1", fork2);
          RFork fork22 = seq->attachFork("fork-2.2", "fork-2");
          
          QueryFocus& focus = sess->focus();
          CHECK (fork22 == focus.getObject());
          
          RFork fork3 = seq->attachFork("fork-3", "root");
          CHECK (fork3  == focus.getObject());
          
          RFork fork31 = sess->attach(
              asset::Struct::retrieve (Query<asset::???? > ("id(fork31)")));  ////TODO broken: we don't use a dedicated Fork asset anymore. It's just an EntryID<Fork>
          
          
          CHECK (fork31 == focus.getObject());
          RFork rootFork = seq->rootFork();
          CHECK (3 == rootFork->subForks.size());
          CHECK (fork1  == rootFork->subForks[0]);
          CHECK (fork2  == rootFork->subForks[1]);
          CHECK (fork3  == rootFork->subForks[2]);
          CHECK (0 == fork1->subForks.size());
          CHECK (2 == fork2->subForks.size());
          CHECK (fork21 == fork2->subForks[0]);
          CHECK (fork22 == fork2->subForks[1]);
          CHECK (1 == fork3->subForks.size());
          CHECK (fork21 == fork3->subForks[0]);
          
          set<RFork> allForks;
          allForks.insert(fork1);
          allForks.insert(fork2);
          allForks.insert(fork21);
          allForks.insert(fork22);
          allForks.insert(fork3);
          allForks.insert(fork31);
          
          // verify we indeed covered all forks known to the session....
          CHECK (and_all (sess->all<Fork>(), contains, ref(allForks), _1 ));
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
        }
      
      
      void
      addObjects()
        {
          PSess sess = Session::current;
          CHECK (sess->isValid());
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
          QueryFocus& focus = sess->focus();
          CHECK (focus.getObject().isCompatible<session::Fork>());
          RClip clip1 = sess->attach (TestClip::create());
          
          RTrack fork31 = clip.getParent();
          CHECK (fork31);
          CHECK ("fork31" == fork31->getNameID());
          
          CHECK (1 == fork31->clips.size());
          CHECK (clip1 == fork31->clips[0]);
          
          RClip clip2 = fork31.attach (TestClip::create());
          RClip clip3 = fork31.attach (clip1);          // creates a clone instance
          
          CHECK (clip1); CHECK (clip2); CHECK (clip3);
          CHECK (clip1 != clip2);
          CHECK (clip1 != clip3);
          CHECK (clip2 != clip3);
          CHECK (!isSharedPointee (clip1, clip2));
          CHECK (!isSharedPointee (clip2, clip3));
          CHECK ( isSharedPointee (clip1, clip3));
          CHECK (isEquivalentPlacement (clip1, clip2));
          CHECK (isEquivalentPlacement (clip2, clip3));
          CHECK (isEquivalentPlacement (clip1, clip3));
          
          RTrack fork2 = sess->sequences[0]
                              ->rootTrack()
                              ->subTracks[1];
          RClip clip4 = fork2.attach (TestClip::create());
          
          // now verify structure built up thus far
          CHECK (focus.getObject() == fork2);        // focus follows point-of-mutation
          CHECK (focus.contains (clip4));
          CHECK (!focus.contains (clip1));
          CHECK (!focus.contains (clip2));
          CHECK (!focus.contains (clip3));
          
          focus.attach (fork31);
          CHECK (focus.getObject() == fork31);
          CHECK (focus.contains (clip1));
          CHECK (focus.contains (clip2));
          CHECK (focus.contains (clip3));
          CHECK (!focus.contains (clip4));
          
          focus.reset();
          CHECK (focus.getObject() == sess->getRoot());
          CHECK (focus.contains (clip1));             // containment test includes sub-scopes
          CHECK (focus.contains (clip2));
          CHECK (focus.contains (clip3));
          CHECK (focus.contains (clip4));
          CHECK (!focus.hasChild (clip1));            // but they are indeed nested more deeply
          CHECK (!focus.hasChild (clip2));
          CHECK (!focus.hasChild (clip3));
          CHECK (!focus.hasChild (clip4));
          
          focus.attach (sess->sequences[0]->rootTrack()->subTracks[2]->subTracks[0]);     // train wreck. Don't try it at home!
          CHECK (focus.getObject() == fork31);                                           // (this test is an exception, as we know the structure precisely
                                                                                          //  production code should always discover one level a time)
          CHECK ( focus.hasChild (clip1));
          CHECK ( focus.hasChild (clip2));
          CHECK ( focus.hasChild (clip3));
          CHECK (!focus.hasChild (clip4));            // ...because this one is on fork2, not fork31
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
        }
      
      
      void
      removeParts()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
          PSess sess = Session::current;
          CHECK (sess->isValid());
          
          RTrack fork31 = sess->sequences[0]->rootTrack()->subTracks[2]->subTracks[0];
          CHECK (fork31);
          CHECK (3 == fork31->clips.size());
          RClip clip2 = fork31->clips[1];
          
          QueryFocus& focus = sess->focus();
          focus.reset(); // navigate to root
          
          CHECK (focus.contains (clip2));
          CHECK (clip2);
          clip2.purge();
          CHECK (!clip2);
          CHECK (!focus.contains (clip2));
          
          CHECK (2 == fork31->clips.size());
          CHECK (clip2 != fork31->clips[1]);
          
          CHECK (focus.getObject() == fork31);       // focus follows point-of-mutation
          
          // Using the query-focus to explore the contents of this current object (fork31)
          ScopeQuery<Clip>::iterator discoverClips = focus.explore<Clip>();
          CHECK (discoverClips);
          RClip clip1 = *discoverClips;
          ++discoverClips;
          RClip clip3 = *discoverClips;
          ++discoverClips;
          CHECK (!discoverClips);
          
          CHECK (fork31->clips[0] == clip1);
          CHECK (fork31->clips[1] == clip3);
          /* please note: the clips aren't discovered in any defined order (hashtable!)
           *              especially, the order doesn't match the order of addition!
           *              thus, what's called clip1 here may or may not be
           *              what we called clip1 in addObjects()
           */
          
          RTrack fork3 = fork31.getParent();
          
          focus.reset(); // back to root
          CHECK (focus.contains (clip1));
          CHECK (focus.contains (clip3));
          CHECK (focus.contains (fork3));
          CHECK (focus.contains (fork31));
          CHECK (clip1);
          CHECK (clip3);
          CHECK (fork3);
          CHECK (fork31);
          
          sess->purge (fork31);
          
          CHECK (focus.getObject() == fork3);
          focus.reset();
          CHECK ( focus.contains (fork3));
              
          CHECK (!focus.contains (clip1));
          CHECK (!focus.contains (clip3));
          CHECK (!focus.contains (fork31));
          CHECK (!clip1);
          CHECK (!clip3);
          CHECK (!fork31);
          CHECK (fork3);
          
          fork3.purge();
          CHECK (!fork3);
          PSequence aSequence = sess->sequences[0];
          CHECK (focus.getObject() == aSequence->rootTrack());
          CHECK (2 == aSequece->rootTrack()->subTracks.size());
          
          CHECK ( contains (sess->sequences, aSequence));
          aSequence->rootFork().purge();                      // automatically kills the sequence as well (sequence == facade to the fork root)
          CHECK (!contains (sess->sequences, aSequence));
          CHECK (0 == sess->sequences.size());
          CHECK (0 == sess->timelines.size());                // killing the sequence also cascaded to the timeline and binding
          CHECK (!sess->isValid());                           // thus effectively the session is now invalid (no timeline)
          
          CHECK (focus.getObject() == sess->getRoot());
          
          PID currRoot = sess->getRoot.getPlacement().getID();
          sess->getRoot().purge();                            // purging the root scope effectively resets the session to defaults
          CHECK (currRoot == sess->getRoot.getPlacement.getID);
                                                              // but the root element itself is retained
          CHECK (sess->isValid());
          CHECK (1 == sess->timelines.size());
          CHECK (1 == sess->sequences.size());
          CHECK (aSequence != sess->sequences[0]);
          CHECK (aSequence.use_count() == 1);                 // we're holding the last remaining reference
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #499
        }
      
      
      void
      verify_dispatchedCommands()
        {
          TODO ("verify the commands issued by this test");   ////////////////////////TICKET #567
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (SessionModifyParts_test, "unit session");
  
  
  
}}}} // namespace proc::mobject::session::test
