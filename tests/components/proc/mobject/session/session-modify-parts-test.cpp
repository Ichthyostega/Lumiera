/*
  SessionModifyParts(Test)  -  adding and removing elements, changing structure
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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
#include "proc/mobject/session/fixture.hpp"             // TODO only temporarily needed
//#include "proc/assetmanager.hpp"          //////??
//#include "proc/asset/timeline.hpp"
#include "proc/asset/sequence.hpp"
//#include "lib/lumitime.hpp"
#include "lib/util-foreach.hpp"
#include "proc/mobject/session/testclip.hpp"
#include "proc/mobject/mobject-ref.hpp"
#include "proc/mobject/placement.hpp"

#include <tr1/functional>
#include <boost/ref.hpp>
#include <iostream>
#include <set>

using boost::ref;
using std::tr1::placeholders::_1;
using util::isSameObject;
using util::and_all;
using std::string;
using std::cout;
using std::set;


namespace mobject {
namespace session {
namespace test    {
  
//  using proc_interface::AssetManager;
  using proc_interface::PAsset;
  
//  using asset::PTimeline;
  using asset::PSequence;
  using asset::RTrack;
  
//  using lumiera::Time;
  
  typedef MORef<session::Clip> RClip;
  
  
  
  /*******************************************************************************
   * @test perform the most important structural modifications on a session and
   *       verify they're carried out properly. 
   *       - attaching tracks
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
          
          addTracks();
          addObjects();
          removeParts();
          
          verify_dispatchedCommands();
        }
      
      
      void
      addTracks()
        {
          PSess sess = Session::current;
          CHECK (sess->isValid());
          
          PSequence seq = sess->defaults (Query<Sequence> ());           //////////////////////TICKET #549
          CHECK (seq);
          
          RTrack track1 = seq->attachTrack();
          RTrack track2 = seq->attachTrack("track-2");
          RTrack track21 = seq->attachTrack("track-2.1", track2);
          RTrack track22 = seq->attachTrack("track-2.2", "track-2");
          
          QueryFocus& focus = sess->focus();
          CHECK (track22 == focus.getObject());
          
          RTrack track3 = seq->attachTrack("track-3", "root");
          CHECK (track3  == focus.getObject());
          
          RTrack track31 = sess->attach(
              asset::Struct::create (Query<asset::Track> ("id(track31)")));
          
          
          CHECK (track31 == focus.getObject());
          RTrack rootTrack = seq->rootTrack();
          CHECK (3 == rootTrack->subTracks.size());
          CHECK (track1  == rootTrack->subTracks[0]);
          CHECK (track2  == rootTrack->subTracks[1]);
          CHECK (track3  == rootTrack->subTracks[2]);
          CHECK (0 == track1->subTracks.size());
          CHECK (2 == track2->subTracks.size());
          CHECK (track21 == track2->subTracks[0]);
          CHECK (track22 == track2->subTracks[1]);
          CHECK (1 == track3->subTracks.size());
          CHECK (track21 == track3->subTracks[0]);
          
          set<RTrack> allTracks;
          allTracks.insert(track1);
          allTracks.insert(track2);
          allTracks.insert(track21);
          allTracks.insert(track22);
          allTracks.insert(track3);
          allTracks.insert(track31);
          
          // verify we indeed covered all tracks known to the session....
          CHECK (and_all (sess->all<Track>(), contains, ref(allTracks), _1 ));
        }
      
      
      void
      addObjects()
        {
          PSess sess = Session::current;
          CHECK (sess->isValid());
          
          QueryFocus& focus = sess->focus();
          CHECK (focus.getObject().isCompatible<session::Track>());
          RClip clip1 = sess->attach (TestClip::create());
          
          RTrack track31 = clip.getParent();
          CHECK (track31);
          CHECK ("track31" == track31->getNameID());
          
          CHECK (1 == track31->clips.size());
          CHECK (clip1 == track31->clips[0]);
          
          RClip clip2 = track31.attach (TestClip::create());
          RClip clip3 = track31.attach (clip1);          // creates a clone instance
          
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
          
          RTrack track2 = sess->sequences[0]
                              ->rootTrack()
                              ->subTracks[1];
          RClip clip4 = track2.attach (TestClip::create());
          
          // now verify structure built up thus far
          CHECK (focus.getObject() == track2);        // focus follows point-of-mutation
          CHECK (focus.contains (clip4));
          CHECK (!focus.contains (clip1));
          CHECK (!focus.contains (clip2));
          CHECK (!focus.contains (clip3));
          
          focus.attach (track31);
          CHECK (focus.getObject() == track31);
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
          CHECK (focus.getObject() == track31);                                           // (this test is an exception, as we know the structure precisely
                                                                                          //  production code should always discover one level a time)
          CHECK ( focus.hasChild (clip1));
          CHECK ( focus.hasChild (clip2));
          CHECK ( focus.hasChild (clip3));
          CHECK (!focus.hasChild (clip4));            // ...because this one is on track2, not track31
        }
      
      
      void
      removeParts()
        {
          PSess sess = Session::current;
          CHECK (sess->isValid());
          
          RTrack track31 = sess->sequences[0]->rootTrack()->subTracks[2]->subTracks[0];
          CHECK (track31);
          CHECK (3 == track31->clips.size());
          RClip clip2 = track31->clips[1];
          
          QueryFocus& focus = sess->focus();
          focus.reset(); // navigate to root
          
          CHECK (focus.contains (clip2));
          CHECK (clip2);
          clip2.purge();
          CHECK (!clip2);
          CHECK (!focus.contains (clip2));
          
          CHECK (2 == track31->clips.size());
          CHECK (clip2 != track31->clips[1]);
          
          CHECK (focus.getObject() == track31);       // focus follows point-of-mutation
          
          // Using the query-focus to explore the contents of this current object (track31)
          ScopeQuery<Clip>::iterator discoverClips = focus.explore<Clip>();
          CHECK (discoverClips);
          RClip clip1 = *discoverClips;
          ++discoverClips;
          RClip clip3 = *discoverClips;
          ++discoverClips;
          CHECK (!discoverClips);
          
          CHECK (track31->clips[0] == clip1);
          CHECK (track31->clips[1] == clip3);
          /* please note: the clips aren't discovered in any defined order (hashtable!)
           *              especially, the order doesn't match the order of addition!
           *              thus, what's called clip1 here may or may not be
           *              what we called clip1 in addObjects()
           */
          
          RTrack track3 = track31.getParent();
          
          focus.reset(); // back to root
          CHECK (focus.contains (clip1));
          CHECK (focus.contains (clip3));
          CHECK (focus.contains (track3));
          CHECK (focus.contains (track31));
          CHECK (clip1);
          CHECK (clip3);
          CHECK (track3);
          CHECK (track31);
          
          sess->purge (track31);
          
          CHECK (focus.getObject() == track3);
          focus.reset();
          CHECK ( focus.contains (track3));
              
          CHECK (!focus.contains (clip1));
          CHECK (!focus.contains (clip3));
          CHECK (!focus.contains (track31));
          CHECK (!clip1);
          CHECK (!clip3);
          CHECK (!track31);
          CHECK (track3);
          
          track3.purge();
          CHECK (!track3);
          PSequence aSequence = sess->sequences[0];
          CHECK (focus.getObject() == aSequence->rootTrack());
          CHECK (2 == aSequece->rootTrack()->subTracks.size());
          
          CHECK ( contains (sess->sequences, aSequence));
          aSequence->rootTrack().purge();                     // automatically kills the sequence as well (sequence == facade to the root track)
          CHECK (!contains (sess->sequences, aSequence));
          CHECK (0 == sess->sequences.size());
          CHECK (0 == sess->timelines.size());                // killing the sequence also cascaded to the timeline and binding
          CHECK (!sess->isValid());                           // thus effectively the session is now invalid (no timeline)
          
          CHECK (focus.getObject() == sess->getRoot());
          
          sess->getRoot().purge();                            // purging the root scope effectively resets the session to defaults
          CHECK (sess->isValid());
          CHECK (1 == sess->timelines.size());
          CHECK (1 == sess->sequences.size());
          CHECK (aSequence != sess->sequences[0]);
          CHECK (aSequence.use_count() == 1);                 // we're holding the last remaining reference
        }
      
      
      void
      verify_dispatchedCommands()
        {
          TODO ("verify the commands issued by this test");   ////////////////////////TICKET #567
        }

    };
  
  
  /** Register this test class... */
  LAUNCHER (SessionModifyParts_test, "unit session");
  
  
  
}}} // namespace mobject::session::test
