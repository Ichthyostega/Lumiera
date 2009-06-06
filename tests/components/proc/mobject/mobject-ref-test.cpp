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
#include "proc/asset/media.hpp"
#include "proc/mobject/mobject.hpp"
#include "proc/mobject/mobject-ref.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/placement-ref.hpp"
#include "proc/mobject/session/clip.hpp"
#include "proc/mobject/explicitplacement.hpp"
#include "proc/mobject/test-dummy-mobject.hpp"

#include <iostream>

using std::string;
using std::cout;
using std::endl;


namespace mobject {
namespace test    {
  
  using lumiera::Time;
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
          
          // extract various kinds of IDs and refs
          PMObj &         rP1 (pClip1);
          PMObj const&    rP2 (pClip2);
          PMObj::ID       id1 = pClip1.getID();
          PMObj::Id<Clip> id2 = pClip2.getID();
          LumieraUid luid = id1.get();
          ///////////////////////////////////////////TODO: check the C-API representation here
          PlacementRef<Clip>    ref1 (id1);
          PlacementRef<MObject> ref2 (pClip2);
          
          
          // -----Tests------------------
          checkBuildMObjectRef (rP1,  &pClip1);
          checkBuildMObjectRef (rP2,  &pClip2);
          checkBuildMObjectRef (id1,  &pClip1);
          checkBuildMObjectRef (id2,  &pClip2);
          checkBuildMObjectRef (luid, &pClip1);
          ///////////////////////////////////////////TODO: check the C-API representation here
          checkBuildMObjectRef (ref1, &pClip1);
          checkBuildMObjectRef (ref2, &pClip2);
          // -----Tests------------------
          
          // verify clean state
          index->remove (pClip1);
          index->remove (pClip2);
          ASSERT (0 == index->size());
          ASSERT (2 == pClip1.use_count());
          ASSERT (2 == pClip2.use_count());
          reset_PlacementIndex();
        }
      
      
      template<typename REF>
      void
      checkBuildMObjectRef (REF refObj, void* placementAdr)
        {
          MORef<Clip> rMO;
          ASSERT (!rMO);                    // still empty (not bound)
          cout << rMO             << endl;
          cout << showSizeof(rMO) << endl;
          
          // activate by binding to provided ref
          rMO.activate(refObj);
          ASSERT (rMO);                     // now bound
          cout << rMO             << endl;
          
          // access MObject (Clip API)
          cout << rMO->operator string() << endl;
          cout << rMO->getMedia()->ident << endl;
          ASSERT (rMO->isValid());
          
          // access the Placement-API
          ASSERT (2 == rMO.use_count());    // we are referring, not creating a new Placement
          ASSERT (0 < rMO.getStartTime());  // (internally, this resolves to an ExplicitPlacement)
          ASSERT ( rMO.isCompatible<MObject>());
          ASSERT ( rMO.isCompatible<Clip>());
          ASSERT (!rMO.isCompatible<TestSubMO1>());
          Time start = rMO.getStartTime();
          
          // re-link to the Placement (note we get the Clip API!)
          Placement<Clip> & refP = rMO.getPlacement();
          ASSERT (refP);
          ASSERT (2 == refP.use_count());
          ASSERT (&refP == placementAdr);   // actually denotes the address of the original Placement in the "session"
          cout << string(refP) << endl;

          ExplicitPlacement exPla = refP.resolve();
          ASSERT (exPla.time == start);     // recovered Placement resolves to the same time as provided by the proxied API
          ASSERT (3 == refP.use_count());   // but now we've indeed created an additional owner (exPla)
          ASSERT (3 == rMO.use_count());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (MObjectRef_test, "unit session");
  
  
}} // namespace mobject::test
