/*
  AddClip(Test)  -  adding an Clip-MObject to the Model/Session

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file add-clip-test.cpp
 ** unit test \ref AddClip_test
 */


#include "lib/test/run.hpp"
#include "steam/mobject/session.hpp"
#include "steam/mobject/session/testclip.hpp"
#include "steam/mobject/placement.hpp"
//#include "lib/format-cout.hpp"
#include "lib/util.hpp"


using util::contains;
//using std::string;


namespace steam {
namespace mobject {
namespace session {
namespace test {
  
  
  
  
  /***************************************************************//**
   * @test adding an test clip to the Model/Session.
   * @see  mobject::session::Clip
   * @see  mobject::Session
   */
  class AddClip_test : public Test
    {
      virtual void
      run (Arg) 
        {
          PSess sess = Session::current;
          PMO clip = TestClip::create();
          sess->attach (clip);
          
///////////////////////////////////////////////////////////////////TICKET #499              
//            CHECK (sess->currEDL().contains (clip));
          // TODO: Clip-Asset and Placement magic??
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (AddClip_test, "unit session");
  
  
  
}}}} // namespace steam::mobject::session::test
