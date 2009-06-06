/*
  MObjectRef(Test)  -  validating basic properties of the external MObject reference tag
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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
#include "lib/lumitime.hpp"
#include "proc/mobject/mobject.hpp"
#include "proc/mobject/mobject-ref.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/placement-ref.hpp"
#include "proc/mobject/explicitplacement.hpp"
#include "proc/mobject/test-dummy-mobject.hpp"

#include <iostream>

using lumiera::Time;
using std::string;
using std::cout;


namespace mobject {
namespace test    {
  
  using session::Clip;

  
  /***************************************************************************
   * @test properties and behaviour of the external reference-mechanism for
   *       MObjects placed into the session. We create a simple, simulated
   *       "session" (no real session datastructure) and a mock PlacementIndex.
   *       Then we add two Placements, which can be used then to build
   *       MObjectRef instances to validate their behaviour. 
   * @see  mobject::Placement
   * @see  mobject::MObject
   * @see  mobject::PlacementRef_test
   */
  class MObjectRef_test : public Test
    {
      
      typedef Placement<MObject>        PMObj;
      typedef Placement<Clip>           PClip;
      typedef TestPlacement<TestSubMO1> PSub1;
      
      
      virtual void
      run (Arg) 
        {
          
          // create data simulating a "Session"
          PMObj  pClip1 = asset::Media::create("test-1", asset::VIDEO)->createClip();
          PMObj  pClip2 = asset::Media::create("test-2", asset::VIDEO)->createClip();
          
          // set up a tie to fixed start positions
          pClip1.chain(Time(10));
          pClip2.chain(Time(20));
          
          ASSERT (pClip1->isValid());
          ASSERT (pClip2->isValid());
          ASSERT (2 == pClip1.use_count());  // one by the placement and one by the clip-Asset
          ASSERT (2 == pClip2.use_count());
          
          
          // Prepare an (test)Index
          typedef shared_ptr<PlacementIndex> PIdx;
          PIdx index (PlacementIndex::create());
          PMO& root = index->getRoot();
          reset_PlacementIndex(index);
          
          // Add the Clips to "session"
          index->insert (pClip1, root);
          index->insert (pClip2, root);
          ASSERT (2 == index->size());
          
          
          // -----Tests------------------
          // -----Tests------------------
          
          // verify clean state
          index->remove (pClip1);
          index->remove (pClip2);
          ASSERT (0 == index->size());
          ASSERT (2 == pClip1.use_count());
          ASSERT (2 == pClip2.use_count());
          reset_PlacementIndex();
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (MObjectRef_test, "unit session");
  
  
}} // namespace mobject::test
