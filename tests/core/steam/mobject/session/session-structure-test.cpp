/*
  SessionStructure(Test)  -  verifying basic Session/Model structure

   Copyright (C)
     2008, 2011,      Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file session-structure-test.cpp
 ** unit test \ref SessionStructure_test
 */


#include "lib/test/run.hpp"
#include "steam/mobject/session.hpp"
#include "steam/fixture/fixture.hpp"             // TODO only temporarily needed
#include "steam/assetmanager.hpp"          //////??
#include "steam/asset/timeline.hpp"
#include "steam/asset/sequence.hpp"
//#include "lib/format-cout.hpp"
#include "lib/util.hpp"

using util::isSameObject;


namespace steam    {
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
  
  
  
}}}} // namespace steam::mobject::session::test
