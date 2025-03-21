/*
  RebuildFixture(Test)  -  (re)building the explicit placements

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file rebuildfixturetest.cpp
 ** unit test \ref RebuildFixture_test
 */


#include "lib/test/run.hpp"
#include "steam/mobject/session.hpp"
#include "steam/mobject/session/testsession1.hpp"
//#include "lib/format-cout.hpp"
//#include "lib/util.hpp"

//using util::contains; 


namespace steam    {
namespace mobject {
namespace session {
namespace test    {
  
  
  
  
  /***************************************************************//**
   * @test (re)building the ExplicitPlacement objects from the objects
   *       placed into the Session/Model.
   * @see  mobject::session::Fixture
   * @see  mobject::ExplicitPlacement
   */
  class RebuildFixture_test : public Test
    {
      virtual void
      run (Arg arg)
        {
          PSess sess = Session::current;
          sess.clear();
          buildTestsession1();
          CHECK (sess->isValid());
          sess->rebuildFixture();
#if false ////////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #548
          TODO ("check the fixture has been touched. e.g. by hash.");
          TODO ("query all Placements of all Clips (via AssetManager). Verify explicit plac contained in Fixture.");
          
          UNIMPLEMENTED ("iterate over fixture");
// TODO              
//        for_each (sess->getFixture(), 
//                  bind (&check_is_from_EDL, _1, sess.getEDL()));
          
          TODO ("can we check the other direction, from EDL to Fixture??");
        }
      
      static void 
      check_is_from_EDL (PMO explicitPlacement, EDL& edl)
        {
          
////TODO do we still support this? can it be replaced by a directly checking predicate on ExplicitPlacement??
          
//        PMO originalPlacement = explicitPlacement->subject->getPlacement();
//        CHECK (edl.contains(originalPlacement));
#endif ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #548
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (RebuildFixture_test, "unit session");
  
  
  
}}}} // namespace steam::mobject::session::test
