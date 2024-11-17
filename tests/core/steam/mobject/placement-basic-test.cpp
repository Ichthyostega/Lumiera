/*
  PlacementBasic(Test)  -  basic Placement and MObject handling

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file placement-basic-test.cpp
 ** unit test \ref PlacementBasic_test
 */


#include "lib/test/run.hpp"
#include "steam/asset/media.hpp"
#include "steam/mobject/session.hpp"
#include "steam/mobject/session/testclip.hpp"
#include "steam/mobject/placement.hpp"
#include "steam/mobject/explicitplacement.hpp"
#include "steam/mobject/session/mobjectfactory.hpp"   ////TODO: avoidable?
#include "lib/time/timevalue.hpp"
//#include "lib/util.hpp"


using lib::time::Time;


namespace steam    {
namespace mobject {
namespace session {
namespace test    {
  
  using asset::VIDEO;
  
  
  
  /***************************************************************//**
   * @test basic behaviour of Placements and access to MObjects.
   * @see  mobject::Placement
   * @see  mobject::MObject#create
   * @see  mobject::Placement#addPlacement
   * @see  mobject::Placement#resolve
   */
  class PlacementBasic_test : public Test
    {
      typedef shared_ptr<asset::Media> PM;
      typedef shared_ptr<asset::Clip> PCA;
      
      virtual void
      run (Arg)
        {
          // create Clip-MObject, which is wrapped into a placement (smart ptr)
          PM media = asset::Media::create("test-1", VIDEO);
          PCA clipAsset = Media::create(*media);
          Placement<Clip> pc = MObject::create (*clipAsset, *media);
          
          // use of the Clip-MObject interface by dereferencing the placement
          PM clip_media = pc->getMedia();
          CHECK (clip_media->ident.category.hasKind (VIDEO));
          
          // using the Placement interface
          // TODO: how to handle insufficiently determinated Placement? Throw?
          FixedLocation & fixloc = pc.chain (Time(0,1)); // TODO: the fork??
          ExplicitPlacement expla = pc.resolve();
          CHECK (expla.time == Time(0,1));
          CHECK (!expla.chain.isOverdetermined());
//            CHECK (*expla == *pc);  ////////////////////////////////////////////TICKET #511 define equivalence of locating chains and solutions
          
          // now overconstraining with another Placement
          pc.chain (Time(0,2));
          ExplicitPlacement expla2 = pc.resolve();
          CHECK (expla2.time == Time(0,2)); // the latest addition wins
          CHECK (expla2.chain.isOverdetermined());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (PlacementBasic_test, "unit session");
  
  
  
}}}} // namespace steam::mobject::session::test
