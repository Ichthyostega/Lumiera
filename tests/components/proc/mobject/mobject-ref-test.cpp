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
#include "proc/mobject/session/placement-index.hpp"
#include "proc/mobject/session/session-service-mock-index.hpp"
#include "proc/mobject/session/clip.hpp"
#include "proc/mobject/explicitplacement.hpp"
#include "proc/mobject/test-dummy-mobject.hpp"
#include "lib/test/test-helper.hpp"

#include <iostream>

using std::string;
using std::cout;
using std::endl;


namespace mobject {
namespace test    {
  
  using lumiera::Time;
  using session::Clip;
  
  using session::SessionServiceMockIndex;  
  

  
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
          
          
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 384  !!!!!!!!!
          // Prepare an (test)Index
          PPIdx index = SessionServiceMockIndex::install();
          PMO& root = index->getRoot();
          
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
          
          checkLifecycle (rP1,rP2);
          checkTypeHandling (luid);
          // -----Tests------------------
          
          // verify clean state
          index->remove (pClip1);
          index->remove (pClip2);
          ASSERT (0 == index->size());
          ASSERT (1 == index.use_count());
          ASSERT (2 == pClip1.use_count());
          ASSERT (2 == pClip2.use_count());
          index.reset();
#endif ////////////////////////////////////////////////////////////////////////////////////////TODO lots of things unimplemented.....!!!!!

        }
      
      
      template<typename REF>
      void
      checkBuildMObjectRef (REF refObj, void* placementAdr)
        {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 384  !!!!!!!!!
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
          ASSERT (3 == rMO.use_count());    // now rMO shares ownership with the Placement
          ASSERT (0 < rMO.getStartTime());  // (internally, this resolves to an ExplicitPlacement)
          ASSERT ( rMO.isCompatible<MObject>());
          ASSERT ( rMO.isCompatible<Clip>());
          ASSERT (!rMO.isCompatible<TestSubMO1>());
          Time start = rMO.getStartTime();
          
          // re-link to the Placement (note we get the Clip API!)
          Placement<Clip> & refP = rMO.getPlacement();
          ASSERT (refP);
          ASSERT (3 == refP.use_count());
          ASSERT (&refP == placementAdr);   // actually denotes the address of the original Placement in the "session"
          cout << string(refP) << endl;
          
          ExplicitPlacement exPla = refP.resolve();
          ASSERT (exPla.time == start);     // recovered Placement resolves to the same time as provided by the proxied API
          ASSERT (4 == refP.use_count());   // but now we've indeed created an additional owner (exPla)
          ASSERT (4 == rMO.use_count());
#endif ////////////////////////////////////////////////////////////////////////////////////////TODO lots of things unimplemented.....!!!!!
        }
      
      void
      checkLifecylce (PMObj const& p1, PMObj const& p2)
        {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 384  !!!!!!!!!
          ASSERT (2 == p1.use_count());
          ASSERT (2 == p2.use_count());
          
          MORef<Clip> rMO;
          ASSERT (!rMO);
          ASSERT (0 == rMO.use_count());
          
          rMO.activate(p1);
          ASSERT (rMO);
          ASSERT (rMO->getMedia()->getFilename() == "test-1");
          ASSERT (3 == rMO.use_count());
          ASSERT (3 == p1.use_count());
          ASSERT (2 == p2.use_count());
          
          rMO.activate(p2);
          ASSERT (rMO);
          ASSERT (rMO->getMedia()->getFilename() == "test-2");
          ASSERT (3 == rMO.use_count());
          ASSERT (2 == p1.use_count());
          ASSERT (3 == p2.use_count());
          
          rMO.activate(p2);
          ASSERT (3 == rMO.use_count());
          
          rMO.close();
          ASSERT (!rMO);
          ASSERT (2 == p1.use_count());
          ASSERT (2 == p2.use_count());
          
          VERIFY_ERROR (INVALID_PLACEMENTREF, rMO.getPlacement() );
          VERIFY_ERROR (INVALID_MOBJECTREF,   rMO->getMedia()    );
#endif ////////////////////////////////////////////////////////////////////////////////////////TODO lots of things unimplemented.....!!!!!
        }
      
      void
      checkTypeHandling (LumieraUid luid)
        {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 384  !!!!!!!!!
          MObjectRef rMObj;
          MORef<Clip> rClip;
          MORef<TestSubMO1> rSub1;
          
          ASSERT (0 == rMObj.use_count());
          ASSERT (0 == rClip.use_count());
          ASSERT (0 == rSub1.use_count());
          
          rMObj.activate(luid);
          ASSERT (3 == rMObj.use_count());
          ASSERT (0 == rClip.use_count());
          ASSERT (0 == rSub1.use_count());
          
          rClip.activate(rMObj);              // attach on existing MObjectRef
          ASSERT (4 == rMObj.use_count());
          ASSERT (4 == rClip.use_count());
          ASSERT (0 == rSub1.use_count());
          
          // impossible, because Clip isn't a subclass of TestSubMO1:
          VERIFY_ERROR (INVALID_PLACEMENTREF, rSub1.activate(luid) );
          VERIFY_ERROR (INVALID_PLACEMENTREF, rSub1 = rMObj        );
          
          ASSERT (rMObj->isValid());
          ASSERT (rClip->isValid());
          ASSERT (rMObj.getPlacement().getID() == rClip.getPlacement().getID());
          
          // doesn't compile, because the function isn't on MObject API:
          // rMObj->getMedia();
          
          // can assign, because the actual type is relevant:
          rClip.close();
          ASSERT (3 == rMObj.use_count());
          ASSERT (0 == rClip.use_count());
          
          rClip = rMObj;
          ASSERT (4 == rMObj.use_count());
          ASSERT (4 == rClip.use_count());
          
          cout << rClip << endl;
          cout << rClip->getMedia()->ident << endl;
#endif ////////////////////////////////////////////////////////////////////////////////////////TODO lots of things unimplemented.....!!!!!
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (MObjectRef_test, "unit session");
  
  
}} // namespace mobject::test
