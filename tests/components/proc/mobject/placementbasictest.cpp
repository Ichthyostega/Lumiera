/*
  PlacementBasic(Test)  -  basic Placement and MObject handling
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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


#include "common/test/run.hpp"
#include "proc/mobject/session.hpp"
#include "proc/mobject/session/edl.hpp"
#include "proc/mobject/session/testclip.hpp"
#include "proc/mobject/placement.hpp"
#include "common/util.hpp"

//#include <boost/format.hpp>
#include <iostream>

//using boost::format;
using cinelerra::Time;
using util::contains;
using std::string;
using std::cout;


namespace mobject
  {
  namespace session
    {
    namespace test
      {
      
      
      
      
      /*******************************************************************
       * @test basic behaviour of Placements and access to MObjects.
       * @see  mobject::Placement
       * @see  mobject::MObject#create
       * @see  mobject::Placement#addPlacement
       * @see  mobject::Placement#resolve
       */
      class PlacementBasic_test : public Test
        {
          typedef shared_ptr<asset::Media> PM;
          
          virtual void
          run (Arg arg) 
            {
              // create Clip-MObject, which is wrapped into a placement (smart ptr)
              PM media = asset::Media::create("test-1", VIDEO);
              Placement<Clip> pc = MObject::create(media);

              // can use the Clip-MObject interface by dereferencing the placement
              PM clip_media = pc->getMedia();
              ASSERT (clip_media->ident.category.hasKind (VIDEO));
              
              // using the Placement interface
              // TODO: how to handle unterdetermined Placement? Throw?
              FixedPlacement & fixpla = pc.addPlacement(Placement::FIXED, Time(1)); // TODO: the track??
              ExplicitPlacement expla = pc.resolve();
              ASSERT (expla.time == 1);
              ASSERT (!expla.isOverdetermined());
              ASSERT (*expla == *pc);
              ASSERT (*fixpla == *pc);
              
              // now overconstraining with another Placement
              pc.addPlacement(Placement::FIXED, Time(2));
              expla = pc.resolve();
              ASSERT (expla.time == 2); // the latest addition wins
              ASSERT (expla.isOverdetermined()); 
            } 
        };
      
      
      /** Register this test class... */
      LAUNCHER (PlacementBasic_test, "unit session");
      
      
      
    } // namespace test
  
  } // namespace session

} // namespace mobject
