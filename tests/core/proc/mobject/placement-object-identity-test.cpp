/*
  PlacementObjectIdentity(Test)  -  object identity for placements into the session

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file placement-object-identity-test.cpp
 ** unit test \ref PlacementObjectIdentity_test
 */


#include "lib/test/run.hpp"
#include "proc/asset/media.hpp"
#include "proc/mobject/mobject.hpp"
#include "proc/mobject/mobject-ref.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/placement-ref.hpp"
#include "proc/mobject/session/placement-index.hpp"
#include "proc/mobject/session/session-service-mock-index.hpp"
#include "proc/mobject/session/clip.hpp"
#include "proc/mobject/explicitplacement.hpp"
#include "lib/time/timevalue.hpp"
//#include "proc/mobject/test-dummy-mobject.hpp"
//#include "lib/test/test-helper.hpp"



namespace proc    {
namespace mobject {
namespace test    {
  
  using session::Clip;
  
  using session::SessionServiceMockIndex;  
  

  
  /***********************************************************************//**
   * @test verify correct handling of object identity. Create test objects,
   *       add placements to a dummy index/session, then create PlacementRef
   *       and MObjectRef handles. Finally do cross comparisons.
   *       
   * @todo WIP-WIP to be written...  TICKET #517
   *        
   * @see  mobject::PlacementRef_test
   * @see  mobject::MObjectRef_test
   */
  class PlacementObjectIdentity_test : public Test
    {
      
      typedef Placement<MObject>        PMObj;
      typedef Placement<Clip>           PClip;

      typedef PlacementMO::ID       PMObjID;
      typedef PlacementMO::Id<Clip> PClipID;

      
      virtual void
      run (Arg) 
        {
          
          // create data simulating a "Session"
          PMObj  pClip1 = asset::Media::create("test-1", asset::VIDEO)->createClip();
          PMObj  pClip2 = asset::Media::create("test-2", asset::VIDEO)->createClip();
          
          // set up a tie to fixed start positions
          pClip1.chain (lib::time::Time(0,10));
          pClip2.chain (lib::time::Time(0,20));
          
          CHECK (pClip1->isValid());
          CHECK (pClip2->isValid());
          CHECK (2 == pClip1.use_count());  // one by the placement and one by the clip-Asset
          CHECK (2 == pClip2.use_count());
          
          ////////////////TODO direct comparisons
          ////////////////TODO direct comparisons
          
          UNIMPLEMENTED ("verify proper handling of object identity");
          
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 517  !!!!!!!!!!!!!!
          // Prepare an (test)Index
          PPIdx index = SessionServiceMockIndex::install();
          PMO& root = index->getRoot();
          
          // Add the Clips to "session"
          PMObjID clip1ID = index->insert (pClip1, root);
          PMObjID clip2ID = index->insert (pClip2, root);

          ////////////////TODO compare ids
          ////////////////TODO compare ids

          
          // extract various kinds of IDs and refs
          PMObj &         rP1 (pClip1);
          PMObj const&    rP2 (pClip2);
          PMObj::ID       id1 = pClip1.getID();
          PMObj::Id<Clip> id2 = pClip2.getID();
          LumieraUid luid = id1.get();
          
          PlacementRef<Clip>    ref1 (id1);
          PlacementRef<MObject> ref2 (pClip2);
          
          MORef<Clip> rMO;
          CHECK (!rMO);                    // still empty (not bound)
          
          // activate by binding to provided ref
          rMO.activate(refObj);
          CHECK (rMO);                     // now bound
          
          ///TODO access Placement, fetch ID
          
          // re-link to the Placement (note we get the Clip API!)
          Placement<Clip> & refP = rMO.getPlacement();
          CHECK (refP);
          CHECK (3 == refP.use_count());
          CHECK (&refP == placementAdr);   // actually denotes the address of the original Placement in the "session"
          
          ExplicitPlacement exPla = refP.resolve();
          CHECK (exPla.time == start);     // recovered Placement resolves to the same time as provided by the proxied API
          CHECK (4 == refP.use_count());   // but now we've indeed created an additional owner (exPla)
          CHECK (4 == rMO.use_count());
          
          
          ////////////////TODO now do the cross comparisons
          ////////////////TODO now do the cross comparisons
          
          
          ///clean up...
          index.reset();

#endif ////////////////////////////////////////////////     ////////////////////////////////////////////////////TODO to be written........!!!!!
        }
      
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (PlacementObjectIdentity_test, "function session");
  
  
}}} // namespace proc::mobject::test
