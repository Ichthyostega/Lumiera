/*
  SessionStructure(Test)  -  verifying basic Session/Model structure

  Copyright (C)         Lumiera.org
    2008, 2011,          Hermann Vosseler <Ichthyostega@web.de>

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

/** @file session-structure-test.cpp
 ** unit test \ref SessionStructure_test
 */


#include "lib/test/run.hpp"
#include "proc/mobject/session.hpp"
#include "proc/mobject/session/fixture.hpp"             // TODO only temporarily needed
#include "proc/assetmanager.hpp"          //////??
#include "proc/asset/timeline.hpp"
#include "proc/asset/sequence.hpp"
//#include "lib/format-cout.hpp"
#include "lib/util.hpp"

using util::isSameObject;


namespace proc    {
namespace mobject {
namespace session {
namespace test    {
  
  using proc_interface::AssetManager;
  using proc_interface::PAsset;
  
  using asset::PTimeline;
  using asset::PSequence;
  
  
  
  /***************************************************************************//**
   * @test access the current session and verify the correct
   *       structure of the most important components: The session
   *       contains an Sequence, we can get at the Fixture, we have at least
   *       one Fork and the corresponding Fork-ID (asset) can be retrieved.
   * @todo define further criteria to be checked 
   * @todo implement Sequence, Fixture, Session#rebuildFixture, handling of EntryID<Fork>
   */
  class SessionStructure_test : public Test
    {
      virtual void
      run (Arg) 
        {
          Session::current.reset();
          CHECK (Session::current.isUp());
          
          verify_defaultStructure();
        }
      
      
      void
      verify_defaultStructure()
        {
          
          PSess sess = Session::current;
          CHECK (sess->isValid());
          
          UNIMPLEMENTED("the real standard structure of the session"); //////////////////////////TICKET #499
          
#if false //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #546              
          
/////TODO the following is code from the old session mockup... try to rephrase it into the new (real) session API
//        
          CHECK (0 <= sess->currEDL().size());                // TODO implement
          CHECK (0 <= sess->getFixture()->size());             // TODO implement
          CHECK (sess->currEDL().getTracks()->isValid());
          
//              PAsset track = sess->currEDL().getTracks()[0];    // TODO obsolete
//              AssetManager& aMang = AssetManager::instance();
//              CHECK (track == aMang.getAsset (track->getID()));
          
#endif //////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #546              
          UNIMPLEMENTED ("how to refer to tracks...");
          
          CHECK (0 < sess->timelines.size());
          PTimeline til = sess->timelines[0];
          
          CHECK (0 < sess->sequences.size());
          PSequence seq = sess->sequences[0];
          
#if false //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #546              
          CHECK (isSameObject (seq, til->getSequence()));
          
          //verify default timeline
          Axis& axis = til->getAxis();
          CHECK (Time(0) == axis.origin());
          CHECK (Time(0) == til->length());                   ////////////////////////TICKET #177
          
          //verify global pipes
          //TODO
          
          //verify default sequence
          RFork rootFork = seq->rootFork();
          CHECK (rootFork->isValid());
          CHECK (Time(0) == rootFork->length());
          CHECK (0 == rootFork->subForks.size());
          CHECK (0 == rootFork->clips.size());
          //TODO verify the output slots of the sequence
          
          //TODO now use the generic query API to discover the same structure.
          CHECK (til == *(sess->all<Timeline>()));
          CHECK (seq == *(sess->all<Sequence>()));
          CHECK (rootFork == *(sess->all<Fork>()));
          CHECK (! sess->all<Clip>());
          
          QueryFocus& focus = sess->focus();
          CHECK (rootFork == focus.getObject());
          focus.navigate (til);
          CHECK (til.getBinding() == focus.getObject());
          CHECK (rootFork == *(focus.children()));
#endif //////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #546              
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SessionStructure_test, "unit session");
  
  
  
}}}} // namespace proc::mobject::session::test
