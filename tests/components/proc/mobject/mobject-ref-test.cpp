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

using lib::test::showSizeof;
using std::string;
using std::cout;
using std::endl;


namespace mobject {
namespace test    {
  
  namespace { // shortcut for checking use-counts
    
    bool
    checkUseCount (size_t cnt, uint additional=0)
    {
      static uint base_count=0;
      if (!additional) // called for init
        base_count = cnt;
      
      return cnt == base_count + additional;
    }
    
    template<class REF>
    bool
    checkUseCount (REF const& ref, uint additional)
    {
      return checkUseCount(ref.use_count(), additional);
    }
  }
  
  
  using lumiera::Time;
  using session::Clip;
  using session::PMedia;
  
  using session::SessionServiceMockIndex;
  using session::PPIdx;
  
  
  
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
          PMObj  testClip1 = asset::Media::create("test-1", asset::VIDEO)->createClip();
          PMObj  testClip2 = asset::Media::create("test-2", asset::VIDEO)->createClip();
          
          // set up a tie to fixed start positions (i.e. "properties of placement")
          testClip1.chain(Time(10));
          testClip2.chain(Time(20));
          
          ASSERT (testClip1->isValid());
          ASSERT (testClip2->isValid());
          ASSERT (2 == testClip1.use_count());  // one by the placement and one by the clip-Asset
          ASSERT (2 == testClip2.use_count());
          
          
          // Prepare an (test)Index
          PPIdx index = SessionServiceMockIndex::install();
          PMO& root = index->getRoot();
          
          // Add the Clips to "session" (here: dummy index)
          PMObj::ID clip1ID = index->insert (testClip1, root);
          PMObj::ID clip2ID = index->insert (testClip2, root);
          ASSERT (2 == index->size());
          
          // use the IDs returned on insertion to fetch
          // language references to the placement instance within the index
          // we'll use these language refs as base to create MObejctRef handles.
          PMObj& pClip1 = index->find(clip1ID);
          PMObj& pClip2 = index->find(clip2ID);
          
          ASSERT (3 == pClip1.use_count());   // clip-Asset, original placement, new placement in index
          ASSERT (3 == pClip2.use_count());
          checkUseCount(pClip1.use_count());  // set ref point for later checks
          
          // extract various kinds of IDs and refs
          PMObj &         rP1 (pClip1);
          PMObj const&    rP2 (pClip2);
          PMObj::ID       id1 = pClip1.getID();
          PMObj::Id<Clip> id2 = pClip2.recastID<Clip>(); // explicit unchecked re-declaration of target type
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
          
          checkComparison(rP1,rP2);
          checkLifecycle (rP1,rP2);
          checkTypeHandling (luid);
          // -----Tests------------------
          
          // verify clean state
          index->remove (pClip1);
          index->remove (pClip2);                // note: this invalidates pClip1 and pClip2;
          ASSERT (!ref1);
          ASSERT (!ref2);
          ASSERT (0 == index->size());
          ASSERT (1 == index.use_count());
          ASSERT (2 == testClip1.use_count());
          ASSERT (2 == testClip2.use_count());
          index.reset();
        }
      
      
      
      template<typename REF>
      void
      checkBuildMObjectRef (REF& refObj, void* placementAdr)
        {
          MORef<Clip> rMO;
          ASSERT (!rMO);                    // still empty (not bound)
          ASSERT (0==rMO.use_count());
          cout << string(rMO)     << endl;                 /////////////////////TICKET #527
          cout << showSizeof(rMO) << endl;
          
          // activate by binding to provided ref
          rMO.activate(refObj);
          ASSERT (rMO);                     // now bound
          cout << string(rMO)     << endl;                 /////////////////////TICKET #527
          
          // access MObject (Clip API)
//        cout << rMO->operator string() << endl;          /////////////////////TICKET #428
          PMedia media = rMO->getMedia();
          cout << str(media) << endl;                      /////////////////////TICKET #520
          Time mediaLength = media->getLength();
          ASSERT (Time(0) < mediaLength);
          ASSERT (rMO->isValid());
          
          // access the Placement-API
          ASSERT (checkUseCount(rMO, 1));          // now rMO shares ownership with the Placement --> use-count += 1
          ASSERT (Time(0) < rMO.getStartTime());  // (internally, this resolves to an ExplicitPlacement)  /////////TICKET #332
          ASSERT ( rMO.isCompatible<MObject>());
          ASSERT ( rMO.isCompatible<Clip>());
          ASSERT (!rMO.isCompatible<TestSubMO1>());
          Time start = rMO.getStartTime();
          
          // re-link to the Placement (note we get the Clip API!)
          Placement<Clip> & refP = rMO.getPlacement();
          ASSERT (refP.isValid());
          ASSERT (refP.use_count() == rMO.use_count());
          ASSERT (checkUseCount(refP, 1));  // use count not changed
          ASSERT (&refP == placementAdr);   // actually denotes the address of the original Placement in the "session"
          cout << string(refP) << endl;
          
          ExplicitPlacement exPla = refP.resolve();
          ASSERT (exPla.time == start);     // recovered Placement resolves to the same time as provided by the proxied API
          ASSERT (checkUseCount(refP, 2));  // but now we've indeed created an additional owner (exPla)
          ASSERT (checkUseCount(rMO, 2));
        }
      
      
      void
      checkComparison (PMO const& p1, PMO const& p2)
        {
          PlacementRef<Clip>    pRef1 (p1);
          PlacementRef<MObject> pRef2 (p2);
          
          MORef<MObject> rM;
          MORef<Clip>    rC;
          
          rM.activate (p1);
          rC.activate (p2);
          ASSERT (rM && rC);
          ASSERT (!(rM == rC) && !(rC == rM));
          ASSERT ( (rM != rC) &&  (rC != rM));
          
          // mixed comparisons
          ASSERT ( (rM == pRef1) &&  (pRef1 == rM));
          ASSERT ( (rC == pRef2) &&  (pRef2 == rC));
          ASSERT (!(rM != pRef1) && !(pRef1 != rM));
          ASSERT (!(rC != pRef2) && !(pRef2 != rC));
          ASSERT ( (rM != pRef2) &&  (pRef2 != rM));
          ASSERT ( (rC != pRef1) &&  (pRef1 != rC));
          ASSERT (!(rM == pRef2) && !(pRef2 == rM));
          ASSERT (!(rC == pRef1) && !(pRef1 == rC));
          
          ASSERT ( (rM == p1.getID()) );
          ASSERT ( (rC == p2.getID()) );
          ASSERT (!(rM != p1.getID()) );
          ASSERT (!(rC != p2.getID()) );
          ASSERT ( (rM != p2.getID()) );
          ASSERT ( (rC != p1.getID()) );
          ASSERT (!(rM == p2.getID()) );
          ASSERT (!(rC == p1.getID()) );
          
          
          rC.activate (pRef1);
          ASSERT ( (rM == rC) &&  (rC == rM));
          ASSERT (!(rM != rC) && !(rC != rM));
          
          ASSERT ( (rC == pRef1) &&  (pRef1 == rC));
          ASSERT (!(rC != pRef1) && !(pRef1 != rC));
          ASSERT ( (rC != pRef2) &&  (pRef2 != rC));
          ASSERT (!(rC == pRef2) && !(pRef2 == rC));
          
          ASSERT ( (rC == p1.getID()) );
          ASSERT (!(rC != p1.getID()) );
          ASSERT ( (rC != p2.getID()) );
          ASSERT (!(rC == p2.getID()) );
          
          
          rM.close();
          ASSERT (!rM);
          ASSERT (!(rM == rC) && !(rC == rM));
          ASSERT ( (rM != rC) &&  (rC != rM));
          
          ASSERT (!(rM == pRef1) && !(pRef1 == rM));
          ASSERT ( (rM != pRef1) &&  (pRef1 != rM));
          ASSERT ( (rM != pRef2) &&  (pRef2 != rM));
          ASSERT (!(rM == pRef2) && !(pRef2 == rM));
          
          ASSERT (!(rM == p1.getID()) );
          ASSERT ( (rM != p1.getID()) );
          ASSERT ( (rM != p2.getID()) );
          ASSERT (!(rM == p2.getID()) );
        }
      
      
      void
      checkLifecycle (PMObj const& p1, PMObj const& p2)
        {
          ASSERT (checkUseCount(p1, 0));
          ASSERT (checkUseCount(p2, 0));
          
          MORef<Clip> rMO;
          ASSERT (!rMO);
          ASSERT (0 == rMO.use_count());
          
          rMO.activate(p1);
          ASSERT (rMO);
          ASSERT (rMO->getMedia()->getFilename() == "test-1");
          ASSERT (checkUseCount(rMO, 1));
          ASSERT (checkUseCount(p1,  1)); // sharing ownership
          ASSERT (checkUseCount(p2,  0));
          
          rMO.activate(p2);
          ASSERT (rMO);
          ASSERT (rMO->getMedia()->getFilename() == "test-2");
          ASSERT (checkUseCount(rMO, 1));
          ASSERT (checkUseCount(p1,  0)); // detached, use count dropped to previous value
          ASSERT (checkUseCount(p2,  1)); // sharing ownership
          
          rMO.activate(p2);
          ASSERT (checkUseCount(rMO, 1)); // no change
          
          rMO.close();
          ASSERT (!rMO);
          ASSERT (checkUseCount(p1,  0));
          ASSERT (checkUseCount(p2,  0));
          
          VERIFY_ERROR (BOTTOM_MOBJECTREF, rMO.getPlacement() );
          VERIFY_ERROR (BOTTOM_MOBJECTREF, rMO->getMedia()    );
        }
      
      void
      checkTypeHandling (LumieraUid luid)
        {
          MObjectRef rMObj;
          MORef<Clip> rClip;
          MORef<TestSubMO1> rSub1;
          
          ASSERT ( ! rMObj.use_count());
          ASSERT ( ! rClip.use_count());
          ASSERT ( ! rSub1.use_count());
          
          rMObj.activate(luid);
          ASSERT (checkUseCount(rMObj, 1));
          ASSERT ( ! rClip.use_count());
          ASSERT ( ! rSub1.use_count());
          
          rClip.activate(rMObj);              // attach on existing MObjectRef
          ASSERT (checkUseCount(rMObj, 2));
          ASSERT (checkUseCount(rClip, 2));
          ASSERT ( ! rSub1.use_count());
          
          // impossible, because Clip isn't a subclass of TestSubMO1:
          VERIFY_ERROR (INVALID_PLACEMENTREF, rSub1.activate(luid) );
          VERIFY_ERROR (INVALID_PLACEMENTREF, rSub1 = rMObj        );
          
          ASSERT (rMObj->isValid());
          ASSERT (rClip->isValid());
          ASSERT (rMObj.getPlacement().getID() == rClip.getPlacement().getID());
          
          // doesn't compile, because the function isn't on MObject API:
          // rMObj->getMedia();
          
          rClip.close();
          ASSERT (checkUseCount(rMObj, 1));
          ASSERT ( ! rClip.use_count());
          
          // can assign, because the actual type is checked:
          rClip = rMObj;
          ASSERT (checkUseCount(rMObj, 2));
          ASSERT (checkUseCount(rClip, 2));
          
          cout << string(rClip) << endl;                         //////////TICKET #527
          cout << string(rClip->getMedia()->ident) << endl;      //////////TICKET #520
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (MObjectRef_test, "unit session");
  
  
}} // namespace mobject::test
