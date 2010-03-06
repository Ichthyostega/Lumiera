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
#include "lib/util.hpp"

#include <iostream>

using util::isSameObject;
using std::string;
using std::cout;


namespace mobject {
namespace session {
namespace test    {
  
//  using proc_interface::AssetManager;
  using proc_interface::PAsset;
  
//  using asset::PTimeline;
  using asset::PSequence;
  using asset::RTrack;
  
//  using lumiera::Time;
  
  
  /*******************************************************************************
   * @test access the current session and verify the correct
   *       structure of the most important components: The session
   *       contains an Sequence, we can get at the Fixture, we have at least
   *       one Track and the corresponding Track asset is available.
   * @todo define further criteria to be checked 
   * @todo implement Sequence, Fixture, Session#rebuildFixture, asset::Track
   */
  class SessionModifyParts_test : public Test
    {
      virtual void
      run (Arg) 
        {
          Session::current.reset();
          CHECK (Session::current.isUp());
          
          addTracks();
          removeParts();
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
          
          RTrack track31 = sess->attach()
          
          CHECK (isSameObject (seq, til->getSequence()));
          
          //verify default timeline
          Axis& axis = til->getAxis();
          CHECK (Time(0) == axis.origin());
          CHECK (Time(0) == til->length());                   ////////////////////////TICKET #177
          
          //verify global pipes
          //TODO
          
          //verify default sequence
          Track rootTrack = seq->rootTrack();
          CHECK (rootTrack->isValid());
          CHECK (Time(0) == rootTrack->length());
          CHECK (0 == rootTrack->subTracks.size());
          CHECK (0 == rootTrack->clips.size());
          //TODO verify the output slots of the sequence
          
          //TODO now use the generic query API to discover the same structure.
          CHECK (til == *(sess->all<Timeline>()));
          CHECK (seq == *(sess->all<Sequence>()));
          CHECK (rootTrack == *(sess->all<Track>()));
          CHECK (! sess->all<Clip>());
          
          QueryFocus& focus = sess->focus();
          CHECK (rootTrack == focus.getObject());
          focus.navigate (til);
          CHECK (til.getBinding() == focus.getObject());
          CHECK (rootTrack == *(focus.children()));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SessionModifyParts_test, "unit session");
  
  
  
}}} // namespace mobject::session::test
