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
#include "proc/assetmanager.hpp"
#include "lib/lumitime.hpp"
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
  
  using lumiera::Time;
  
  
  /********************************************************************************
   * @test verify retrieval and instance management of the top level facade objects
   *       as integrated with the session and high-level model.
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
        }
      
      
      void
      verify_retrieval()
        {
          
          PSess sess = Session::current;
          ASSERT (sess->isValid());
          
          
          ASSERT (0 < sess->timelines.size());
          Timeline& til = sess->timelines[0];
          
          ASSERT (0 < sess->sequences.size());
          Sequence& seq = sess->sequences[0];
          
          ASSERT (isSameObject (seq, til.getSequence()));
          
          //verify default timeline
          Axis& axis = til.getAxis();
          ASSERT (Time(0) == axis.origin());
          ASSERT (Time(0) == til.length());                   ////////////////////////TICKET #177
          
          //verify global pipes
          //TODO
          
          //verify default sequence
          Track rootTrack = seq.rootTrack();
          ASSERT (rootTrack->isValid());
          ASSERT (Time(0) == rootTrack->length());
          ASSERT (0 == rootTrack->subTracks.size());
          ASSERT (0 == rootTrack->clips.size());
          //TODO verify the output slots of the sequence
          
          //TODO now use the generic query API to discover the same structure.
          ASSERT (til == *(sess->all<Timeline>()));
          ASSERT (seq == *(sess->all<Sequence>()));
          ASSERT (rootTrack == *(sess->all<Track>()));
          ASSERT (! sess->all<Clip>());
          
          QueryFocus& focus = sess->focus();
          ASSERT (rootTrack == focus.getObject());
          focus.navigate (til);
          ASSERT (til.getBinding() == focus.getObject());
          ASSERT (rootTrack == *(focus.children()));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimelineSequenceHandling_test, "unit session");
  
  
  
}}} // namespace mobject::session::test
